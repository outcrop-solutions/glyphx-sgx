
#include "sceneviewer.h"
#include <cstdarg>
#include <ctime>
#include <boost/algorithm/string/replace.hpp>
#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QKeyEvent>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/compatibility.hpp>
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
#include "freecameracontroller.h"
#include "orbitcameracontroller.h"
#include "overheadcameracontroller.h"
#include "superimposedgroupmanager.h"

//temp
#include <QtCore/qitemselectionmodel.h>
#include "itemfocusselectionmodel.h"
#include "GlyphForestInfoModel.h"

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

	unsigned int SceneViewer::active_viewer_count = 0u;

	SceneViewer::SceneViewer(QWidget *parent, ViewerMode _mode, bool autoLoadDefaultScene)
		: QOpenGLWidget(parent), initialized(false), scene(nullptr), free_selection_camera(false), selection_effect_enabled(true), wheel_delta(0.f), scene_axes_enabled(true),
		hud_axes_enabled(true), hud_axes_location(HUDAxesLocation::TopLeft), enable_fly_to_object(false), animation_enabled(true), wireframe(false), glyph_renderer(nullptr),
		renderer(nullptr), background_color(render::color::black()), filtered_glyph_opacity(0.5f), item_focus_sm(nullptr), mode(_mode), auto_load_default_scene(autoLoadDefaultScene)
	{
		setLogoFile("textures/glyphx.png");

		memset(key_states, 0, sizeof(key_states));

		setMouseTracking(true);
		setFocusPolicy(Qt::FocusPolicy::StrongFocus);

		summation = true;
		explosion = false;

		mult = 1;
		QScreen* screen = QGuiApplication::primaryScreen();
		QRect screenGeometry = screen->geometry();
		if (screenGeometry.width() == 3840) {
			mult = 2;
		}

		legendsWidget = new LegendsWidget(this);
		legendsWidget->resize(400, 280);
		legendsWidget->hide();

		if (mode == ViewerMode::Full)
		{
			m_upRotateButton = CreateNavigationButton(tr("Rotate Up"), true);
			m_upRotateButton->setIcon(QIcon(":GlyphViewer/Resources/rotate_up.png"));
			QObject::connect(m_upRotateButton, &QToolButton::pressed, this, [this]() { cur_cam_control->turn(glm::vec2(0.f, -buttonRotateRate)); });

			m_leftRotateButton = CreateNavigationButton(tr("Rotate Left"), true);
			m_leftRotateButton->setIcon(QIcon(":GlyphViewer/Resources/rotate_left.png"));
			QObject::connect(m_leftRotateButton, &QToolButton::pressed, this, [this]() { cur_cam_control->turn(glm::vec2(-buttonRotateRate, 0.f)); });

			m_rightRotateButton = CreateNavigationButton(tr("Rotate Right"), true);
			m_rightRotateButton->setIcon(QIcon(":GlyphViewer/Resources/rotate_right.png"));
			QObject::connect(m_rightRotateButton, &QToolButton::pressed, this, [this]() { cur_cam_control->turn(glm::vec2(buttonRotateRate, 0.f)); });

			m_downRotateButton = CreateNavigationButton(tr("Rotate Down"), true);
			m_downRotateButton->setIcon(QIcon(":GlyphViewer/Resources/rotate_down.png"));
			QObject::connect(m_downRotateButton, &QToolButton::pressed, this, [this]() { cur_cam_control->turn(glm::vec2(0.f, buttonRotateRate)); });

			m_upRotateButton->hide();
			m_leftRotateButton->hide();
			m_rightRotateButton->hide();
			m_downRotateButton->hide();

			auto move = [&](const glm::vec3& dir, float amt)
			{
				if (cur_cam_control == free_cam_control)
					cur_cam_control->move(dir * amt);
				else if (cur_cam_control == orbit_cam_control)
					cur_cam_control->move(glm::vec3(0.f, 0.f, amt));
			};

			m_moveForwardButton = CreateNavigationButton(tr("Move Forward"), true);
			m_moveForwardButton->setIcon(QIcon(":SGXGUI/Resources/plus.png"));
			QObject::connect(m_moveForwardButton, &QToolButton::pressed, this, [this, move]() { move(camera->get_forward(), camera_mode() == camera_mode_t::free ? buttonMoveForwardBackRate : -buttonMoveForwardBackRate); });

			m_moveBackwardButton = CreateNavigationButton(tr("Move Backward"), true);
			m_moveBackwardButton->setIcon(QIcon(":SGXGUI/Resources/minus.png"));
			QObject::connect(m_moveBackwardButton, &QToolButton::pressed, this, [this, move]() { move(camera->get_forward(), camera_mode() == camera_mode_t::free ? -buttonMoveForwardBackRate : buttonMoveForwardBackRate); });

			m_moveUpButton = CreateNavigationButton(tr("Move Up"), true);
			m_moveUpButton->setIcon(QIcon(":SGXGUI/Resources/up_arrow.png"));
			QObject::connect(m_moveUpButton, &QToolButton::pressed, this, [this, move]() { move(camera->get_world_up(), buttonMoveUpDownRate); });

			m_moveDownButton = CreateNavigationButton(tr("Move Down"), true);
			m_moveDownButton->setIcon(QIcon(":SGXGUI/Resources/down_arrow.png"));
			QObject::connect(m_moveDownButton, &QToolButton::pressed, this, [this, move]() { move(camera->get_world_up(), -buttonMoveUpDownRate); });

			m_moveForwardButton->hide();
			m_moveBackwardButton->hide();
			m_moveUpButton->hide();
			m_moveDownButton->hide();

			QString style = QString("QToolButton {"
				"    border: " + QString::number(1 * mult) + "px;"
				"    width: " + QString::number(36 * mult) + "px; min-width: " + QString::number(36 * mult) + "px; max-width: " + QString::number(36 * mult) + "px;"
				"    height: " + QString::number(36 * mult) + "px; min-height: " + QString::number(36 * mult) + "px; max-height: " + QString::number(36 * mult) + "px;"
				"    border-radius: " + QString::number(18 * mult) + "px;"
				"    background-color: rgb(45, 48, 145);"
				"    color: white;"
				"    padding: 0;"
				"    margin: 0;"
				"    font-size: " + QString::number(12*mult) + "px;"
				"    font-family: Arial;"
				"}"
				"QToolButton:checked {"
				"    border: " + QString::number(2 * mult) + "px solid white; "
				"}"
				"QToolButton:disabled {"
				"    background-color: rgb(45, 48, 145); "
				"    color: grey; "
				"}");

			QString style_red = QString("QToolButton {"
				"    border: " + QString::number(1 * mult) + "px;"
				"    width: " + QString::number(36 * mult) + "px; min-width: " + QString::number(36 * mult) + "px; max-width: " + QString::number(36 * mult) + "px;"
				"    height: " + QString::number(36 * mult) + "px; min-height: " + QString::number(36 * mult) + "px; max-height: " + QString::number(36 * mult) + "px;"
				"    border-radius: " + QString::number(18 * mult) + "px;"
				"    background-color: red;"
				"    color: white;"
				"    padding: 0;"
				"    margin: 0;"
				"    font-size: " + QString::number(12 * mult) + "px;"
				"    font-family: Arial;"
				"}"
				"QToolButton:checked {"
				"    border: " + QString::number(2 * mult) + "px solid white; "
				"}"
				"QToolButton:disabled {"
				"    background-color: red; "
				"    color: grey; "
				"}");

			QToolButton* homeButton = new QToolButton(this);
			homeButton->setAttribute(Qt::WA_TranslucentBackground);
			homeButton->setStyleSheet(style);
			QIcon homeIcon;
			homeIcon.addFile(":SGXGUI/Resources/Icons/home_icon.png", QSize(60 * mult, 60 * mult), QIcon::Normal, QIcon::Off);
			//legendIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
			homeButton->setIcon(homeIcon);
			homeButton->setIconSize(QSize(24 * mult, 24 * mult));
			homeButton->setToolTip("Home");
			QObject::connect(homeButton, &QToolButton::pressed, this, [this]() { emit closeVisualization(); });
			homeButton->move(QPoint(5, 20 * mult));
			/*
			QToolButton* legendButton = new QToolButton(this);
			legendButton->setAttribute(Qt::WA_TranslucentBackground);
			legendButton->setStyleSheet(style);
			QIcon legendIcon;
			legendIcon.addFile(":SGXGUI/Resources/Icons/legend_icon.png", QSize(60 * mult, 60 * mult), QIcon::Normal, QIcon::Off);
			//legendIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
			legendButton->setIcon(legendIcon);
			legendButton->setIconSize(QSize(24 * mult, 24 * mult));
			legendButton->setToolTip("Show/Hide Legends");
			QObject::connect(legendButton, &QToolButton::pressed, this, [this]() { legendsWidget->setVisible(!legendsWidget->isVisible()); });
			legendButton->move(QPoint(0, 60 * mult));

			QToolButton* intLegendButton = new QToolButton(this);
			intLegendButton->setAttribute(Qt::WA_TranslucentBackground);
			intLegendButton->setStyleSheet(style);
			QIcon intLegendIcon;
			intLegendIcon.addFile(":SGXGUI/Resources/Icons/int_legend_icon.png", QSize(60 * mult, 60 * mult), QIcon::Normal, QIcon::Off);
			//legendIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
			intLegendButton->setIcon(intLegendIcon);
			intLegendButton->setIconSize(QSize(24 * mult, 24 * mult));
			intLegendButton->setToolTip("Show/Hide Interactive Legend");
			QObject::connect(intLegendButton, &QToolButton::pressed, this, [this]() { emit interactiveLegendToggled(); });
			intLegendButton->move(QPoint(0, 100 * mult));
			*/
			QToolButton* axesButton = new QToolButton(this);
			axesButton->setAttribute(Qt::WA_TranslucentBackground);
			axesButton->setStyleSheet(style);
			QIcon axesIcon;
			axesIcon.addFile(":SGXGUI/Resources/Icons/axes_icon.png", QSize(60 * mult, 60 * mult), QIcon::Normal, QIcon::Off);
			//axesIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
			axesButton->setIcon(axesIcon);
			axesButton->setIconSize(QSize(24 * mult, 24 * mult));
			axesButton->setToolTip("Show/Hide Axes");
			QObject::connect(axesButton, &QToolButton::pressed, this, [this]() { enableSceneAxes(!sceneAxesEnabled()); });
			axesButton->move(QPoint(5, 60 * mult));

			QToolButton* hudButton = new QToolButton(this);
			hudButton->setAttribute(Qt::WA_TranslucentBackground);
			hudButton->setStyleSheet(style);
			QIcon hudIcon;
			hudIcon.addFile(":SGXGUI/Resources/Icons/compass_icon.png", QSize(60 * mult, 60 * mult), QIcon::Normal, QIcon::Off);
			//camIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
			hudButton->setIcon(hudIcon);
			hudButton->setIconSize(QSize(24 * mult, 24 * mult));
			hudButton->setToolTip("Show/Hide Compass");
			QObject::connect(hudButton, &QToolButton::pressed, this, [this]() { enableHUDAxes(!hudAxesEnabled()); });
			hudButton->move(QPoint(5, 100 * mult));
			/*
			QToolButton* saveButton = new QToolButton(this);
			saveButton->setAttribute(Qt::WA_TranslucentBackground);
			saveButton->setStyleSheet(style);
			QIcon saveIcon;
			saveIcon.addFile(":SGXGUI/Resources/Icons/camera_icon.png", QSize(60 * mult, 60 * mult), QIcon::Normal, QIcon::Off);
			//camIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
			saveButton->setIcon(saveIcon);
			saveButton->setIconSize(QSize(24 * mult, 24 * mult));
			saveButton->setToolTip("Snapshot");
			QObject::connect(saveButton, &QToolButton::pressed, this, [this]() { emit saveSnapshot(); });
			saveButton->move(QPoint(0, 220 * mult));
			*/
			QToolButton* camButton = new QToolButton(this);
			camButton->setAttribute(Qt::WA_TranslucentBackground);
			camButton->setStyleSheet(style_red);
			QIcon camIcon;
			camIcon.addFile(":SGXGUI/Resources/Icons/camera-reset.png", QSize(60 * mult, 60 * mult), QIcon::Normal, QIcon::Off);
			//camIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
			camButton->setIcon(camIcon);
			camButton->setIconSize(QSize(24 * mult, 24 * mult));
			camButton->setToolTip("Reset Camera Position");
			QObject::connect(camButton, &QToolButton::pressed, this, [this]() { resetCamera(); });
			camButton->move(QPoint(5, 140 * mult));
/*
			QToolButton* acamButton = new QToolButton(this);
			acamButton->setAttribute(Qt::WA_TranslucentBackground);
			acamButton->setStyleSheet(style_red);
			QIcon acamIcon;
			acamIcon.addFile(":SGXGUI/Resources/Icons/camera_icon.png", QSize(60 * mult, 60 * mult), QIcon::Normal, QIcon::Off);
			//camIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
			acamButton->setIcon(camIcon);
			acamButton->setIconSize(QSize(24 * mult, 24 * mult));
			acamButton->setToolTip("Set Camera Position");
			QObject::connect(acamButton, &QToolButton::pressed, this, [this]() { std::vector<double> position{ -289.296, -263.176, 285.157, 0.623141, 0.566878, -0.538836 }; setCameraPosition(position); });
			acamButton->move(QPoint(0, 300 * mult));
*/
			//":GlyphViewer/Resources/rotate_up.png"
			orthoTopButton = CreateMenuBubble(style, "T", "Ortho Top");
			QObject::connect(orthoTopButton, &QToolButton::pressed, this, [this]() { orthogonal_view(0); });
			//orthoTopButton->move(QPoint(0, 300 * mult));

			orthoXButton = CreateMenuBubble(style, "X", "Ortho X");
			QObject::connect(orthoXButton, &QToolButton::pressed, this, [this]() { orthogonal_view(1); });
			//orthoXButton->move(QPoint(0, 340 * mult));

			orthoYButton = CreateMenuBubble(style, "Y", "Ortho Y");
			QObject::connect(orthoYButton, &QToolButton::pressed, this, [this]() { orthogonal_view(2); });
			//orthoYButton->move(QPoint(0, 380 * mult));

		}
		else
		{
			m_moveUpButton = m_moveDownButton = m_moveBackwardButton = m_moveForwardButton = nullptr;
			m_upRotateButton = m_downRotateButton = m_leftRotateButton = m_rightRotateButton = nullptr;
		}

		desired_selection_dist = flyToObjectDistance;

		scene = new GlyphScene(geomDB);
	}

	SceneViewer::~SceneViewer()
	{
		if (initialized)
		{
			makeCurrent();
			clearScene();
			delete scene;
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
			hal::device::release(tex_effect);
			hal::device::release(drag_effect);
			hal::device::release(default_base_texture);
			hal::device::release(sgx_logo);
			hal::device::release(hud_font);
			hal::device::release_context(context);

			--active_viewer_count;
			if (active_viewer_count == 0)
				hal::device::shutdown();
		}
	}

	void SceneViewer::loadScene(const char* sceneFile, const char* countFile, std::vector<std::string> baseImages, bool useSuperimposed)
	{

		hal::debug::profile_timer timer;

		makeCurrent();

		clearScene();

		// Load textures for new scene (should eventually move into scene loader).
		for (auto img : baseImages)
		{
			auto texture = hal::device::load_texture(img.c_str());

			if (!texture)
			{
				texture = default_base_texture;
				hal::device::addref(texture);
			}
			base_textures.push_back(texture);
		}

		r = new SceneReader();
		r->setSummation(summation);
		scene->setUseSuperimposed(explosion);
		r->read(sceneFile, countFile, *scene, *base_images, base_textures, default_base_texture, *grids);

		resetCamera();

		//auto scene_ptr = scene;
		if (explosion){
			scene->enumGroups([&](const std::vector<const Glyph3DNode*>& nodes, unsigned int group_idx) {
			auto pos = nodes[0]->getCachedPosition();
			float radius = nodes[0]->getCachedCombinedBound().get_radius();
			group_manager->create(group_idx, pos, radius);
			});
		}

		timer.print_ms_to_debug("full scene read");

	}

	void SceneViewer::clearScene()
	{
		if (initialized)
		{
			setFilteredResults(IndexSet(), true);
			scene->clearFilter();
			scene->clear();
			group_manager->clear();
			base_images->clear();
			grids->clear();
			for (int i = 0; i < 3; ++i)
				axis_names[i] = "";
			status_message = "";
			for (auto& tex : base_textures)
				hal::device::release(tex);
			base_textures.clear();
			geomDB.reset();
		}
	}

	QToolButton* SceneViewer::CreateMenuBubble(QString style, QString icon, QString tooltip) {

		QToolButton* button = new QToolButton(this);
		button->setAttribute(Qt::WA_TranslucentBackground);
		button->setStyleSheet(style);
		//QIcon ico;
		//ico.addFile(icon, QSize(60 * mult, 60 * mult), QIcon::Normal, QIcon::Off);
		//camIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
		//button->setIcon(ico);
		button->setText(icon);
		button->setIconSize(QSize(24 * mult, 24 * mult));
		button->setToolTip(tooltip);

		return button;
	}

	QToolButton* SceneViewer::CreateNavigationButton(const QString& toolTip, bool autoRepeat) {

		QToolButton* navButton = new QToolButton(this);
		QSize icon_size(navigationButtonSize*mult, navigationButtonSize*mult);
		navButton->setFixedSize(icon_size.width(), icon_size.height());
		navButton->setAutoRepeat(autoRepeat);
		navButton->setToolTip(toolTip);
		navButton->setAutoRepeatDelay(10);
		navButton->setAutoRepeatInterval(10);

		return navButton;
	}

	void SceneViewer::initializeGL()
	{
		if (active_viewer_count == 0u)
		{
			if (format().majorVersion() < 3 || (format().majorVersion() == 3 && format().minorVersion() < 3))
			{
				QString error = QString("Unable to create OpenGL 3.3 or higher core context (got %1.%2).").arg(
					QString::number(format().majorVersion()), QString::number(format().minorVersion()));
				QMessageBox::critical(this, "OpenGL Error", error);
			}

			// Global device initialization.
			hal::device::set_break_on_error(true);
			bool device_init_result = hal::device::init();
			hal::device::add_forced_effect_include("shaders/header.h");
			hal::device::add_forced_effect_include("shaders/util.h");
			if (!device_init_result) qDebug() << "SynGlyphX::hal::device::init() failed.";
			checkErrors();
		}
		viewer_id = active_viewer_count;
		++active_viewer_count;

		context = hal::device::get_new_context();

		geomDB.init();

		// Set up timer to attempt to trigger repaints at ~60fps.
		timer.setInterval(16);
		timer.setSingleShot(false);
		connect(&timer, SIGNAL(timeout()), SLOT(updateFrame()));
		timer.start();

		elapsed_timer.start();

		grids = new render::grid_renderer;
		axis_renderer = new AxisRenderer;
		base_images = new BaseImageRenderer;

		camera = new render::perspective_camera();
		camera->set_clip_planes(0.1f, 2048.f);

		free_cam_control = new FreeCameraController(camera);
		overhead_cam_control = new OverheadCameraController(camera);
		orbit_cam_control = new OrbitCameraController(camera);
		orbit_cam_control->setOrbitMaxDistance(camera->get_far_clip() * 0.75f);
		resetCamera();

		ui_camera = new render::ortho_camera(512u, 512u, -1024.f, 1024.f);

		tex_effect = hal::device::load_effect("shaders/texture.vert", nullptr, "shaders/texture.frag");
		drag_effect = hal::device::load_effect("shaders/drag_select.vert", nullptr, "shaders/drag_select.frag");

		default_base_texture = hal::device::load_texture("DefaultBaseImages/Clear.png");

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
		fmt.add_stream(hal::stream_info(hal::stream_type::float32, 3, hal::stream_semantic::position, 0));
		fmt.add_stream(hal::stream_info(hal::stream_type::float32, 2, hal::stream_semantic::texcoord, 0));

		if (logo_file.length() > 0)
			sgx_logo = hal::device::load_texture(logo_file.c_str());

		auto plane_mesh = hal::device::create_mesh(fmt, hal::primitive_type::triangle_list, 4, square, 2, square_indices);

		auto scale = glm::scale(glm::mat4(), glm::vec3(3000.f, 1.f, 1500.f));
		auto translate = glm::translate(glm::mat4(), glm::vec3(-1500.f, -50.f, 0.f));
		render::model_part* part = new render::model_part(plane_mesh, translate * scale, render::sphere_bound(glm::vec3(), 5000.f) /* not real bound */, "plane");
		plane = new render::model;
		plane->add_part(part);

		part = new render::model_part(plane_mesh, glm::mat4(), "sgx logo");
		logo = new render::model;
		logo->add_part(part);

		part = new render::model_part(plane_mesh, glm::mat4(), "drag select");
		drag_select = new render::model;
		drag_select->add_part(part);

		glyph_renderer = new GlyphRenderer(geomDB);
		glyph_renderer->setScene(scene);
		renderer = new render::renderer;

		initialized = true;

		hud_font = hal::device::load_font("fonts/OpenSans-Regular.ttf", 16*mult);

		group_manager = new SuperimposedGroupManager(*scene);

		if (auto_load_default_scene)
		{
			std::vector<std::string> images;
			QStringList filters;
			filters << "*.png";
			//images.push_back("C:/Users/bryan/OneDrive/Documents/GitHub/sgx/bin/Win64/Release/VizFiles/scene/base_img.png");
			QStringList infoList = QDir::current().entryList(filters, QDir::Files);
			for (QString image : infoList){
				images.push_back(image.toStdString());
			}

			loadScene("glyphs.sgc", "glyphs.sgn", images);
		}
	}

	void SceneViewer::resizeGL(int w, int h)
	{
		hal::device::set_external_default_render_target(defaultFramebufferObject());

		glm::mat4 view, proj;
		camera->update_viewport_size(w, h);
		ui_camera->update_viewport_size(w, h);

		if (mode == ViewerMode::Full)
		{
			QSize logo_size(hal::device::get_texture_width(sgx_logo), hal::device::get_texture_height(sgx_logo));
			auto logo_w = logo_size.width();//hal::device::get_texture_width( sgx_logo );
			auto logo_h = logo_size.height();//hal::device::get_texture_height( sgx_logo );

			auto logo_rotate = glm::rotate(glm::mat4(), glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f));
			auto logo_translate = glm::translate(glm::mat4(), glm::vec3(float(w - logo_w*mult)-10.f, float(logo_h*mult)+10.f, 0.f));
			auto logo_scale = glm::scale(glm::mat4(), glm::vec3(logo_w*mult, logo_h*mult, 1.f));
			auto logo_transform = logo_translate * logo_scale * logo_rotate;
			logo->set_transform(logo_transform);

			checkErrors();

			unsigned int nav_button_size = QSize(navigationButtonSize*mult, navigationButtonSize*mult).width();

			unsigned int leftPosOfButtonsInHCenter = w - 10 - (2 * nav_button_size);
			unsigned int topPositionOfButton = logo_h*mult + (2 * 10);

			m_upRotateButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

			topPositionOfButton += nav_button_size;
			m_leftRotateButton->move(QPoint(leftPosOfButtonsInHCenter - nav_button_size, topPositionOfButton));
			m_rightRotateButton->move(QPoint(leftPosOfButtonsInHCenter + nav_button_size, topPositionOfButton));

			topPositionOfButton += nav_button_size;
			m_downRotateButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

			topPositionOfButton += (2 * nav_button_size);
			m_moveForwardButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

			topPositionOfButton += nav_button_size;
			m_moveBackwardButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

			topPositionOfButton += (2 * nav_button_size);
			m_moveUpButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

			topPositionOfButton += nav_button_size;
			m_moveDownButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

			orthoTopButton->move(QPoint(w / 2, 20 * mult));
			orthoXButton->move(QPoint((w / 2) - 40, 20 * mult));
			orthoYButton->move(QPoint((w / 2) + 40, 20 * mult));

		}
	}

	glm::vec3 SceneViewer::compute_scene_axis_sizes()
	{
		const render::box_bound& bound = render::combine_bounds(scene->getBound(), base_images->get_base_image_bound(0u));
		return glm::vec3(bound.get_max().x - bound.get_min().x, bound.get_max().y - bound.get_min().y, bound.get_max().z - bound.get_min().z);
	}

	glm::vec3 SceneViewer::compute_scene_axis_origin()
	{
		const render::box_bound& bound = render::combine_bounds(scene->getBound(), base_images->get_base_image_bound(0u));
		return glm::vec3(bound.get_min());
	}

	glm::vec3 SceneViewer::compute_hud_axis_origin()
	{
		const glm::vec3 hud_axes_offset(100.f*mult, 100.f*mult, 0.f);
		glm::vec3 pos = hud_axes_offset;
		if (hud_axes_location == HUDAxesLocation::BottomLeft || hud_axes_location == HUDAxesLocation::BottomRight) pos.y = height() - hud_axes_offset.y;
		if (hud_axes_location == HUDAxesLocation::BottomRight) pos.x = width() - hud_axes_offset.x;
		return pos;
	}

	void SceneViewer::paintGL()
	{
		QPainter painter(this);	// for some reason rendering breaks if I remove this, even though we're not using it anywhere except for calling the constructor.
									// assuming there's some kind of weird opengl state setting happening in the constructor (or maybe the destructor). 
									// todo: investigate, fix, remove. might need to look at the Qt source if all else fails.

		hal::device::begin_frame();
		assert(elapsed_timer.isValid());

		//context->bind( ( hal::render_target_set* )nullptr );

		glyph_renderer->enableSelectionEffect(selection_effect_enabled);
		glyph_renderer->enableAnimation(animation_enabled);
		glyph_renderer->setFilterAlpha(glm::lerp(0.0f, 0.8f, filtered_glyph_opacity));

		hal::rasterizer_state rast{ true, true, false };
		context->set_rasterizer_state(rast);

		//background_color = glm::vec4(0.059, 0.09, 0.165, 1);
		background_color = glm::vec4(0.1215, 0.1529, 0.2274, 1);
		context->clear(hal::clear_type::color_depth, background_color);

		// Compute some axis information beforehand since we'll need it in a few places.
		glm::vec2 cam_xy(camera->get_forward().x, camera->get_forward().y);
		float angle = atan2f(cam_xy.x, cam_xy.y);

		glm::vec3 scene_axis_sizes = compute_scene_axis_sizes();
		glm::vec3 scene_axis_origin = compute_scene_axis_origin();
		glm::vec3 hud_axes_origin = compute_hud_axis_origin();
		glm::mat4 hud_axes_rotation = glm::rotate(glm::mat4(), glm::half_pi<float>() * -0.25f, glm::vec3(1.f, 0.f, 0.f)) * glm::rotate(glm::mat4(), angle, glm::vec3(0.f, 1.f, 0.f));
		const glm::vec3 hud_axes_offset(100.f, 100.f, 0.f);
		const float hud_axes_size = 48.f;

		bool exploded_group = (scene->getActiveGroup() != 0.f && scene->getGroupStatus() > 0.f);

		if (!scene->empty())
		{
			base_images->draw(context, camera);

			// Draw the actual glyphs.
			renderer->set_wireframe(wireframe);
			renderer->render(context, camera);
			glyph_renderer->setWireframeMode(wireframe);
			glyph_renderer->render_solid(context, camera, float(elapsed_timer.elapsed()) / 1000.f);

			// Draw the scene axes.
			if (scene_axes_enabled && mode == ViewerMode::Full)
			{
				axis_renderer->set_scales(2.0f, glm::vec3(6.f, 6.f, 8.f));
				axis_renderer->draw_axis(context, camera, render::color::red(), AxisDirection::X, scene_axis_origin, scene_axis_sizes.x);
				axis_renderer->draw_axis(context, camera, render::color::green(), AxisDirection::Y, scene_axis_origin, scene_axis_sizes.y);
				axis_renderer->draw_axis(context, camera, render::color::blue(), AxisDirection::Z, scene_axis_origin, scene_axis_sizes.z);
			}

			// Draw the grids.
			grids->draw(context, camera, render::color::white());

			// Clear depth so it doesn't interfere with screen-space elements.
			context->clear(hal::clear_type::depth);

			if (hud_axes_enabled && mode == ViewerMode::Full)
			{
				context->set_depth_state(hal::depth_state::read_write);
				axis_renderer->set_scales(2.f, glm::vec3(8.f, 8.f, 12.f));
				axis_renderer->draw_axis(context, ui_camera, render::color::red(), AxisDirection::X, hud_axes_origin, hud_axes_size*mult, hud_axes_rotation, true);
				axis_renderer->draw_axis(context, ui_camera, render::color::blue(), AxisDirection::Y, hud_axes_origin, hud_axes_size*mult, hud_axes_rotation, true);
				axis_renderer->draw_axis(context, ui_camera, render::color::green(), AxisDirection::Z, hud_axes_origin, hud_axes_size*mult, hud_axes_rotation, true);
			}

			glyph_renderer->render_blended(context, camera, float(elapsed_timer.elapsed()) / 1000.f);

			if (exploded_group)
			{
				context->set_depth_state(hal::depth_state::read_write);
				context->clear(hal::clear_type::depth);
				glyph_renderer->render_solid(context, camera, float(elapsed_timer.elapsed()) / 1000.f, true);
				group_manager->render(context, camera);
				glyph_renderer->render_blended(context, camera, float(elapsed_timer.elapsed()) / 1000.f, true);
			}

			if (!exploded_group)
				group_manager->render(context, camera);
		}

		// States for blended overlay elements.
		context->set_depth_state(hal::depth_state::disabled);
		context->set_blend_state(hal::blend_state::alpha);

		// draw drag-select
		if (mode != ViewerMode::SingleGlyph)
		{
			if (drag_info(button::left).dragging && (QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ShiftModifier))
			{
				bool alt = (QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::AltModifier);
				glm::vec3 origin(std::min(drag_info(button::left).drag_start_x, mouse_x), std::max(drag_info(button::left).drag_start_y, mouse_y), 0.f);
				glm::vec3 size(fabs(float(mouse_x - drag_info(button::left).drag_start_x)), fabs(float(mouse_y - drag_info(button::left).drag_start_y)), 0.f);
				auto drag_rotate = glm::rotate(glm::mat4(), glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f));
				auto drag_translate = glm::translate(glm::mat4(), origin);
				auto drag_scale = glm::scale(glm::mat4(), size);
				drag_select->set_transform(drag_translate * drag_scale * drag_rotate);
				context->bind(drag_effect);
				const glm::vec4 select_color = glm::vec4(1.f, 1.f, 1.f, 0.25f);
				const glm::vec4 unselect_color = glm::vec4(1.f, 0.f, 0.f, 0.25f);
				renderer->add_blended_batch(drag_select, drag_effect, glm::mat4(), alt ? unselect_color : select_color);
				renderer->render(context, ui_camera);
			}

			// draw logo
			if (mode == ViewerMode::Full)
			{
				context->bind(tex_effect);
				context->bind(0u, sgx_logo);
				context->set_constant(tex_effect, "material", "tint_color", glm::vec4(1.f, 1.f, 1.f, 1.f));
				renderer->add_blended_batch(logo, tex_effect);
				renderer->render(context, ui_camera);
			}

			checkErrors();
		}

		context->reset_defaults();

		if (mode == ViewerMode::Full)
		{
			if (!scene->empty())
			{
				if (scene->selectionEmpty())
				{
					auto campos = camera->get_position();
					renderTextCenteredF(hud_font, glm::vec2(width() / 2, height() - 32), CenterMode::X, render::color::white(), "Camera Position: X: %.2f, Y: %.2f, Z: %.2f", campos.x, campos.y, campos.z);
				}
				else
				{
					auto single_root = scene->getSingleRoot();
					if (single_root && single_root->getType() != Glyph3DNodeType::Link)
					{
						std::string positionHUD;
						for (int i = 0; i < 3; ++i) {

							//4 is the size of "X / ".  If an axis name's size is only 4 then nothing is mapped to that axis.
							if (axis_names[i].size() > 4) {

								if (!positionHUD.empty()) {
									positionHUD += ", ";
								}

								positionHUD += axis_names[i] + ": " + m_overridePosition[i];
							}
						}
						boost::replace_all(positionHUD, "%", "%%");
						renderTextCenteredF(hud_font, glm::vec2(width() / 2, height() - 32), CenterMode::X, render::color::white(), positionHUD.c_str());
					}
					else
					{
						renderTextCenteredF(hud_font, glm::vec2(width() / 2, height() - 32), CenterMode::X, render::color::white(), "Selection Centered At: X: %.2f, Y: %.2f, Z: %.2f", orbit_center.x, orbit_center.y, orbit_center.z);
					}
				}
			}

			// Draw axis names.
			if (hud_axes_enabled)
			{
				const float axis_name_offset = 16.f;
				if (axis_names[0] != "") renderText(hud_font, glm::vec2(hud_axes_origin + (hud_axes_size + axis_name_offset) * glm::vec3((hud_axes_rotation * glm::vec4(1.f, 0.f, 0.f, 0.f)))), render::color::white(), axis_names[0].c_str());
				if (axis_names[1] != "") renderText(hud_font, glm::vec2(hud_axes_origin + (hud_axes_size + axis_name_offset) * glm::vec3((hud_axes_rotation * glm::vec4(0.f, 0.f, 1.f, 0.f)))), render::color::white(), axis_names[1].c_str());
				if (axis_names[2] != "") renderText(hud_font, glm::vec2(hud_axes_origin - (hud_axes_size + axis_name_offset) * glm::vec3((hud_axes_rotation * glm::vec4(0.f, 1.f, 0.f, 0.f)))), render::color::white(), axis_names[2].c_str());
			}

			if (scene_axes_enabled)
			{
				const float axis_name_offset = 10.f;
				if (axis_names[0] != "") renderText(hud_font, camera, scene_axis_origin + (scene_axis_sizes.x + axis_name_offset) * glm::vec3(1.f, 0.f, 0.f), render::color::white(), axis_names[0].c_str());
				if (axis_names[1] != "") renderText(hud_font, camera, scene_axis_origin + (scene_axis_sizes.y + axis_name_offset) * glm::vec3(0.f, 1.f, 0.f), render::color::white(), axis_names[1].c_str());
				if (axis_names[2] != "") renderText(hud_font, camera, scene_axis_origin + (scene_axis_sizes.z + axis_name_offset) * glm::vec3(0.f, 0.f, 1.f), render::color::white(), axis_names[2].c_str());
			}
		}

		if (!status_message.empty())
			renderTextCenteredF(hud_font, glm::vec2(width() / 2, height() - 16), CenterMode::X, render::color::white(), status_message.c_str());

		// Draw tags.
		scene->enumTagEnabled([this](const Glyph3DNode& glyph) {
			if (scene->getFilterMode() != FilteredResultsDisplayMode::HideUnfiltered || scene->passedFilter(&glyph))
			{
				auto pos = glyph.getCachedPosition();
				if (scene->isExploded(&glyph)) pos += glyph.getExplodedPosition();
				if (glyph.getString(GlyphStringType::Tag)) renderText(hud_font, camera, pos, render::color::white(), glyph.getString(GlyphStringType::Tag));
			}
		});

		hal::device::end_frame();
	}

	void SceneViewer::checkErrors()
	{
		// HAL errors.
		if (hal::log::has_errors())
			qDebug() << "HAL Error(s):";
		while (hal::log::has_errors())
		{
			auto err = hal::log::next_error();
			qDebug() << err.first.c_str();
			if (err.second.length() > 0)
				qDebug() << "  " << err.second.c_str();
		}
	}

	void SceneViewer::showTagsOfSelectedObjects(bool show)
	{
		scene->enumSelected([this, show](const Glyph3DNode& glyph) {
			if (show)
				scene->enableTag(&glyph);
			else
				scene->disableTag(&glyph);
		});
	}

	void SceneViewer::hideAllTags()
	{
		scene->disableAllTags();
	}

	void SceneViewer::renderTextF(hal::font* font, const glm::vec2& pos, const glm::vec4& color, const char* string, ...)
	{
		const unsigned int buf_size = 8192u;
		static char buf[buf_size];
		va_list args;
		va_start(args, string);
#ifdef WIN32
		vsprintf_s(buf, string, args);
#else
		vsnprintf(buf, buf_size, string, args);
#endif
		va_end(args);

		auto transform = ui_camera->get_proj() * ui_camera->get_view() * glm::translate(glm::mat4(), glm::vec3(glm::round(pos), 0.f));
		context->draw(font, transform, color, buf);
	}

	void SceneViewer::renderTextCenteredF(hal::font* font, const glm::vec2& pos, CenterMode mode, const glm::vec4& color, const char* string, ...)
	{
		const unsigned int buf_size = 8192u;
		static char buf[buf_size];
		va_list args;
		va_start(args, string);
#ifdef WIN32
		vsprintf_s(buf, string, args);
#else
		vsnprintf(buf, buf_size, string, args);
#endif
		va_end(args);

		glm::vec2 text_size = context->measure_text(font, buf);
		int imode = int(mode);
		glm::vec2 adjustment((imode & int(CenterMode::X)) ? text_size.x * 0.5f : 0.f, (imode & int(CenterMode::Y)) ? text_size.y * 0.5f : 0.f);

		auto transform = ui_camera->get_proj() * ui_camera->get_view() * glm::translate(glm::mat4(), glm::vec3(glm::round(pos - adjustment), 0.f));
		context->draw(font, transform, color, buf);
	}

	void SceneViewer::renderText(hal::font* font, const glm::vec2& pos, const glm::vec4& color, const char* string)
	{
		auto transform = ui_camera->get_proj() * ui_camera->get_view() * glm::translate(glm::mat4(), glm::vec3(glm::round(pos), 0.f));
		context->draw(font, transform, color, string);
	}

	void SceneViewer::renderTextCentered(hal::font* font, const glm::vec2& pos, CenterMode mode, const glm::vec4& color, const char* string)
	{
		glm::vec2 text_size = context->measure_text(font, string);
		int imode = int(mode);
		glm::vec2 adjustment((imode & int(CenterMode::X)) ? text_size.x * 0.5f : 0.f, (imode & int(CenterMode::Y)) ? text_size.y * 0.5f : 0.f);

		auto transform = ui_camera->get_proj() * ui_camera->get_view() * glm::translate(glm::mat4(), glm::vec3(glm::round(pos - adjustment), 0.f));
		context->draw(font, transform, color, string);
	}

	void SceneViewer::renderText(hal::font* font, const render::camera* camera, const glm::vec3& pos, const glm::vec4& color, const char* string)
	{
		auto window_pt = camera->world_pt_to_window_pt(pos);
		if (!camera->pt_behind_camera(pos) && window_pt.x >= 0.f && window_pt.y > 0.f && window_pt.x < float(width()) && window_pt.y < float(height()))
			renderText(font, window_pt, color, string);
	}

	void SceneViewer::keyPressEvent(QKeyEvent* event)
	{
		if (event->key() < key_states_size) key_states[tolower(event->key())] = true;
		QWidget::keyPressEvent(event);

		auto key = event->key();
		switch (key)
		{
		case Qt::Key_Escape:
		{
			scene->collapse(scene->getActiveGroup());
			break;
		}
		case 'X':
		{
			if (scene->getActiveGroup() == 0u)
			{
				auto root = scene->getSingleRoot();
				if (root)
				{
					auto group = root->getExplodedPositionGroup();
					if (group > 0) scene->explode(group);
				}
				break;
			}
		}
#ifdef _DEBUG
		case 'R': hal::device::rebuild_effects(); break;
		case 'B': glyph_renderer->enableBoundVis(!glyph_renderer->boundVisEnabled()); break;
		case 'M': if (glyph_renderer->boundVisEnabled())
		{
			if (glyph_renderer->getBoundVisMode() == GlyphRenderer::BoundVisMode::Combined)
				glyph_renderer->setBoundVisMode(GlyphRenderer::BoundVisMode::Individual);
			else
				glyph_renderer->setBoundVisMode(GlyphRenderer::BoundVisMode::Combined);
			break;
		}
		case 'P':
		{
			auto sel = scene->getSingleSelection();
			if (scene->selectionSize() > 1)
				hal::debug::print("Multiple objects selected; printing hierarchy from first.");
			if (sel) scene->debugPrint(sel);
			break;
		}
#endif
		}
	}

	void SceneViewer::keyReleaseEvent(QKeyEvent* event)
	{
		if (event->key() < key_states_size) key_states[tolower(event->key())] = false;
		QWidget::keyReleaseEvent(event);
	}

	void SceneViewer::wheelEvent(QWheelEvent* event)
	{
		wheel_delta = event->angleDelta().y();
	}

	void SceneViewer::mouseMoveEvent(QMouseEvent* event)
	{
		mouse_x = event->x();
		mouse_y = event->y();

		for (int i = 0; i < int(button::count); ++i)
		{
			if (drag[i].dragging)
			{
				drag[i].drag_delta_x = mouse_x - mouse_prev_x;
				drag[i].drag_delta_y = mouse_y - mouse_prev_y;
				drag[i].drag_distance_x += abs(drag[i].drag_delta_x);
				drag[i].drag_distance_y += abs(drag[i].drag_delta_y);

				mouse_prev_x = event->x();
				mouse_prev_y = event->y();
			}
		}

		// disable context menu if we're right-dragging or L/R-dragging
		if (mode != ViewerMode::Full || (drag_info(button::left).dragging || drag_info(button::right).dragging))
			setContextMenuPolicy(Qt::NoContextMenu);
		else
			setContextMenuPolicy(Qt::ActionsContextMenu);
	}

	void SceneViewer::mousePressEvent(QMouseEvent* event)
	{
		auto set_button = [&](int i)
		{
			auto& b = drag[i];
			b.drag_start_x = mouse_prev_x = event->x();
			b.drag_start_y = mouse_prev_y = event->y();
			b.dragging = true;
			b.drag_distance_x = 0;
			b.drag_distance_y = 0;
		};

		if (event->buttons() & Qt::LeftButton) set_button(int(button::left));
		if (event->buttons() & Qt::RightButton) set_button(int(button::right));
		if (event->buttons() & Qt::MiddleButton) set_button(int(button::middle));
	}

	void SceneViewer::mouseReleaseEvent(QMouseEvent* event)
	{
		auto release_button = [&](int i)
		{
			auto& b = drag[i];
			b.dragging = false;
		};

		if (event->button() == Qt::LeftButton) release_button(int(button::left));
		if (event->button() == Qt::RightButton) release_button(int(button::right));
		if (event->button() == Qt::MiddleButton) release_button(int(button::middle));

		if (mode != ViewerMode::SingleGlyph)
		{
			bool changed_selection = false;
			if (event->button() == Qt::MouseButton::LeftButton)
			{
				if (!(QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ShiftModifier))
				{
					glm::vec3 origin, dir;
					camera->viewport_pt_to_ray(event->x(), event->y(), origin, dir);
					const int select_threshold = 3;
					bool ctrl = (QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ControlModifier);
					bool alt = (QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::AltModifier);
					if (drag_info(button::left).drag_distance_x < select_threshold
						&& drag_info(button::left).drag_distance_y < select_threshold)
					{
						if (!ctrl && !alt)
							scene->clearSelection();

						auto scene_pick_result = scene->pick_with_distance(origin, dir, scene->getFilterMode() == FilteredResultsDisplayMode::TranslucentUnfiltered, scene->getActiveGroup() != GlyphScene::NO_GROUP && scene->getGroupStatus() > 0.f);
						float scene_pick_dist = scene_pick_result.second;

						auto gadget_pick_result = group_manager->pick(camera, origin, dir, scene_pick_dist);
						float gadget_pick_dist = gadget_pick_result.second;
						auto gadget_group_idx = gadget_pick_result.first;
						auto active_group = scene->getActiveGroup();

						// if we hit a gadget, and it's closer than any glyphs we hit, toggle it-- but only if
						// either there's no current exploded group or we clicked the current one (don't want to explode
						// a group if there's already an exploded one)
						if (gadget_pick_dist < scene_pick_dist && (active_group == 0 || gadget_group_idx == active_group))
						{
							if (gadget_group_idx > 0)
								scene->toggleExplode(gadget_group_idx);
						}
						else
						{
							auto g = scene_pick_result.first;
							if (g)
							{
								if (alt || (ctrl && scene->isSelected(g)))
									scene->setUnSelected(g);
								else
									scene->setSelected(g);
							}
						}
						changed_selection = true;

					}
				}
				else  // holding shift, so we're doing a drag-select
				{
					bool ctrl = (QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ControlModifier);
					bool alt = (QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::AltModifier);

					if (!ctrl && !alt)
						scene->clearSelection();

					bool exploded = scene->getActiveGroup() > 0.f && scene->getGroupStatus() > 0.f;

					scene->enumGlyphs([this, alt, exploded](const Glyph3DNode& node) {
						auto pos = scene->getExplodedPosition(&node);
						auto pos2d = camera->world_pt_to_window_pt(pos);
						if (pos2d.x > std::min(drag_info(button::left).drag_start_x, mouse_x)
							&& pos2d.x < std::max(drag_info(button::left).drag_start_x, mouse_x)
							&& pos2d.y > std::min(drag_info(button::left).drag_start_y, mouse_y)
							&& pos2d.y < std::max(drag_info(button::left).drag_start_y, mouse_y))
						{
							if (!exploded || (node.getExplodedPositionGroup() == scene->getActiveGroup()))
							{
								if (glm::dot(pos - camera->get_position(), camera->get_forward()) > 0.f)	// make sure it's not behind the camera
								{
									if (node.getColor().a > 0.f)
									{
										if (scene->getFilterMode() != FilteredResultsDisplayMode::HideUnfiltered || scene->passedFilter(&node))
										{
											if (!alt)
												scene->setSelected(&node);
											else
												scene->setUnSelected(&node);
										}
									}
								}
							}
						}
						return true;
					});

					changed_selection = true;
				}

				if (changed_selection)
				{
					selection_changed();
				}
			}
		}
	}

	void SceneViewer::resetCamera()
	{
		camera->set_position(glm::vec3(0.f, -345.f, 345.f));
		glm::vec3 cam_dir = glm::normalize(glm::vec3(0.f, 1.f, -1.f));
		// Point the camera at the center of the scene's bound, so we can be reasonably sure we see something when we load up.
		if (!scene->empty())
			cam_dir = glm::normalize(scene->getBound().get_center() - camera->get_position());
		camera->set_forward(cam_dir);
		set_cam_control(free_cam_control, true);	// force reactivate to let the controller know to update its internal state (since it has
													// no other way to know we changed the camera orientation directly)

		if (mode != ViewerMode::Full)
		{
			// Single glyph mode is locked to orbit cam, so find a better starting position for the camera.
			glm::vec3 pos;
			int count = 0;
			float radius = 0.f;
			scene->enumGlyphs([&radius, &pos, &count](const Glyph3DNode& glyph) {
				float r = glyph.getCachedCombinedBound().get_radius();
				if (r > radius) radius = r;
				pos += glyph.getCachedPosition();
				++count;
				return true;
			});
			pos /= float(count);
			camera->set_position(pos + glm::vec3(0.f, 0.f, -radius * 5.f));
			orbit_cam_control->setOrbitTarget(pos, radius * 3.f, true);
			set_cam_control(orbit_cam_control, true);
		}

		scene->clearSelection();
		selection_changed();
	}

	std::vector<float> SceneViewer::getCameraPosition() {

		glm::vec3 pos = camera->get_position();
		glm::vec3 center = scene->getBound().get_center();
		glm::vec3 cam_dir = glm::normalize(center - pos);

		std::vector<float> position{pos[0], pos[1], pos[2], cam_dir[0], cam_dir[1], cam_dir[2]};
		return position;
	}

	void SceneViewer::setCameraPosition(std::vector<double> pos) {

		orbit_cam_control->cancelFlyToTarget();
		camera->set_position(glm::vec3(pos[0], pos[1], pos[2]));
		glm::vec3 cam_dir = glm::normalize(glm::vec3(pos[3], pos[4], pos[5]));
		camera->set_forward(cam_dir);
		orbit_cam_control->setOrbitTarget(orbit_center, 3.f, true);
		set_cam_control(orbit_cam_control, true);
		//set_cam_control(free_cam_control, true);
/*
		QString out = QString::number(pos[0]) + ", " + QString::number(pos[1]) + ", " + QString::number(pos[2]) + "\n" +
			QString::number(pos[3]) + ", " + QString::number(pos[4]) + ", " + QString::number(pos[5]);
		QMessageBox::information(this, tr("Server message"), out);
*/
	}

	void SceneViewer::orthogonal_view(int pos) {

		glm::vec3 position = camera->get_position();
		std::vector<double> new_pos;
		auto dist = glm::distance(position, orbit_center);

		if (pos == 0) { //TOP
			new_pos.push_back(orbit_center[0]);
			new_pos.push_back(orbit_center[1]);
			new_pos.push_back(dist);

			new_pos.push_back(-0.01);
			new_pos.push_back(0.01);
			new_pos.push_back(-0.99);
		}
		else if (pos == 1) { //X

			new_pos.push_back(orbit_center[0]);
			new_pos.push_back(dist*-1);
			new_pos.push_back(orbit_center[2]);

			new_pos.push_back(0);
			new_pos.push_back(1);
			new_pos.push_back(0);

		}
		else if (pos == 2) { //Y
			new_pos.push_back(dist);
			new_pos.push_back(orbit_center[1]);
			new_pos.push_back(orbit_center[2]);

			new_pos.push_back(-1);
			new_pos.push_back(0);
			new_pos.push_back(0);
		}
		setCameraPosition(new_pos);
	}

	void SceneViewer::updateFrame()
	{
		bool shift = QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ShiftModifier;
		bool ctrl = QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ControlModifier;

		// TODO: mouse cursor hiding / resetting for infinite drag capability. tricky to manage with Qt. 
		// see http://stackoverflow.com/questions/9774929/qt-keeping-mouse-centered
		/*
		if ( ( drag_info( button::left ).dragging && !shift ) || ( drag_info( button::middle ).dragging ) && !scene->selectionEmpty() )
		{
			auto cursor = QCursor( Qt::ArrowCursor );
			setCursor( cursor );
		}
		else
		{
			setCursor( QCursor( Qt::ArrowCursor ) );
		}
		*/

		if (!scene->empty())
		{
			// Camera update (if we're not holding down shift, in which case we're trying to drag-select).
			if (!shift)
			{
				bool fast = ctrl;

				glm::vec3 cam_fwd = camera->get_forward();
				glm::vec3 cam_right = camera->get_right();

				const float mouse_zoom_speed = 1.f;
				const float wheel_zoom_speed = 0.1f;

				glm::vec3 motion;

				if (camera_mode() == camera_mode_t::free)
				{
					// right-dragging has specialized behavior
					if (!drag_info(button::left).dragging && drag_info(button::right).dragging && drag_distance(button::right) > 0.f)
					{
						auto proj_fwd = glm::vec3(cam_fwd.x, cam_fwd.y, 0.f);
						if (glm::length(proj_fwd) > 0.f)
						{
							const float drag_motion_speed = 0.005f;
							proj_fwd = glm::normalize(proj_fwd);
							auto& drag = drag_info(button::right);
							auto drag_motion = drag_motion_speed * glm::vec2(mouse_x - drag.drag_start_x, mouse_y - drag.drag_start_y);
							motion = proj_fwd * -drag_motion.y * mouse_zoom_speed;
							free_cam_control->turn(glm::vec2(drag_motion.x, 0.f));
						}
					}
					else  // otherwise, the standard free-camera movement
					{
						// Forward/back along camera axis.
						if (key_states[uint8_t('w')])
							motion += cam_fwd;
						else if (key_states[uint8_t('s')])
							motion -= cam_fwd;

						motion += cam_fwd * wheel_delta * wheel_zoom_speed;
						if (drag_info(button::middle).dragging)
						{
							motion -= cam_fwd * float(drag_info(button::middle).drag_delta_y) * mouse_zoom_speed;
						}
						else if (drag_info(button::left).dragging && drag_info(button::right).dragging)
						{
							motion -= cam_fwd * float(drag_info(button::left).drag_delta_y) * mouse_zoom_speed;
						}

						if (!(drag_info(button::left).dragging && drag_info(button::right).dragging))	// if two-button dragging, don't turn
							free_cam_control->turn(glm::vec2(float(drag_info(button::left).drag_delta_x), float(drag_info(button::left).drag_delta_y)));
					}

					// Left-right along camera axis.
					if (key_states[uint8_t('d')])
						motion += cam_right;
					else if (key_states[uint8_t('a')])
						motion -= cam_right;

					// Move the camera up and down along the world vertical axis. (To move along the camera's local
					// up/down axis instead, use get_up instead of get_world_up.)
					glm::vec3 cam_up = camera->get_world_up();
					if (key_states[uint8_t('q')])
						motion -= cam_up;
					else if (key_states[uint8_t('e')])
						motion += cam_up;

					free_cam_control->move(motion * (fast ? 5.f : 1.f));
					set_cam_control(free_cam_control);
					if (mode == ViewerMode::Full)
					{
						m_moveUpButton->setEnabled(true);
						m_moveDownButton->setEnabled(true);
					}
				}
				else
				{
					orbit_center = glm::vec3();
					bool glyphs_selected = false;
					unsigned int count = 0u;
					float largest_bound = 0.f;
					float selection_radius = 0.f;
					if (mode == ViewerMode::Full || mode == ViewerMode::SingleGlyph_AllowSelection)
					{
						scene->enumSelected([this, &count, &largest_bound, &glyphs_selected](const Glyph3DNode& glyph) {
							orbit_center += glyph.getCachedPosition();
							if (glyph.getType() != Glyph3DNodeType::Link && glyph.getRootParent()->getCachedCombinedBound().get_radius() > largest_bound) largest_bound = glyph.getRootParent()->getCachedCombinedBound().get_radius();
							if (glyph.getType() == Glyph3DNodeType::GlyphElement) glyphs_selected = true;
							++count;
						});
						if (count > 0u) orbit_center /= static_cast<float>(count);
						selection_radius = 2.f * (scene->getSingleSelection() ? scene->getSingleSelection()->getRootParent()->getCachedCombinedBound().get_radius() : 0.f);
						// account for the glyph possibly being in an exploded group
						glm::vec3 explosion_offset;
						auto single_selection = scene->getSingleSelection();
						if (single_selection && scene->isExploded(single_selection))
							explosion_offset = single_selection->getExplodedPosition();

						// if we only have links selected don't restrict the zoom distance (they tend to have huge bounds)
						float orbit_min_distance = glyphs_selected ? selection_radius + largest_bound : 0.f;
						orbit_cam_control->setOrbitTarget(orbit_center + explosion_offset, orbit_min_distance, cur_cam_control != orbit_cam_control);
					}

					// Handle zooming with middle button, L/R buttons, or wheel.
					float zoom = 0.f;
					if (wheel_delta != 0.f)
						zoom = -wheel_delta * wheel_zoom_speed;
					else if (drag_info(button::middle).dragging)
						zoom = drag_info(button::middle).drag_delta_y * mouse_zoom_speed;
					else if (drag_info(button::left).dragging && drag_info(button::right).dragging)
						zoom = drag_info(button::left).drag_delta_y * mouse_zoom_speed;

					// Keyboard camera rotation
					glm::vec2 orbit;
					if (key_states[uint8_t('d')])
						orbit.x -= 1.f;
					else if (key_states[uint8_t('a')])
						orbit.x += 1.f;
					if (key_states[uint8_t('w')])
						orbit.y += 1.f;
					else if (key_states[uint8_t('s')])
						orbit.y -= 1.f;

					if (!(drag_info(button::left).dragging && drag_info(button::right).dragging))
						orbit += glm::vec2(float(drag_info(button::left).drag_delta_x), float(drag_info(button::left).drag_delta_y));

					orbit *= (fast ? 5.f : 1.f);

					orbit_cam_control->turn(orbit);
					orbit_cam_control->move(glm::vec3(0.f, 0.f, zoom * (fast ? 5.f : 1.f)));

					set_cam_control(orbit_cam_control);

					if (mode == ViewerMode::Full)
					{
						m_moveUpButton->setEnabled(false);
						m_moveDownButton->setEnabled(false);
					}
				}

			}

			cur_cam_control->update(16.f);

			for (int i = 0; i < int(button::count); ++i)
				drag[i].drag_delta_x = drag[i].drag_delta_y = 0;
			wheel_delta = 0.f;
		}

		scene->update(16.f);

		update();
	}

	void SceneViewer::setFilteredResults(const IndexSet& results, bool disableFiltering)
	{
		QFile srfile("filterLog.txt");
		srfile.open(QIODevice::WriteOnly);
		QTextStream out(&srfile);
		out << "{{Filtering Log}}" << endl;

		out << "Result size: " << results.size() << endl;

		QMap<int, float> stacked_heights;
		QMap<int, double> stacked_tag;
		QMap<int, QList<int>> currentIds;
		QSet<int> stacked_filter;
		scene->clearFilter();
		if (!disableFiltering) scene->setFilterApplied();
		for (auto index : results){
			scene->setPassedFilter(index);
			//out << index << endl;
			//Handle filtering of stacked glyphs if they exist
			int idx = index+6;
			int stacked = r->getFilteringIndexMap()[idx];
			if (idx != stacked) {
				stacked_filter.insert(stacked-6);

				float height = scene->getGlyph3D(idx)->getLocalScale().z;
				if (stacked_heights.contains(stacked)) {
					stacked_heights[stacked] += height;
					stacked_tag[stacked] += r->getTagValueMap()[idx];
					currentIds[stacked].append(idx);
				}
				else {
					stacked_heights[stacked] = height;
					stacked_tag[stacked] = r->getTagValueMap()[idx];
					currentIds.insert(stacked, QList<int>{idx});
				}
			}
		}
		foreach(const int &value, stacked_filter) {
			//out << value << endl;
			glm::vec3 scale = scene->getGlyph3D(value + 6)->getLocalScale();
			glm::vec3 position = scene->getGlyph3D(value + 6)->getLocalPosition();

			if (scale.z != stacked_heights[value + 6]) {

				float ratio = stacked_heights[value + 6] / scale.z;
				scale.z = stacked_heights[value + 6];
				scene->getGlyph3D(value + 6)->setLocalScale(scale);
				position.z = position.z * ratio;
				scene->getGlyph3D(value + 6)->setLocalPosition(position);
				Glyph3DNode* glyphnode = scene->getGlyph3D(value + 6);
				r->setCurrentGlyphIds(value + 6, currentIds[value + 6]);

				//update color
				glyphnode->setColor(r->getColorMapping()[r->findClosest(r->getAllScaleZ().size(), stacked_heights[value + 6])]);

				//update tag
				std::string tag = "Z: " + QString::number(stacked_tag[value + 6]).toStdString();
				glyphnode->setString(GlyphStringType::Tag, scene->createString(tag.c_str()));
				r->setCurrentTag(value + 6, stacked_tag[value + 6]);

				glm::mat4 transform = glm::translate(glm::mat4(), glyphnode->getLocalPosition());
				glm::vec3 visual_scale(1.f, 1.f, 1.f);
				transform = glm::rotate(transform, glm::radians(glyphnode->getLocalRotation().y), glm::vec3(0.0f, 0.0f, -1.0f));
				transform = glm::rotate(transform, glm::radians(glyphnode->getLocalRotation().x), glm::vec3(-1.0f, 0.0f, 0.0f));
				transform = glm::rotate(transform, glm::radians(glyphnode->getLocalRotation().z), glm::vec3(0.0f, 0.0f, -1.0f));

				transform = glm::scale(transform, glyphnode->getLocalScale());

				glyphnode->setCachedTransform(transform);
				glyphnode->setVisualScale(visual_scale);

				//r->getStackedGlyphMap()[r->getIndexToUID()[value+6]].scaleZ)

			}

			//out << "glyph: " << value + 6 << " z: " << scene->getGlyph3D(value + 6)->getLocalScale().z << endl;
			scene->setPassedFilter(value);
		}
		//}
		if (scene->getFilterMode() == FilteredResultsDisplayMode::HideUnfiltered)
			scene->clearFilteredOutFromSelection();
	}

	void SceneViewer::setAxisNames(const char* X, const char* Y, const char* Z)
	{
		axis_names[0] = std::string("X / ") + X;
		axis_names[1] = std::string("Y / ") + Y;
		axis_names[2] = std::string("Z / ") + Z;
	}

	void SceneViewer::setOverridePositionXYZ(const std::array<std::string, 3>& positionOverride)
	{
		m_overridePosition = positionOverride;
	}

	void SceneViewer::enableFlyToObject(bool enabled)
	{
		enable_fly_to_object = enabled;

		if (enabled)
			orbit_cam_control->flyToTarget();
		else
			orbit_cam_control->cancelFlyToTarget();
	}

	void SceneViewer::setFilteredResultsDisplayMode(FilteredResultsDisplayMode mode)
	{
		scene->setFilterMode(mode);

		if (mode == FilteredResultsDisplayMode::HideUnfiltered)
			scene->clearFilteredOutFromSelection();
	}

	void SceneViewer::set_cam_control(CameraController* cc, bool forceActivate)
	{
		if (cur_cam_control != cc || forceActivate)
			cc->activate();

		if (cur_cam_control != cc)
			cur_cam_control = cc;
	}

	void SceneViewer::selection_changed()
	{

		if (on_selection_changed) on_selection_changed(!scene->empty());
		if (!scene->selectionEmpty() && enable_fly_to_object) orbit_cam_control->flyToTarget();

		if (item_focus_sm)
		{
			item_focus_sm->clear();
			QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect;
			SynGlyphX::ItemFocusSelectionModel::FocusFlags focusFlags = SynGlyphX::ItemFocusSelectionModel::FocusFlag::ClearAndFocus;
			flags = QItemSelectionModel::Toggle;
			focusFlags = SynGlyphX::ItemFocusSelectionModel::FocusFlag::Toggle;

			std::unordered_set<int> selected_ids;

			QItemSelection selected;
			scene->enumSelected([&](const Glyph3DNode& node)
			{
				int id = node.getRootParent()->getID();
				if (selected_ids.find(id) == selected_ids.end())
				{
					QModelIndex modelIndex = glyph_forest_model->IndexFromCSVID(id);
					selected.select(modelIndex, modelIndex);
					selected_ids.insert(id);
				}
			});

			if (selected.size() > 0)
			{
				item_focus_sm->select(selected, flags);
			}
		};
	}

	SceneViewer::camera_mode_t SceneViewer::camera_mode()
	{
		// If fly-to-object is enabled, enable the orbit camera until we're done flying (or it's canceled).
		// (The orbit camera is the one that implements the fly-to-object animation.)
		if (!scene->selectionEmpty() && enable_fly_to_object && orbit_cam_control->flyingToTarget())
			return camera_mode_t::orbit;

		// Otherwise if we're in full viewer mode and either the user has chosen the free camera or there's no
		// selection, choose the free camera.
		else if ((scene->selectionEmpty() || free_selection_camera) && mode == ViewerMode::Full)
			return camera_mode_t::free;

		// Otherwise, orbit.
		else
			return camera_mode_t::orbit;
	}

	float SceneViewer::drag_distance(button b)
	{
		mouse_drag_info& d = drag[int(b)];
		if (d.dragging)
			return glm::length(glm::vec2(d.drag_distance_x, d.drag_distance_y));
		else
			return 0.f;
	}

	void SceneViewer::enableSuperimposedGlyphGadgets(bool val)
	{
		if (group_manager) group_manager->setMode(val ? SuperimposedGadgetMode::Always : SuperimposedGadgetMode::OnSelection);
	}

	void SceneViewer::setupLegendWindow(QString cache_location, QStringList legends)
	{
		legendsWidget->SetLegends(cache_location, legends);
	}

}
