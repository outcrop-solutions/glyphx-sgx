
///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

#pragma once

#include "../platform.h"
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QToolButton>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLFunctions>
#include <QtCore/QTimer>
#include <QtCore/QElapsedTimer>
#include <QtGui/QFont>
#include <render/types.h>
#include <hal/hal.h>
#include <set>
#include "glyphrenderer.h"

namespace SynGlyphX
{
	namespace render { class grid_renderer; }
	typedef std::set<unsigned long> IndexSet;

	class AxisRenderer; class BaseImageRenderer; class LinksRenderer;
	class FreeCameraController; class OrbitCameraController; class OverheadCameraController; class CameraController;

	enum class HUDAxesLocation : unsigned int
	{
		TopLeft = 0u,
		BottomLeft,
		BottomRight
	};

	class SGXSCENEVIEWER_API SceneViewer : public QOpenGLWidget
	{
		Q_OBJECT

	public:
		SceneViewer( QWidget *parent = nullptr );
		SceneViewer( const SceneViewer& ) = delete;
		~SceneViewer();

		void initializeGL() override;
		void resizeGL( int w, int h ) override;
		void paintGL() override;

		void keyPressEvent( QKeyEvent* event ) override;
		void keyReleaseEvent( QKeyEvent* event ) override;

		void mouseMoveEvent( QMouseEvent* event ) override;
		void mousePressEvent( QMouseEvent* event ) override;
		void mouseReleaseEvent( QMouseEvent* event ) override;
		void wheelEvent( QWheelEvent* event ) override;

		GlyphScene& getScene() { return scene; }

		void resetCamera();
		void enableFlyToObject( bool enabled );
		void enableAnimation( bool enabled ) { animation_enabled = enabled; }
		bool animationEnabled() { return animation_enabled; }
		bool stereoSupported() { return false; /* not yet implemented */ }
		void setStereoMode( bool enable ) { hal::debug::_assert( false, "stereo mode not yet implemented" ); }
		bool stereoMode() { return false; /* not yet implemented */ }

		void enableSceneAxes( bool enabled ) { scene_axes_enabled = enabled; }
		void enableHUDAxes( bool enabled ) { hud_axes_enabled = enabled; }
		void setHUDAxesLocation( HUDAxesLocation loc ) { hud_axes_location = loc; }
		void showTagsOfSelectedObjects( bool show );
		void setAxisNames( const char* X, const char* Y, const char* Z );
		void hideAllTags();

		void setFilteredResults( const IndexSet& results );
		void setFilteredResultsDisplayMode( FilteredResultsDisplayMode mode );
		FilteredResultsDisplayMode filteredResultsDisplayMode() { return filter_mode; }

		bool hudAxesEnabled() { return hud_axes_enabled; }
		bool sceneAxesEnabled() { return scene_axes_enabled; }
		HUDAxesLocation hudAxesLocation() { return hud_axes_location; }

		void clearScene();
		void loadLegacyScene( const char* nodeFile, const char* tagFile, std::vector<std::string> baseImages );

		void setBackgroundColor( const glm::vec4& color ) { background_color = color; }

		// todo: use Qt signals/slots mechanism for this?
		void setOnSelectionChanged( std::function<void( bool )> fn ) { on_selection_changed = fn; }

	protected:
		void renderText( QPainter& painter, int x, int y, const QString &str, const QFont & font = QFont() );
		void renderText( QPainter& painter, const render::camera* camera, const glm::vec3& pos, const QString &str, const QFont & font = QFont( ) );
		void renderText( QPainter& painter, const glm::vec3& pos, const QString &str, const QFont & font = QFont() );	// shortcut to use 3D camera

	private slots:
		void updateFrame();

	private:
		QToolButton* CreateNavigationButton( const QString& toolTip, bool autoRepeat );
		void checkErrors();
		bool initialized;

		GlyphScene scene;

		QTimer timer;
		QElapsedTimer elapsed_timer;
		QFont hud_font;

		// Logo texture.
		hal::texture* sgx_logo;
		QOpenGLTexture* q_sgx_logo;

		// Input handling.
		static const unsigned int key_states_size = 256u;
		bool key_states[key_states_size];
		int mouse_prev_x, mouse_prev_y;
		int mouse_x, mouse_y;
		float selection_dist, desired_selection_dist;
		glm::vec3 selection_center;
		FreeCameraController* free_cam_control;
		OrbitCameraController* orbit_cam_control;
		OverheadCameraController* overhead_cam_control;
		CameraController* cur_cam_control;
		void set_cam_control( CameraController* cc, bool forceActivate = false );

		enum class button
		{
			left,
			right,
			middle,
			count,
		};

		struct mouse_drag_info
		{
			mouse_drag_info() : dragging( false ), drag_delta_x( 0 ), drag_delta_y( 0 ) { }
			bool dragging;
			int drag_start_x, drag_start_y;
			unsigned int drag_distance_x, drag_distance_y;
			int drag_delta_x, drag_delta_y;
		};
		mouse_drag_info drag[int(button::count)];
		mouse_drag_info& drag_info( button b ) { return drag[int( b )]; }

		float wheel_delta;

		// Settings.
		bool scene_axes_enabled, hud_axes_enabled;
		HUDAxesLocation hud_axes_location;
		bool enable_fly_to_object;
		bool animation_enabled;
		bool wireframe;
		FilteredResultsDisplayMode filter_mode;

		// Callbacks (todo: use Qt signals/slots mechanism for this?)
		std::function<void( bool )> on_selection_changed;

		// Rendering.
		glm::vec3 compute_scene_axis_sizes();
		glm::vec3 compute_scene_axis_origin();
		glm::vec3 compute_hud_axis_origin();
		std::pair<QOpenGLTexture*, hal::texture*> load_texture( const char* filename );
		std::pair<QOpenGLTexture*, hal::texture*> default_base_texture;
		std::vector<std::pair<QOpenGLTexture*, hal::texture*>> base_textures;
		GlyphRenderer* glyph_renderer;
		AxisRenderer* axis_renderer;
		std::string axis_names[3];
		BaseImageRenderer* base_images;
		LinksRenderer* links;
		render::renderer* renderer;
		render::grid_renderer* grids;
		hal::context* context;
		render::perspective_camera* camera;
		render::ortho_camera* ui_camera;
		hal::effect* tex_effect;
		hal::effect* drag_effect;
		render::model* plane;
		render::model* logo;
		render::model* drag_select;
		glm::vec4 background_color;

		//Navigation buttons.
		QToolButton* m_upRotateButton;
		QToolButton* m_leftRotateButton;
		QToolButton* m_rightRotateButton;
		QToolButton* m_downRotateButton;
		QToolButton* m_moveForwardButton;
		QToolButton* m_moveBackwardButton;
		QToolButton* m_moveUpButton;
		QToolButton* m_moveDownButton;
	};
}