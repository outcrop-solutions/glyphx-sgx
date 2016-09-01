
#include "sceneviewer.h"
#include <ctime>
#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtWidgets/QMessageBox>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/color_space.hpp>
#include "hal/hal.h"
#include "hal/vertex_format.h"
#include "render/perspective_camera.h"
#include "render/ortho_camera.h"
#include "render/model_loader.h"
#include "render/model.h"
#include "render/renderer.h"
#include "render/grid_renderer.h"
#include "glyphnode.h"
#include "glyphgeometrydb.h"
#include "placementpolicy.h"
#include "axisrenderer.h"
#include "baseimagerenderer.h"
#include "legacyscenereader.h"
#include "freecameracontroller.h"
#include "orbitcameracontroller.h"
#include "overheadcameracontroller.h"

namespace SynGlyphX
{
	namespace
	{
		const unsigned int navigationButtonSize = 24;
		const float buttonRotateRate = 1.f;
		const float buttonMoveUpDownRate = 0.5f;
		const float buttonMoveForwardBackRate = 1.f;
		const float flyToObjectDistance = 5.f;
	}

	SceneViewer::SceneViewer( QWidget *parent )
		: QOpenGLWidget( parent ), hud_font( "Arial", 12, QFont::Normal ), glyph_renderer( nullptr ),
		renderer( nullptr ), wireframe( false ), enable_fly_to_object( false ), scene_axes_enabled( true ), wheel_delta( 0.f ),
		hud_axes_enabled( true ), hud_axes_location( HUDAxesLocation::TopLeft ), animation_enabled( true ), background_color( render::color::black() ),
		initialized( false )
	{
		memset( key_states, 0, sizeof( key_states ) );

		setMouseTracking( true );
		setFocusPolicy( Qt::FocusPolicy::StrongFocus );

		m_upRotateButton = CreateNavigationButton( tr( "Rotate Up" ), true );
		m_upRotateButton->setIcon( QIcon( ":GlyphViewer/Resources/rotate_up.png" ) );
		QObject::connect( m_upRotateButton, &QToolButton::pressed, this, [this]() { cur_cam_control->turn( glm::vec2( 0.f, -buttonRotateRate ) ); } );

		m_leftRotateButton = CreateNavigationButton( tr( "Rotate Left" ), true );
		m_leftRotateButton->setIcon( QIcon( ":GlyphViewer/Resources/rotate_left.png" ) );
		QObject::connect( m_leftRotateButton, &QToolButton::pressed, this, [this]() { cur_cam_control->turn( glm::vec2( -buttonRotateRate, 0.f ) ); } );

		m_rightRotateButton = CreateNavigationButton( tr( "Rotate Right" ), true );
		m_rightRotateButton->setIcon( QIcon( ":GlyphViewer/Resources/rotate_right.png" ) );
		QObject::connect( m_rightRotateButton, &QToolButton::pressed, this, [this]() { cur_cam_control->turn( glm::vec2( buttonRotateRate, 0.f ) ); } );

		m_downRotateButton = CreateNavigationButton( tr( "Rotate Down" ), true );
		m_downRotateButton->setIcon( QIcon( ":GlyphViewer/Resources/rotate_down.png" ) );
		QObject::connect( m_downRotateButton, &QToolButton::pressed, this, [this]() { cur_cam_control->turn( glm::vec2( 0.f, buttonRotateRate ) ); } );

		auto move = [&]( const glm::vec3& dir, float amt )
		{
			if ( cur_cam_control == free_cam_control )
				cur_cam_control->move( dir * amt );
			else if ( cur_cam_control == orbit_cam_control )
				cur_cam_control->move( glm::vec3( 0.f, 0.f, amt ) );
		};

		m_moveForwardButton = CreateNavigationButton( tr( "Move Forward" ), true );
		m_moveForwardButton->setIcon( QIcon( ":SGXGUI/Resources/plus.png" ) );
		QObject::connect( m_moveForwardButton, &QToolButton::pressed, this, [this, move]() { move( camera->get_forward(), buttonMoveForwardBackRate ); } );

		m_moveBackwardButton = CreateNavigationButton( tr( "Move Backward" ), true );
		m_moveBackwardButton->setIcon( QIcon( ":SGXGUI/Resources/minus.png" ) );
		QObject::connect( m_moveBackwardButton, &QToolButton::pressed, this, [this, move]() { move( camera->get_forward(), -buttonMoveForwardBackRate ); } );

		m_moveUpButton = CreateNavigationButton( tr( "Move Up" ), true );
		m_moveUpButton->setIcon( QIcon( ":SGXGUI/Resources/up_arrow.png" ) );
		QObject::connect( m_moveUpButton, &QToolButton::pressed, this, [this, move]() { move( camera->get_world_up(), buttonMoveUpDownRate ); } );

		m_moveDownButton = CreateNavigationButton( tr( "Move Down" ), true );
		m_moveDownButton->setIcon( QIcon( ":SGXGUI/Resources/down_arrow.png" ) );
		QObject::connect( m_moveDownButton, &QToolButton::pressed, this, [this, move]() { move( camera->get_world_up(), -buttonMoveUpDownRate ); } );

		desired_selection_dist = flyToObjectDistance;
	}

	SceneViewer::~SceneViewer()
	{
		if ( initialized )
		{
			makeCurrent();
			GlyphGeometryDB::clear();
			delete glyph_renderer;
			delete axis_renderer;
			delete base_images;
			delete renderer;
			delete grids;
			delete free_cam_control;
			delete orbit_cam_control;
			delete overhead_cam_control;
			delete camera;
#ifdef DRAW_BOUNDS
			delete bound_sphere;
#endif
			delete logo;
			delete plane;
			hal::device::release( tex_effect );
			hal::device::release( drag_effect );
			delete q_sgx_logo;
			for ( auto& tex : base_textures )
			{
				hal::device::release( tex.second );
				delete tex.first;
			}
			hal::device::release( default_base_texture.second );
			delete default_base_texture.first;
			hal::device::shutdown();
		}
	}

	void SceneViewer::loadLegacyScene( const char* nodeFile, const char* tagFile, std::vector<std::string> baseImages )
	{
		makeCurrent();

		// Clear out current scene.
		scene.clear();
		base_images->clear();
		grids->clear();
		GlyphGeometryDB::reset();
		resetCamera();

		// Load textures for new scene (should eventually move into scene loader).
		std::vector<hal::texture*> textures;
		for ( auto img : baseImages )
		{
			auto tex = load_texture( img.c_str() );
			base_textures.push_back( tex );
			textures.push_back( tex.second );
		}

		SynGlyphX::LegacySceneReader::LoadLegacyScene( getScene(), *base_images, *grids, default_base_texture.second, nodeFile, tagFile, textures );
	}

	void SceneViewer::clearScene()
	{
		scene.clear();
		base_images->clear();
		grids->clear();
		for ( int i = 0; i < 3; ++i )
			axis_names[i] = "";
	}

	std::pair<QOpenGLTexture*, hal::texture*> SceneViewer::load_texture( const char* filename )
	{
		// Slightly clunky way to use Qt to load textures for our renderer. Eventually should implement
		// freeimage or similar to load textures without involving Qt at all.
		QOpenGLTexture* qtex = new QOpenGLTexture( QImage( filename ) );
		auto texid = qtex->textureId();
		hal::texture* tex = hal::device::register_external_texture( &texid );
		return std::make_pair( qtex, tex );
	}

	QToolButton* SceneViewer::CreateNavigationButton( const QString& toolTip, bool autoRepeat ) {

		QToolButton* navButton = new QToolButton( this );
		navButton->setFixedSize( navigationButtonSize, navigationButtonSize );
		navButton->setAutoRepeat( autoRepeat );
		navButton->setToolTip( toolTip );
		navButton->setAutoRepeatDelay( 10 );
		navButton->setAutoRepeatInterval( 10 );

		return navButton;
	}

	void SceneViewer::initializeGL()
	{
		if ( format().majorVersion() < 3 || ( format().majorVersion() == 3 && format().minorVersion() < 3 ) )
		{
			QString error = QString( "Unable to create OpenGL 3.3 or higher core context (got %1.%2)." ).arg(
				QString::number( format().majorVersion() ), QString::number( format().minorVersion() ) );
			QMessageBox::critical( this, "OpenGL Error", error );
		}

		// Set up timer to attempt to trigger repaints at ~60fps.
		timer.setInterval( 16 );
		timer.setSingleShot( false );
		connect( &timer, SIGNAL( timeout() ), SLOT( updateFrame() ) );
		timer.start();

		elapsed_timer.start();

		hal::device::set_break_on_error( true );

		bool device_init_result = hal::device::init();
		hal::device::add_forced_effect_include( "shaders/header.h" );
		hal::device::add_forced_effect_include( "shaders/util.h" );
		if ( !device_init_result ) qDebug() << "SynGlyphX::hal::device::init() failed.";
		context = hal::device::get_default_context();
		checkErrors();

		grids = new render::grid_renderer;
		axis_renderer = new AxisRenderer;
		base_images = new BaseImageRenderer;

		camera = new render::perspective_camera();
		camera->set_clip_planes( 1.f, 2048.f );

		free_cam_control = new FreeCameraController( camera );
		overhead_cam_control = new OverheadCameraController( camera );
		orbit_cam_control = new OrbitCameraController( camera );
		orbit_cam_control->setOrbitMaxDistance( camera->get_far_clip() * 0.75f );
		resetCamera();

		ui_camera = new render::ortho_camera( 512u, 512u, -1024.f, 1024.f );

		tex_effect = hal::device::create_effect( "shaders/texture.vert", nullptr, "shaders/texture.frag" );
		drag_effect = hal::device::create_effect( "shaders/drag_select.vert", nullptr, "shaders/drag_select.frag" );

		default_base_texture = load_texture( "DefaultBaseImages/World.png" );

		float square[]
		{
			0.f, 0.f, 0.f, 0.f, 1.f,
			1.f, 0.f, 0.f, 1.f, 1.f,
			1.f, 0.f, 1.f, 1.f, 0.f,
			0.f, 0.f, 1.f, 0.f, 0.f,
		};
		unsigned int square_indices[]
		{
			0u, 1u, 2u, 0u, 2u, 3u
		};

		hal::vertex_format fmt;
		fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3, hal::stream_semantic::position, 0 ) );
		fmt.add_stream( hal::stream_info( hal::stream_type::float32, 2, hal::stream_semantic::texcoord, 0 ) );

		q_sgx_logo = new QOpenGLTexture( QImage( "logo.png" ) );
		auto logoid = q_sgx_logo->textureId();
		sgx_logo = hal::device::register_external_texture( &logoid );

		auto plane_mesh = hal::device::create_mesh( fmt, hal::primitive_type::triangle_list, 4, square, 2, square_indices );

		auto scale = glm::scale( glm::mat4(), glm::vec3( 3000.f, 1.f, 1500.f ) );
		auto translate = glm::translate( glm::mat4(), glm::vec3( -1500.f, -50.f, 0.f ) );
		render::model_part* part = new render::model_part( plane_mesh, translate * scale, render::sphere_bound( glm::vec3(), 5000.f ) /* not real bound */, "plane" );
		plane = new render::model;
		plane->add_part( part );

		part = new render::model_part( plane_mesh, glm::mat4(), "sgx logo" );
		logo = new render::model;
		logo->add_part( part );

		part = new render::model_part( plane_mesh, glm::mat4(), "drag select" );
		drag_select = new render::model;
		drag_select->add_part( part );

		glyph_renderer = new GlyphRenderer;
		glyph_renderer->setScene( &scene );
		renderer = new render::renderer;

		GlyphGeometryDB::init();

		initialized = true;
	}

	void SceneViewer::resizeGL( int w, int h )
	{
		glm::mat4 view, proj;
		camera->update_viewport_size( w, h );
		ui_camera->update_viewport_size( w, h );

		auto logo_rotate = glm::rotate( glm::mat4(), glm::half_pi<float>(), glm::vec3( 1.f, 0.f, 0.f ) );
		auto logo_translate = glm::translate( glm::mat4(), glm::vec3( float( w - q_sgx_logo->width() ), float( q_sgx_logo->height() ), 0.f ) );
		auto logo_scale = glm::scale( glm::mat4(), glm::vec3( q_sgx_logo->width(), q_sgx_logo->height(), 1.f ) );
		auto logo_transform = logo_translate * logo_scale * logo_rotate;
		logo->set_transform( logo_transform );

		checkErrors();

		unsigned int leftPosOfButtonsInHCenter = w - 10 - ( 2 * navigationButtonSize );
		unsigned int topPositionOfButton = std::abs( q_sgx_logo->height() ) + ( 2 * 10 );

		m_upRotateButton->move( QPoint( leftPosOfButtonsInHCenter, topPositionOfButton ) );

		topPositionOfButton += navigationButtonSize;
		m_leftRotateButton->move( QPoint( leftPosOfButtonsInHCenter - navigationButtonSize, topPositionOfButton ) );
		m_rightRotateButton->move( QPoint( leftPosOfButtonsInHCenter + navigationButtonSize, topPositionOfButton ) );

		topPositionOfButton += navigationButtonSize;
		m_downRotateButton->move( QPoint( leftPosOfButtonsInHCenter, topPositionOfButton ) );

		topPositionOfButton += ( 2 * navigationButtonSize );
		m_moveForwardButton->move( QPoint( leftPosOfButtonsInHCenter, topPositionOfButton ) );

		topPositionOfButton += navigationButtonSize;
		m_moveBackwardButton->move( QPoint( leftPosOfButtonsInHCenter, topPositionOfButton ) );

		topPositionOfButton += ( 2 * navigationButtonSize );
		m_moveUpButton->move( QPoint( leftPosOfButtonsInHCenter, topPositionOfButton ) );

		topPositionOfButton += navigationButtonSize;
		m_moveDownButton->move( QPoint( leftPosOfButtonsInHCenter, topPositionOfButton ) );
	}

	glm::vec3 SceneViewer::compute_scene_axis_sizes()
	{
		const render::box_bound& bound = render::combine_bounds( scene.get_bound(), base_images->get_base_image_bound( 0u ) );
		return glm::vec3( bound.get_max().x - bound.get_min().x, bound.get_max().y - bound.get_min().y, bound.get_max().z - bound.get_min().z );
	}

	glm::vec3 SceneViewer::compute_scene_axis_origin()
	{
		const render::box_bound& bound = render::combine_bounds( scene.get_bound(), base_images->get_base_image_bound( 0u ) );
		return glm::vec3( bound.get_min() );
	}

	glm::vec3 SceneViewer::compute_hud_axis_origin()
	{
		const glm::vec3 hud_axes_offset( 100.f, 100.f, 0.f );
		glm::vec3 pos = hud_axes_offset;
		if ( hud_axes_location == HUDAxesLocation::BottomLeft || hud_axes_location == HUDAxesLocation::BottomRight ) pos.y = height() - hud_axes_offset.y;
		if ( hud_axes_location == HUDAxesLocation::BottomRight ) pos.x = width() - hud_axes_offset.x;
		return pos;
	}

	void SceneViewer::paintGL()
	{
		assert( elapsed_timer.isValid() );
		QPainter painter( this );
		if ( format().profile() == QSurfaceFormat::CompatibilityProfile ) painter.beginNativePainting();

		glyph_renderer->enableAnimation( animation_enabled );

		hal::rasterizer_state rast{ true, true, false };
		context->set_rasterizer_state( rast );

		context->clear( hal::clear_type::color, background_color );

		// Compute some axis information beforehand since we'll need it in a few places.
		glm::vec2 cam_xy( camera->get_forward().x, camera->get_forward().y );
		float angle = atan2f( cam_xy.x, cam_xy.y );

		glm::vec3 scene_axis_sizes = compute_scene_axis_sizes();
		glm::vec3 scene_axis_origin = compute_scene_axis_origin();
		glm::vec3 hud_axes_origin = compute_hud_axis_origin();
		glm::mat4 hud_axes_rotation = glm::rotate( glm::mat4(), glm::half_pi<float>() * -0.25f, glm::vec3( 1.f, 0.f, 0.f ) ) * glm::rotate( glm::mat4(), angle, glm::vec3( 0.f, 1.f, 0.f ) );
		const glm::vec3 hud_axes_offset( 100.f, 100.f, 0.f );
		const float hud_axes_size = 48.f;

		if ( !scene.empty() )
		{
			base_images->draw( context, camera );

			// Draw the actual glyphs.
			renderer->set_wireframe( wireframe );
			renderer->render( context, camera );
			glyph_renderer->setWireframeMode( wireframe );
			glyph_renderer->render_solid( context, camera, float( elapsed_timer.elapsed() ) / 1000.f );

			// Draw the scene axes.
			if ( scene_axes_enabled )
			{
				axis_renderer->set_scales( 2.0f, glm::vec3( 6.f, 6.f, 8.f ) );
				axis_renderer->draw_axis( context, camera, render::color::red(), AxisDirection::X, scene_axis_origin, scene_axis_sizes.x );
				axis_renderer->draw_axis( context, camera, render::color::green(), AxisDirection::Y, scene_axis_origin, scene_axis_sizes.y );
				axis_renderer->draw_axis( context, camera, render::color::blue(), AxisDirection::Z, scene_axis_origin, scene_axis_sizes.z );
			}

			// Draw the grids.
			grids->draw( context, camera, render::color::white() );

			// Clear depth so it doesn't interfere with screen-space elements.
			context->clear( hal::clear_type::depth );

			if ( hud_axes_enabled )
			{
				context->set_depth_state( hal::depth_state::read_write );
				axis_renderer->set_scales( 2.f, glm::vec3( 8.f, 8.f, 12.f ) );
				axis_renderer->draw_axis( context, ui_camera, render::color::red(), AxisDirection::X, hud_axes_origin, hud_axes_size, hud_axes_rotation, true );
				axis_renderer->draw_axis( context, ui_camera, render::color::blue(), AxisDirection::Y, hud_axes_origin, hud_axes_size, hud_axes_rotation, true );
				axis_renderer->draw_axis( context, ui_camera, render::color::green(), AxisDirection::Z, hud_axes_origin, hud_axes_size, hud_axes_rotation, true );
			}

			glyph_renderer->render_blended( context, camera, float( elapsed_timer.elapsed() ) / 1000.f );
		}

		// States for blended overlay elements.
		context->set_depth_state( hal::depth_state::disabled );
		context->set_blend_state( hal::blend_state::alpha );

		// draw drag-select
		if ( drag_info( button::left ).dragging && ( QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ShiftModifier ) )
		{
			bool alt = ( QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::AltModifier );
			glm::vec3 origin( std::min( drag_info( button::left ).drag_start_x, mouse_x ), std::max( drag_info( button::left ).drag_start_y, mouse_y ), 0.f );
			glm::vec3 size( fabs( float( mouse_x - drag_info( button::left ).drag_start_x ) ), fabs( float( mouse_y - drag_info( button::left ).drag_start_y ) ), 0.f );
			auto drag_rotate = glm::rotate( glm::mat4(), glm::half_pi<float>(), glm::vec3( 1.f, 0.f, 0.f ) );
			auto drag_translate = glm::translate( glm::mat4(), origin );
			auto drag_scale = glm::scale( glm::mat4(), size );
			drag_select->set_transform( drag_translate * drag_scale * drag_rotate );
			context->bind( drag_effect );
			const glm::vec4 select_color = glm::vec4( 1.f, 1.f, 1.f, 0.25f );
			const glm::vec4 unselect_color = glm::vec4( 1.f, 0.f, 0.f, 0.25f );
			renderer->add_blended_batch( drag_select, drag_effect, glm::mat4(), alt ? unselect_color : select_color );
			renderer->render( context, ui_camera );
		}

		// draw logo
		context->bind( tex_effect );
		context->bind( 0u, sgx_logo );
		context->set_constant( tex_effect, "material", "tint_color", glm::vec4( 1.f, 1.f, 1.f, 1.f ) );
		renderer->add_blended_batch( logo, tex_effect );
		renderer->render( context, ui_camera );

		checkErrors();

		if ( format().profile() == QSurfaceFormat::CompatibilityProfile )
		{
			context->reset_defaults();
			painter.endNativePainting();

			if ( !scene.empty() )
			{
				if ( scene.selectionEmpty() )
				{
					auto campos = camera->get_position();
					QString positionHUD = tr( "Camera Position: X: %1, Y: %2, Z: %3" ).arg( QString::number( campos.x ), QString::number( campos.y ), QString::number( campos.z ) );
					QFontMetrics hudFontMetrics( hud_font );
					renderText( painter, ( width() / 2 ) - ( hudFontMetrics.width( positionHUD ) / 2 ), height() - 16, positionHUD, hud_font );
				}
				else
				{
					QString positionHUD = tr( "Object Position: X: %1, Y: %2, Z: %3" ).arg( QString::number( selection_center.x ), QString::number( selection_center.y ), QString::number( selection_center.z ) );
					QFontMetrics hudFontMetrics( hud_font );
					renderText( painter, ( width() / 2 ) - ( hudFontMetrics.width( positionHUD ) / 2 ), height() - 16, positionHUD, hud_font );
				}
			}

			// Draw tags.
			scene.enumTagEnabled( [&painter, this]( const Glyph3DNode& glyph ) {
				if ( glyph.getTag() ) renderText( painter, glyph.getCachedPosition(), glyph.getTag(), hud_font );
			} );

			// Draw axis names.
			if ( hud_axes_enabled )
			{
				const float axis_name_offset = 16.f;
				if ( axis_names[0] != "" ) renderText( painter, ui_camera, hud_axes_origin + ( hud_axes_size + axis_name_offset ) * glm::vec3( ( hud_axes_rotation * glm::vec4( 1.f, 0.f, 0.f, 0.f ) ) ), QString::fromStdString( axis_names[0] ), hud_font );
				if ( axis_names[1] != "" ) renderText( painter, ui_camera, hud_axes_origin + ( hud_axes_size + axis_name_offset ) * glm::vec3( ( hud_axes_rotation * glm::vec4( 0.f, 0.f, 1.f, 0.f ) ) ), QString::fromStdString( axis_names[1] ), hud_font );
				if ( axis_names[2] != "" ) renderText( painter, ui_camera, hud_axes_origin - ( hud_axes_size + axis_name_offset ) * glm::vec3( ( hud_axes_rotation * glm::vec4( 0.f, 1.f, 0.f, 0.f ) ) ), QString::fromStdString( axis_names[2] ), hud_font );
			}
			
			if ( scene_axes_enabled )
			{
				const float axis_name_offset = 10.f;
				if ( axis_names[0] != "" ) renderText( painter, camera, scene_axis_origin + ( scene_axis_sizes.x + axis_name_offset ) * glm::vec3( 1.f, 0.f, 0.f ), QString::fromStdString( axis_names[0] ), hud_font );
				if ( axis_names[1] != "" ) renderText( painter, camera, scene_axis_origin + ( scene_axis_sizes.y + axis_name_offset ) * glm::vec3( 0.f, 1.f, 0.f ), QString::fromStdString( axis_names[1] ), hud_font );
				if ( axis_names[2] != "" ) renderText( painter, camera, scene_axis_origin + ( scene_axis_sizes.z + axis_name_offset ) * glm::vec3( 0.f, 0.f, 1.f ), QString::fromStdString( axis_names[2] ), hud_font );
			}

			painter.end();
		}
	}

	void SceneViewer::checkErrors()
	{
		// HAL errors.
		if ( hal::log::has_errors() )
			qDebug() << "HAL Error(s):";
		while ( hal::log::has_errors() )
		{
			auto err = hal::log::next_error();
			qDebug() << err.first.c_str();
			if ( err.second.length() > 0 )
				qDebug() << "  " << err.second.c_str();
		}
	}

	void SceneViewer::showTagsOfSelectedObjects( bool show )
	{
		scene.enumSelected( [this, show]( const Glyph3DNode& glyph ) {
			if ( show )
				scene.enableTag( &glyph );
			else
				scene.disableTag( &glyph );
		} );
	}

	void SceneViewer::hideAllTags()
	{
		scene.disableAllTags();
	}

	void SceneViewer::renderText( QPainter& painter, int x, int y, const QString& str, const QFont& font )
	{
		// QPainter doesn't support core profiles, so if we're using one we can't render text this way (it will
		// cause OpenGL errors). But we want the option to use a core profile for GPU debugging etc.
		if ( format().profile() == QSurfaceFormat::CompatibilityProfile )
		{
			painter.setPen( Qt::white );
			painter.setFont( font );
			painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );
			painter.drawText( x, y, str );
		}
	}

	void SceneViewer::renderText( QPainter& painter, const render::camera* cam, const glm::vec3& pos, const QString& str, const QFont& font )
	{
		auto window_pt = cam->world_pt_to_window_pt( pos );

		if ( !cam->pt_behind_camera( pos ) && window_pt.x >= 0.f && window_pt.y > 0.f && window_pt.x < float( width() ) && window_pt.y < float( height() ) )
			renderText( painter, int( window_pt.x ), height() - int( window_pt.y ), str, font );
	}

	void SceneViewer::renderText( QPainter& painter, const glm::vec3& pos, const QString& str, const QFont& font )
	{
		renderText( painter, camera, pos, str, font );
	}

	void SceneViewer::keyPressEvent( QKeyEvent* event )
	{
		if ( event->key() < key_states_size ) key_states[tolower( event->key() )] = true;
		QWidget::keyPressEvent( event );

#ifdef _DEBUG
		auto key = event->key();
		switch ( key )
		{
			case 'R': hal::device::rebuild_effects(); break;
			case 'B': glyph_renderer->enableBoundVis( !glyph_renderer->boundVisEnabled() ); break;
			case 'P':
				auto sel = scene.getSingleSelection();
				if ( scene.selectionSize() > 1 )
					hal::debug::print( "Multiple objects selected; printing hierarchy from first." );
				if ( sel ) scene.debugPrint( sel );
/*			case ' ':
				if ( cur_cam_control == overhead_cam_control )
					set_cam_control( free_cam_control );
				else
					set_cam_control( overhead_cam_control );
				break;*/
		}
#endif
	}

	void SceneViewer::keyReleaseEvent( QKeyEvent* event )
	{
		if ( event->key() < key_states_size ) key_states[tolower( event->key() )] = false;
		QWidget::keyReleaseEvent( event );
	}

	void SceneViewer::wheelEvent( QWheelEvent* event )
	{
		wheel_delta = event->angleDelta().y();
	}

	void SceneViewer::mouseMoveEvent( QMouseEvent* event )
	{
		mouse_x = event->x();
		mouse_y = event->y();

		for ( int i = 0; i < int( button::count ); ++i )
		{
			if ( drag[i].dragging )
			{
				drag[i].drag_delta_x = mouse_x - mouse_prev_x;
				drag[i].drag_delta_y = mouse_y - mouse_prev_y;
				drag[i].drag_distance_x += abs( drag[i].drag_delta_x );
				drag[i].drag_distance_y += abs( drag[i].drag_delta_y );

				mouse_prev_x = event->x();
				mouse_prev_y = event->y();
			}
		}
	}

	void SceneViewer::mousePressEvent( QMouseEvent* event )
	{
		auto set_button = [&]( int i )
		{
			auto& b = drag[i];
			b.drag_start_x = mouse_prev_x = event->x();
			b.drag_start_y = mouse_prev_y = event->y();
			b.dragging = true;
			b.drag_distance_x = 0;
			b.drag_distance_y = 0;
		};

		if ( event->buttons() & Qt::LeftButton ) set_button( int( button::left ) );
		if ( event->buttons() & Qt::RightButton ) set_button( int( button::right ) );
		if ( event->buttons() & Qt::MiddleButton ) set_button( int( button::middle ) );

		// prevent context menu from coming up unless ONLY right button is clicked
		// (otherwise it comes up when doing L/R-drag)
		if ( event->buttons() == Qt::RightButton )
			setContextMenuPolicy( Qt::ActionsContextMenu );
		else
			setContextMenuPolicy( Qt::NoContextMenu );
	}

	void SceneViewer::mouseReleaseEvent( QMouseEvent* event )
	{
		auto release_button = [&]( int i )
		{
			auto& b = drag[i];
			b.dragging = false;
		};

		if ( event->button() == Qt::LeftButton ) release_button( int( button::left ) );
		if ( event->button() == Qt::RightButton ) release_button( int( button::right ) );
		if ( event->button() == Qt::MiddleButton ) release_button( int( button::middle ) );

		bool selection_changed = false;
		if ( event->button() == Qt::MouseButton::LeftButton )
		{
			// Not holding shift: common case.
			if ( !( QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ShiftModifier ) )
			{
				const int select_threshold = 3;
				bool ctrl = ( QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ControlModifier );
				bool alt = ( QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::AltModifier );
				if ( drag_info( button::left ).drag_distance_x < select_threshold
					&& drag_info( button::left ).drag_distance_y < select_threshold )
				{
					if ( !ctrl && !alt )
						scene.clearSelection();

					glm::vec3 origin, dir;
					camera->viewport_pt_to_ray( event->x(), event->y(), origin, dir );
					const Glyph3DNode* g = scene.pick( origin, dir, scene.getFilterMode() == FilteredResultsDisplayMode::TranslucentUnfiltered );
					if ( g )
					{
						if ( alt || ( ctrl && scene.isSelected( g ) ) )
							scene.setUnSelected( g );
						else
							scene.setSelected( g );
					}
					selection_changed = true;
				}
			}
			else  // holding shift, so we're doing a drag-select
			{
				bool ctrl = ( QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ControlModifier );
				bool alt = ( QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::AltModifier );

				if ( !ctrl && !alt )
					scene.clearSelection();

				scene.enumGlyphs( [this, event, alt]( const Glyph3DNode& node ) {
					auto pos = node.getCachedPosition();
					auto pos2d = camera->world_pt_to_window_pt( pos );
					pos2d.y = height() - pos2d.y;
					if ( pos2d.x > std::min( drag_info( button::left ).drag_start_x, mouse_x )
						&& pos2d.x < std::max( drag_info( button::left ).drag_start_x, mouse_x )
						&& pos2d.y > std::min( drag_info( button::left ).drag_start_y, mouse_y )
						&& pos2d.y < std::max( drag_info( button::left ).drag_start_y, mouse_y ) )
					{
						if ( scene.getFilterMode() != FilteredResultsDisplayMode::HideUnfiltered || scene.passedFilter( &node ) )
						{
							if ( !alt )
								scene.setSelected( &node );
							else
								scene.setUnSelected( &node );
						}
					}
					return true;
				}, true );

				selection_changed = true;
			}

			if ( selection_changed )
			{
				if ( on_selection_changed ) on_selection_changed( !scene.empty() );
				if ( !scene.selectionEmpty() && enable_fly_to_object ) orbit_cam_control->flyToTarget();
			}
		}
	}

	void SceneViewer::resetCamera()
	{
		scene.clearSelection();
		if ( on_selection_changed ) on_selection_changed( !scene.empty() );
		camera->set_forward( glm::normalize( glm::vec3( 0.f, 1.f, -1.f ) ) );
		camera->set_position( glm::vec3( 0.f, -345.f, 345.f ) );
		set_cam_control( free_cam_control, true );	// force reactivate to let the controller know to update its internal state (since it has
													// no other way to know we changed the camera orientation directly)
	}

	template <typename T> int sgn( T val ) {
		return ( T( 0 ) < val ) - ( val < T( 0 ) );
	}

	void SceneViewer::updateFrame()
	{
		bool shift = QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ShiftModifier;
		bool ctrl = QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ControlModifier;

		// TODO: mouse cursor hiding / resetting for infinite drag capability. tricky to manage with Qt. 
		// see http://stackoverflow.com/questions/9774929/qt-keeping-mouse-centered
		/*
		if ( ( drag_info( button::left ).dragging && !shift ) || ( drag_info( button::middle ).dragging ) && !scene.selectionEmpty() )
		{
			auto cursor = QCursor( Qt::ArrowCursor );
			setCursor( cursor );
		}
		else
		{
			setCursor( QCursor( Qt::ArrowCursor ) );
		}
		*/

		if ( !scene.empty() )
		{
			// Camera update (if we're not holding down shift, in which case we're trying to drag-select).
			if ( !shift )
			{
				bool fast = ctrl;

				glm::vec3 cam_fwd = camera->get_forward();
				glm::vec3 cam_right = camera->get_right();

				float mouse_zoom_speed = 1.f;
				float wheel_zoom_speed = 0.1f;

				if ( scene.selectionEmpty() )
				{
					glm::vec3 motion;
					if ( key_states['w'] )
						motion += cam_fwd;
					else if ( key_states['s'] )
						motion -= cam_fwd;
					if ( key_states['d'] )
						motion += cam_right;
					else if ( key_states['a'] )
						motion -= cam_right;

					// Move the camera up and down along the world vertical axis. (To move along the camera's local
					// up/down axis instead, use get_up instead of get_world_up.)
					glm::vec3 cam_up = camera->get_world_up();
					if ( key_states['q'] )
						motion -= cam_up;
					else if ( key_states['e'] )
						motion += cam_up;

					free_cam_control->turn( glm::vec2( float( drag_info( button::left ).drag_delta_x ), float( drag_info( button::left ).drag_delta_y ) ) );
					free_cam_control->move( motion * ( fast ? 5.f : 1.f ) );
					set_cam_control( free_cam_control );
				}
				else
				{
					unsigned int count = 0u;
					selection_center = glm::vec3();
					float largest_bound = 0.f;
					bool glyphs_selected = false;
					scene.enumSelected( [this, &count, &largest_bound, &glyphs_selected]( const Glyph3DNode& glyph ) {
						selection_center += glyph.getCachedPosition();
						if ( glyph.getType() != Glyph3DNodeType::Link && glyph.getCachedBound().get_radius() > largest_bound ) largest_bound = glyph.getCachedBound().get_radius();
						if ( glyph.getType() == Glyph3DNodeType::GlyphElement ) glyphs_selected = true;
						++count;
					} );
					selection_center /= static_cast<float>( count );
					float selection_radius = 2.f * scene.getSingleSelection()->getCachedBound().get_radius();

					// if we only have links selected don't restrict the zoom distance (they tend to have huge bounds)
					float orbit_min_distance = glyphs_selected ? selection_radius + largest_bound : 0.f;
					orbit_cam_control->setOrbitTarget( selection_center );
					orbit_cam_control->setOrbitMinDistance( orbit_min_distance );

					// Handle zooming with middle button, L/R buttons, or wheel.
					float zoom = 0.f;
					if ( wheel_delta != 0.f )
						zoom = -wheel_delta * wheel_zoom_speed;
					else if ( drag_info( button::middle ).dragging )
						zoom = drag_info( button::middle ).drag_delta_y * mouse_zoom_speed;
					else if ( drag_info( button::left ).dragging && drag_info( button::right ).dragging )
						zoom = drag_info( button::left ).drag_delta_y * mouse_zoom_speed;

					// Keyboard camera rotation
					glm::vec2 orbit;
					if ( key_states['d'] )
						orbit.x -= 1.f;
					else if ( key_states['a'] )
						orbit.x += 1.f;
					if ( key_states['w'] )
						orbit.y += 1.f;
					else if ( key_states['s'] )
						orbit.y -= 1.f;

					if ( !( drag_info( button::left ).dragging && drag_info( button::right ).dragging ) )
						orbit += glm::vec2( float( drag_info( button::left ).drag_delta_x ), float( drag_info( button::left ).drag_delta_y ) );

					orbit *= ( fast ? 5.f : 1.f );

					orbit_cam_control->turn( orbit );
					orbit_cam_control->move( glm::vec3( 0.f, 0.f, zoom * ( fast ? 5.f : 1.f ) ) );

					set_cam_control( orbit_cam_control );
				}

			}

			cur_cam_control->update( 16.f );

			for ( int i = 0; i < int( button::count ); ++i )
				drag[i].drag_delta_x = drag[i].drag_delta_y = 0;
			wheel_delta = 0.f;
		}

		update();
	}

	void SceneViewer::setFilteredResults( const IndexSet& results )
	{
		scene.clearFilter();
		for ( auto index : results )
			scene.setPassedFilter( index );
	}

	void SceneViewer::setAxisNames( const char* X, const char* Y, const char* Z )
	{
		axis_names[0] = std::string( "X / " ) + X;
		axis_names[1] = std::string( "Y / " ) + Y;
		axis_names[2] = std::string( "Z / " ) + Z;
	}

	void SceneViewer::enableFlyToObject( bool enabled )
	{
		enable_fly_to_object = enabled;

		if ( enabled )
			orbit_cam_control->flyToTarget();
		else
			orbit_cam_control->cancelFlyToTarget();
	}

	void SceneViewer::setFilteredResultsDisplayMode( FilteredResultsDisplayMode mode )
	{
		scene.setFilterMode( mode );

		if ( mode == FilteredResultsDisplayMode::HideUnfiltered )
			scene.clearFilteredOutFromSelection();
	}

	void SceneViewer::set_cam_control( CameraController* cc, bool forceActivate )
	{
		if ( cur_cam_control != cc || forceActivate )
			cc->activate();

		if ( cur_cam_control != cc )
			cur_cam_control = cc;
	}
}
