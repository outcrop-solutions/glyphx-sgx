#include "antzforestwidget.h"
#include <QtGui/QOpenGLContext>
#include <QtGui/QMouseEvent>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QDebug>
#include <QtGui/QSurfaceFormat>
#include "io/npfile.h"
#include "io/npch.h"
#include "npctrl.h"
#include "npui.h"
#include "ctrl/npengine.h"
#include "npctrl.h"
#include "io/npgl.h"
#include "io/gl/nptags.h"
#include "glyphbuilderapplication.h"
#include "itemfocusselectionmodel.h"
#include <stack>
#include "defaultbaseimagescombobox.h"
#include "glyphnodeconverter.h"
#include <boost/math/constants/constants.hpp>

namespace SynGlyphXANTz {

	const double s_unfilteredTransparencyMultiplier = 0.5;

    ANTzForestWidget::ANTzForestWidget( GlyphForestModel* model, SynGlyphX::ItemFocusSelectionModel* selectionModel, QWidget *parent ) :
        QOpenGLWidget( parent ),
        m_model( model ),
		m_selectionModel( selectionModel ),
		m_antzData( model->GetANTzData() ),
#ifdef USE_ZSPACE
		m_zSpaceContext( nullptr ),
		m_zSpaceDisplay( nullptr ),
		m_zSpaceBuffer( nullptr ),
		m_zSpaceViewport( nullptr ),
		m_zSpaceFrustum( nullptr ),
		m_zSpaceStylus( nullptr ),
#endif
		m_topLevelWindow( nullptr ),
		m_oglTextFont( "Arial", 12, QFont::Normal ),
		m_isReseting( false ),
		m_worldTextureID( 0 ),
		m_lastMousePosition( boost::none ),
		m_regionSelectionRect( QRect() ),
		m_filteredResultsDisplayMode( FilteredResultsDisplayMode::TranslucentUnfiltered ),
		m_drawHUD( true ),
#ifdef USE_ZSPACE
		m_zSpaceOptions(),
#endif
		m_logoTextureID(nullptr),
		m_showAnimation(true),
		m_isInStereo(false),
		m_initialCameraZAngle(45.0f),
		m_sceneAxisInfoQuadric(static_cast<GLUquadric*>(CreateNewQuadricObject())),
		m_showHUDAxisInfoObject(true),
		m_showSceneAxisInfoObject(true),
		m_isStereoSetup(false),
		m_sceneAxisBoundingBox(glm::vec3(-185.0f, -95.0f, 0.0f), glm::vec3(185.0f, 95.0f, 0.0f))
	{
        // Set up timer to attempt to trigger repaints at ~60fps.
        timer.setInterval(16);
        timer.setSingleShot(false);
        connect(&timer,SIGNAL(timeout()), SLOT(update() ) );
        timer.start();

        SetAxisInfoObjectLocation(HUDLocation::TopLeft);

		setFocusPolicy( Qt::StrongFocus );

		QFont newFont = font();
		newFont.setPointSize( 12 );
		setFont( newFont );

		InitIO();

		//Change fly speeds
		m_antzData->GetData()->ctrl.slow = 0.5f;
		m_antzData->GetData()->ctrl.fast = 0.75f;

		if ( m_selectionModel != nullptr ) {
			QObject::connect( m_selectionModel, &SynGlyphX::ItemFocusSelectionModel::selectionChanged, this, &ANTzForestWidget::OnSelectionUpdated );
			QObject::connect( m_selectionModel, &SynGlyphX::ItemFocusSelectionModel::FocusChanged, this, &ANTzForestWidget::OnFocusChanged );
		}

		QObject::connect( m_model, &GlyphForestModel::modelReset, this, &ANTzForestWidget::OnModelReset );
		QObject::connect( m_model, &GlyphForestModel::modelAboutToBeReset, this, [this]{ m_isReseting = true; } );

		setFocus();
	}

	ANTzForestWidget::~ANTzForestWidget()
	{
		makeCurrent();
		if ( m_worldTextureID != nullptr ) {

            delete m_worldTextureID;
			m_worldTextureID = nullptr;
		}

		if ( m_logoTextureID != nullptr ) {

			delete m_logoTextureID;
			m_logoTextureID = nullptr;
		}

		gluDeleteQuadric(m_sceneAxisInfoQuadric);
#ifdef USE_ZSPACE
		ClearZSpaceContext();
#endif
		npCloseGL( m_antzData->GetData() );
	}
    
    namespace
    {
        void transform(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
        {
            auto M = [](int row, int col) { return col * 4 + row; };
            out[0] = M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
            out[1] = M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
            out[2] = M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
            out[3] = M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
        }
        
        void project(GLdouble objx, GLdouble objy, GLdouble objz,
                     const GLdouble model[16], const GLdouble proj[16],
                     const GLint viewport[4],
                     GLdouble& winx, GLdouble& winy, GLdouble& winz)
        {
            GLdouble in[4], out[4];
            
            in[0] = objx;
            in[1] = objy;
            in[2] = objz;
            in[3] = 1.0;
            transform(out, model, in);
            transform(in, proj, out);
            
            in[0] /= in[3];
            in[1] /= in[3];
            in[2] /= in[3];
            
            winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
            winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;
            winz = (1 + in[2]) / 2;
        }
    }
    
    void ANTzForestWidget::renderText(double x, double y, double z, const QString& str, const QFont &font)
    {
        GLdouble model[16], proj[16];
        GLint view[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);
        GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;
        
        project(x, y, z,
                model, proj, view,
                textPosX, textPosY, textPosZ);
        
        textPosY = height() - textPosY;
        
        QPainter painter(this);
        painter.setPen(Qt::white);
        painter.setFont(font);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        painter.drawText(textPosX, textPosY, str);
        painter.end();
        
        // Restore default blend mode since QPainter seems to trash it on OSX.
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void ANTzForestWidget::renderText(int x, int y, const QString& str, const QFont &font)
    {
        QPainter painter(this);
        painter.setPen(Qt::white);
        painter.setFont(font);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        painter.drawText(x, y, str);
        painter.end();
        
        // Restore default blend mode since QPainter seems to trash it on OSX.
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void ANTzForestWidget::qglColor(QColor color)
    {
        glColor4f(color.redF(), color.blueF(), color.greenF(), color.alphaF());
    }
    
#ifdef USE_ZSPACE
	void ANTzForestWidget::ClearZSpaceContext() {

		if ( m_zSpaceContext != nullptr ) {

			zsShutdown( m_zSpaceContext );
		}
	}

	void ANTzForestWidget::ConnectZSpaceTrackers() {

		ZSError error = zsAddTrackerEventHandler( m_zSpaceStylus, ZS_TRACKER_EVENT_BUTTON_PRESS, &SynGlyphX::ZSpaceEventDispatcher::Dispatch, &m_zSpaceEventDispatcher );
		CheckZSpaceError( error );
		QObject::connect( &m_zSpaceEventDispatcher, &SynGlyphX::ZSpaceEventDispatcher::ZSpaceButtonPressed, this, &ANTzForestWidget::ZSpaceButtonPressHandler, Qt::BlockingQueuedConnection );
		//QObject::connect(&m_zSpaceEventDispatcher, &ZSpaceEventDispatcher::ZSpaceButtonPressed, this, &ANTzForestWidget::ZSpaceButtonReleaseHandler, Qt::BlockingQueuedConnection);

		error = zsAddTrackerEventHandler( m_zSpaceStylus, ZS_TRACKER_EVENT_BUTTON_RELEASE, &SynGlyphX::ZSpaceEventDispatcher::Dispatch, &m_zSpaceEventDispatcher );
		CheckZSpaceError( error );
		QObject::connect( &m_zSpaceEventDispatcher, &SynGlyphX::ZSpaceEventDispatcher::ZSpaceButtonReleased, this, &ANTzForestWidget::ZSpaceButtonReleaseHandler, Qt::BlockingQueuedConnection );

		error = zsAddTrackerEventHandler( m_zSpaceStylus, ZS_TRACKER_EVENT_MOVE, &SynGlyphX::ZSpaceEventDispatcher::Dispatch, &m_zSpaceEventDispatcher );
		CheckZSpaceError( error );
		QObject::connect( &m_zSpaceEventDispatcher, &SynGlyphX::ZSpaceEventDispatcher::ZSpaceStylusMoved, this, &ANTzForestWidget::ZSpaceStylusMoveHandler, Qt::BlockingQueuedConnection );

		//error = zsAddTrackerEventHandler(m_zSpaceStylus, ZS_TRACKER_EVENT_TAP_SINGLE, &ZSpaceEventDispatcher, this);
		//CheckZSpaceError(error);
	}

	void ANTzForestWidget::CheckZSpaceError( ZSError error ) {

		if ( error != ZS_ERROR_OKAY ) {

			QString zSpaceErrorString = tr( "zSpace error in " );
			zSpaceErrorString += ":";
			if ( error == ZS_ERROR_NOT_IMPLEMENTED ) {

				zSpaceErrorString += "Not Implemented";
			}
			else if ( error == ZS_ERROR_NOT_INITIALIZED ) {

				zSpaceErrorString += "Not Initalized";
			}
			else if ( error == ZS_ERROR_ALREADY_INITIALIZED ) {

				zSpaceErrorString += "Already Initalized";
			}
			else if ( error == ZS_ERROR_INVALID_PARAMETER ) {

				zSpaceErrorString += "Invalid Parameter";
			}
			else if ( error == ZS_ERROR_INVALID_CONTEXT ) {

				zSpaceErrorString += "Invalid Context";
			}
			else if ( error == ZS_ERROR_INVALID_HANDLE ) {

				zSpaceErrorString += "Invalid Handle";
			}
			else if ( error == ZS_ERROR_RUNTIME_INCOMPATIBLE ) {

				zSpaceErrorString += "Incompatible Runtime";
			}
			else if ( error == ZS_ERROR_RUNTIME_NOT_FOUND ) {

				zSpaceErrorString += "Runtime Not Found";
			}
			else if ( error == ZS_ERROR_SYMBOL_NOT_FOUND ) {

				zSpaceErrorString += "Symbol Not Found";
			}
			else if ( error == ZS_ERROR_DISPLAY_NOT_FOUND ) {

				zSpaceErrorString += "Display Not Found";
			}
			else if ( error == ZS_ERROR_DEVICE_NOT_FOUND ) {

				zSpaceErrorString += "Device Not Found";
			}
			else if ( error == ZS_ERROR_TARGET_NOT_FOUND ) {

				zSpaceErrorString += "Target Not Found";
			}
			else if ( error == ZS_ERROR_CAPABILITY_NOT_FOUND ) {

				zSpaceErrorString += "Error Capability Not Found";
			}
			else if ( error == ZS_ERROR_BUFFER_TOO_SMALL ) {

				zSpaceErrorString += "Buffer too small";
			}

			throw std::runtime_error(zSpaceErrorString.toStdString().c_str());
		}
	}
#endif

	void ANTzForestWidget::InitIO()
	{
		pData data = m_antzData->GetData();

		//zz-CAMERA
		//compare intialization values with working project, debug zz

		data->io.mouse.target.x = 0.0f;
		data->io.mouse.target.y = 0.0f;
		data->io.mouse.target.z = 0.0f;

		data->io.mouse.targetDest.x = 0.0f;
		data->io.mouse.targetDest.y = 0.0f;
		data->io.mouse.targetDest.z = 0.0f;

		data->io.mouse.targetRadius = 0.0f;
		data->io.mouse.targetRadiusDest = 0.0f;

		data->io.mouse.targeting = false;

		data->io.mouse.x = 0;						//raw coordinate
		data->io.mouse.y = 0;
		data->io.mouse.z = 0;						//typically the scroll wheel

		data->io.mouse.mode = kNPmouseModeNull;
		//data->io.mouse.pickMode = kNPmodeNull;
		//	data->io.mouse.pickMode = kNPmodeCamera;
		data->io.mouse.pickMode = kNPmodePin;
		data->io.mouse.tool = kNPtoolNull;

		data->io.mouse.buttonL = false;					//true when pressed
		data->io.mouse.buttonC = false;
		data->io.mouse.buttonR = false;

		data->io.mouse.window.x = 0;					//screen position in pixels
		data->io.mouse.window.y = 0;
		data->io.mouse.window.z = 0;

		data->io.mouse.previous.x = 0;
		data->io.mouse.previous.y = 0;
		data->io.mouse.previous.z = 0;

		data->io.mouse.cmDX = 0.0f;
		data->io.mouse.cmDY = 0.0f;

		data->io.mouse.delta.x = 0.0f;					//mouse vector
		data->io.mouse.delta.y = 0.0f;					//clears every mouse event
		data->io.mouse.delta.z = 0.0f;					//z is typically scroll wheel

		data->io.mouse.deltaSum.x = 0.0f;				//clears each NPE period
		data->io.mouse.deltaSum.y = 0.0f;
		data->io.mouse.deltaSum.z = 0.0f;

		data->io.mouse.pinSelected = false;

		data->io.mouse.createEvent = false;
		data->io.mouse.singleClick = false;
		data->io.mouse.doubleClick = false;

		data->io.mouse.linkA = NULL;

		data->io.mouse.size = sizeof( NPmouse );
	}

	void ANTzForestWidget::initializeGL() {

		pData antzData = m_antzData->GetData();
		npInitGLDraw(antzData);
		npInitGLPrimitive(antzData);
		npInitTags(antzData);

		QImage image(SynGlyphX::GlyphBuilderApplication::GetLogoLocation(SynGlyphX::GlyphBuilderApplication::WhiteBorder));
		m_logoPosition.setCoords(0, 0, 0, 0);
		m_logoPosition.setSize(QSize(image.width(), -image.height()));
		m_logoTextureID = new QOpenGLTexture(image.mirrored());

		m_worldTextureID = BindTextureInFile(SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation() + QString::fromStdWString(SynGlyphX::DefaultBaseImageProperties::GetBasefilename()));
		pNPnode rootGrid = static_cast<pNPnode>(antzData->map.node[kNPnodeRootGrid]);
		SetGridTexture(rootGrid);
		SetGridLinesColor(rootGrid, Qt::blue);
		antzData->io.gl.textureCount = 1;
		SetCameraToDefaultPosition();
	}

	void ANTzForestWidget::resizeGL( int w, int h ) {
#ifdef USE_ZSPACE
		ResizeZSpaceViewport();
#endif
		QSize logoSize = m_logoPosition.size();
		m_logoPosition.setLeft(w - logoSize.width() - 10);
		m_logoPosition.setTop(h - 10);
		m_logoPosition.setSize(logoSize);

		npGLResizeScene(w, h);

		SetAxisInfoObjectLocation(m_hudAxisInfoObjectLocation);
	}

	void ANTzForestWidget::paintGL() {

		if ( !context()->isValid() ) {

			return;
		}

		glDrawBuffer( GL_BACK_LEFT );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		if ( IsStereoSupported() ) {

			glDrawBuffer( GL_BACK_RIGHT );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		}

		if ( m_isReseting ) {

			return;
		}

		pData antzData = m_antzData->GetData();

		if ( antzData->io.gl.pickPass ) {

			return;
		}
        
        QElapsedTimer timer;
        timer.start();
		npUpdateCh( antzData );

		npUpdateEngine( antzData );		//position, physics, interactions...
		UpdateBoundingBoxes();
//        qDebug() << "3D update took " << timer.elapsed() << "ms.";

		//We may need to have a selected pin node during update to position the camera, but we don't want it during drawing
		antzData->map.selectedPinNode = NULL;
		antzData->map.selectedPinIndex = 0;

		// zero out our mouse to prevent drifting objects
		antzData->io.mouse.delta.x = 0.0f;
		antzData->io.mouse.delta.y = 0.0f;

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		npSetLookAtFromCamera( antzData );

#ifdef USE_ZSPACE
		if ( IsInZSpaceStereo() ) {

			ZSError error = zsUpdate( m_zSpaceContext );
			error = zsBeginStereoBufferFrame( m_zSpaceBuffer );
			glGetFloatv( GL_MODELVIEW_MATRIX, m_originialViewMatrix.f );
		}
#endif

		DrawSceneForEye( Eye::Left, true );
		if ( IsInStereoMode() )  {

			DrawSceneForEye( Eye::Right, false );
		}

		/*
		while ( auto error = glGetError() )
		{
			qDebug() << "OpenGL Error " << error << ": " << reinterpret_cast<const char*>( gluErrorString( error ) );
		}
		*/
	}

	void ANTzForestWidget::DrawSceneForEye( Eye eye, bool getStylusWorldPosition ) {

		pData antzData = m_antzData->GetData();
		pNPnode camNode = npGetActiveCam( antzData );
		NPcameraPtr camData = static_cast<NPcameraPtr>( camNode->data );

#ifdef USE_ZSPACE
		ZSEye zSpaceEye;
		if ( eye == Eye::Left ) {

			glDrawBuffer( GL_BACK_LEFT );
			zSpaceEye = ZS_EYE_LEFT;
		}
		else {

			glDrawBuffer( GL_BACK_RIGHT );
			zSpaceEye = ZS_EYE_RIGHT;
		}
#endif

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();

#ifdef USE_ZSPACE
		if ( IsInZSpaceStereo() ) {

			ZSTrackerPose stylusPose;
			ZSError error = zsGetTargetPose( m_zSpaceStylus, &stylusPose );

			// Transform the stylus pose from tracker to camera space.
			error = zsTransformMatrix( m_zSpaceViewport, ZS_COORDINATE_SPACE_TRACKER, ZS_COORDINATE_SPACE_CAMERA, &stylusPose.matrix );

			ZSMatrix4 displayToCameraMatrix;
			error = zsTransformMatrix( m_zSpaceViewport, ZS_COORDINATE_SPACE_DISPLAY, ZS_COORDINATE_SPACE_CAMERA, &displayToCameraMatrix );

			ZSMatrix4 originalCameraMatrix;
			npInvertMatrixf( m_originialViewMatrix.f, originalCameraMatrix.f );

			// Transform the stylus pose from camera space to world space.
			// This is done by multiplying the pose (camera space) by the 
			// application's camera matrix.
			npMultMatrix( m_zSpaceStylusWorldMatrix.f, originalCameraMatrix.f, stylusPose.matrix.f );
			npMultMatrix( m_zSpaceDisplayWorldMatrix.f, originalCameraMatrix.f, displayToCameraMatrix.f );

			SetZSpaceMatricesForDrawing( zSpaceEye, m_originialViewMatrix, camData );
		}
		else if ( !IsInStereoMode() ) 
#endif
		{

			glGetFloatv( GL_MODELVIEW_MATRIX, camData->matrix );
			npInvertMatrixf( camData->matrix, camData->inverseMatrix );
		}

		glMatrixMode( GL_MODELVIEW );

		npGLLighting( antzData );
		npGLShading( antzData );
		npDrawNodes( antzData );

		if (!antzData->io.gl.pickPass) {

			DrawSceneAxisInfoObject();

			//Leave this here so that bounding boxes can be shown for debugging
			//DrawBoundingBoxes();

#ifdef USE_ZSPACE
			DrawZSpaceStylus(m_zSpaceStylusWorldMatrix, getStylusWorldPosition);
#endif

			if (m_drawHUD) {

				DrawHUD();
			}

			DrawLogo();
		}

		glMatrixMode( GL_PROJECTION );
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
        
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
	}

#ifdef USE_ZSPACE
	void ANTzForestWidget::DrawZSpaceStylus( const ZSMatrix4& stylusMatrix, bool getStylusWorldPosition ) {

		if ( IsInZSpaceStereo() ) {

			pData antzData = m_antzData->GetData();
			pNPnode camNode = npGetActiveCam( antzData );
			NPcameraPtr camData = static_cast<NPcameraPtr>( camNode->data );

			const QColor& stylusColor = m_zSpaceOptions.GetStylusColor();
			glColor3f(stylusColor.redF(), stylusColor.greenF(), stylusColor.blueF());

			// Multiply the model-view matrix by the stylus world matrix.
			glMatrixMode( GL_MODELVIEW );
			glPushMatrix();
			glMultMatrixf( stylusMatrix.f );

			// Draw the line.
			glBegin( GL_LINES );
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( 0.0f, 0.0f, -m_zSpaceOptions.GetStylusLength() );

			glEnd();

			if ( getStylusWorldPosition ) {

				NPfloatXYZ world = { 0.0f, 0.0f, 0.0f };
				GLfloat modelView[16];
				glGetFloatv( GL_MODELVIEW_MATRIX, modelView );
				npLocalToWorld( &world, camData->inverseMatrix, modelView );

				m_stylusWorldLine.first.x = world.x;
				m_stylusWorldLine.first.y = world.y;
				m_stylusWorldLine.first.z = world.z;

				//m_stylusWorldTapLine.first = m_stylusWorldLine.first;

				glTranslatef( 0.0f, 0.0f, -m_zSpaceOptions.GetStylusLength() );
				//glScalef(0.0025f, 0.0025f, 0.0025f);
				//npGLPrimitive(kNPgeoSphere, 0.1f);
				glGetFloatv( GL_MODELVIEW_MATRIX, modelView );
				npLocalToWorld( &world, camData->inverseMatrix, modelView );

				m_stylusWorldLine.second.x = world.x;
				m_stylusWorldLine.second.y = world.y;
				m_stylusWorldLine.second.z = world.z;

			}

			glPopMatrix();

			/*if (getStylusWorldPosition) {

			glPushMatrix();
			glMultMatrixf(m_zSpaceDisplayWorldMatrix.f);
			glTranslatef(0.0f, 0.0f, -1.0);

			NPfloatXYZ world = { 0.0f, 0.0f, 0.0f };
			GLfloat modelView[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
			npLocalToWorld(&world, camData->inverseMatrix, modelView);

			m_stylusWorldTapLine.second.x = m_stylusWorldTapLine.first.x + world.x;
			m_stylusWorldTapLine.second.y = m_stylusWorldTapLine.first.y + world.y;
			m_stylusWorldTapLine.second.z = m_stylusWorldTapLine.first.z + world.z;

			glPopMatrix();
			}*/
		}
	}
#endif

	void* ANTzForestWidget::CreateNewQuadricObject() {

		GLUquadric* gluObject = nullptr;

		gluObject = gluNewQuadric();
		gluQuadricDrawStyle(gluObject, GLU_FILL);
		//gluQuadricTexture(gluObject, TRUE);
		gluQuadricNormals(gluObject, GLU_SMOOTH);

		return gluObject;
	}

	bool ANTzForestWidget::IsHUDLocationOnRightSide(HUDLocation location) const {

		return (location == HUDLocation::BottomRight);
	}

	void ANTzForestWidget::DrawSceneAxisInfoObject() {

		if (!m_showSceneAxisInfoObject) {

			return;
		}

		pData antzData = m_antzData->GetData();
		const std::array<QString, 3>& mappedFields = m_model->GetRootPosXYZMappedFields();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		glTranslatef(-181.0f, -91.0f, 0.0f);

		glColor3f(1.0f, 0.0f, 0.0f);

		glPushMatrix();

		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 1.0, 1.0, 361.0, 24, 1);

		glTranslatef(0.0f, 0.0f, 361.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 3.0, 0.0, 10.0, 24, 1);

		GLdouble xTextModelMatrix[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, xTextModelMatrix);

		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluDisk(m_sceneAxisInfoQuadric, 0.0, 3.0, 24, 1);
		
		glPopMatrix();

		glColor3f(0.0f, 1.0f, 0.0f);

		glPushMatrix();

		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 1.0, 1.0, 181.0, 24, 1);

		glTranslatef(0.0f, 0.0f, 181.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 3.0, 0.0, 10.0, 24, 1);

		GLdouble yTextModelMatrix[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, yTextModelMatrix);

		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluDisk(m_sceneAxisInfoQuadric, 0.0, 3.0, 24, 1);

		glPopMatrix();

		glColor3f(0.0f, 0.0f, 1.0f);

		glPushMatrix();

		glTranslatef(0.0f, 0.0f, -0.5f);
		gluCylinder(m_sceneAxisInfoQuadric, 1.0, 1.0, 180.5, 24, 1);

		glTranslatef(0.0f, 0.0f, 180.5f);
		gluCylinder(m_sceneAxisInfoQuadric, 3.0, 0.0, 5.0, 24, 1);

		GLdouble zTextModelMatrix[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, zTextModelMatrix);

		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluDisk(m_sceneAxisInfoQuadric, 0.0, 3.0, 24, 1);

		glPopMatrix();

		//Wait until end to draw text
		qglColor(Qt::white);
		
		GLdouble orthoMatrix[16];
		glGetDoublev(GL_PROJECTION_MATRIX, orthoMatrix);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glDisable(GL_DEPTH_TEST);

		GLdouble textPosition[3];

		if (!mappedFields[0].isEmpty()) {

			gluProject(0.0, 0.0, 12.0, xTextModelMatrix, orthoMatrix, viewport, &textPosition[0], &textPosition[1], &textPosition[2]);
			renderText(textPosition[0], antzData->io.gl.height - textPosition[1], mappedFields[0], m_oglTextFont);
		}

		if (!mappedFields[1].isEmpty()) {

			gluProject(0.0, 0.0, 12.0, yTextModelMatrix, orthoMatrix, viewport, &textPosition[0], &textPosition[1], &textPosition[2]);
			renderText(textPosition[0], antzData->io.gl.height - textPosition[1], mappedFields[1], m_oglTextFont);
		}

		if (!mappedFields[2].isEmpty()) {

			gluProject(0.0, 0.0, 8.0, zTextModelMatrix, orthoMatrix, viewport, &textPosition[0], &textPosition[1], &textPosition[2]);
			renderText(textPosition[0], antzData->io.gl.height - textPosition[1], mappedFields[2], m_oglTextFont);
		}

		glEnable(GL_DEPTH_TEST);
		
		glPopMatrix();
	}

	void ANTzForestWidget::DrawHUDAxisInfoObject() {

		if (!m_showHUDAxisInfoObject) {

			return;
		}

		pData antzData = m_antzData->GetData();
		pNPnode currentCamera = npGetActiveCam(antzData);

		GLdouble xTextMatrix[16];
		GLdouble yTextMatrix[16];
		GLdouble zTextMatrix[16];

		bool flipXTextPosition = ((currentCamera->rotate.y > 90.0) && (currentCamera->rotate.y <= 270.0));
		bool flipYTextPosition = (currentCamera->rotate.y > 180.0);

		const std::array<QString, 3>& mappedFields = m_model->GetRootPosXYZMappedFields();
		std::array<unsigned int, 3> mappedFieldsWidth = { { 0, 0, 0 } };

		bool putTextOnLeftSide = IsHUDLocationOnRightSide(m_hudAxisInfoObjectLocation);
		if (putTextOnLeftSide) {

			QFontMetrics oglFontMetrics(m_oglTextFont);
			mappedFieldsWidth[0] = oglFontMetrics.width(mappedFields[0]);
			mappedFieldsWidth[1] = oglFontMetrics.width(mappedFields[1]);
			mappedFieldsWidth[2] = std::max(0, oglFontMetrics.width(mappedFields[2]) - 65);

			flipXTextPosition = !flipXTextPosition;
			flipYTextPosition = !flipYTextPosition;
		}

		GLint viewport[4] = { m_hudAxisInfoViewport.left(), m_hudAxisInfoViewport.top(), m_hudAxisInfoViewport.width(), m_hudAxisInfoViewport.height() };
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(m_hudAxisInfoOrtho.left(), m_hudAxisInfoOrtho.right(), m_hudAxisInfoOrtho.top(), m_hudAxisInfoOrtho.bottom(), -10.0, 10.0);

		GLdouble orthoMatrix[16];
		glGetDoublev(GL_PROJECTION_MATRIX, orthoMatrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(0.0, 2.5, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

		glColor3f(1.0f, 0.0f, 0.0f);

		glPushMatrix();
		
		glRotatef(currentCamera->rotate.y, 0.0f, 1.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, -3.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 0.1, 0.1, 6.0, 24, 1);

		gluDisk(m_sceneAxisInfoQuadric, 0.0, 0.4, 24, 1);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 0.4, 0.0, 1.0, 24, 1);

		if (flipXTextPosition) {

			glGetDoublev(GL_MODELVIEW_MATRIX, xTextMatrix);
		}

		glTranslatef(0.0f, 0.0f, -6.0f);
		gluDisk(m_sceneAxisInfoQuadric, 0.0, 0.4, 24, 1);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 0.4, 0.0, 1.0, 24, 1);

		if (!flipXTextPosition) {

			glGetDoublev(GL_MODELVIEW_MATRIX, xTextMatrix);
		}
		
		glPopMatrix();

		glColor3f(0.0f, 1.0f, 0.0f);

		glPushMatrix();	

		glRotatef(currentCamera->rotate.y, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, -3.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 0.1, 0.1, 6.0, 24, 1);

		gluDisk(m_sceneAxisInfoQuadric, 0.0, 0.4, 24, 1);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 0.4, 0.0, 1.0, 24, 1);

		if (flipYTextPosition) {

			glGetDoublev(GL_MODELVIEW_MATRIX, yTextMatrix);
		}

		glTranslatef(0.0f, 0.0f, -6.0f);
		gluDisk(m_sceneAxisInfoQuadric, 0.0, 0.4, 24, 1);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 0.4, 0.0, 1.0, 24, 1);

		if (!flipYTextPosition) {

			glGetDoublev(GL_MODELVIEW_MATRIX, yTextMatrix);
		}

		glPopMatrix();
		
		glColor3f(0.0f, 0.0f, 1.0f);

		glPushMatrix();

		glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, -3.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 0.1, 0.1, 6.0, 24, 1);

		gluDisk(m_sceneAxisInfoQuadric, 0.0, 0.4, 24, 1);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 0.4, 0.0, 1.0, 24, 1);

		glTranslatef(0.0f, 0.0f, -6.0f);
		gluDisk(m_sceneAxisInfoQuadric, 0.0, 0.4, 24, 1);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(m_sceneAxisInfoQuadric, 0.4, 0.0, 1.0, 24, 1);

		glGetDoublev(GL_MODELVIEW_MATRIX, zTextMatrix);

		//glPopMatrix();

		//Wait until end to draw text
		qglColor(Qt::white);
		//glPushMatrix();
		glDisable(GL_DEPTH_TEST);

		GLdouble textPosition[3];

		if (!mappedFields[0].isEmpty()) {

			gluProject(-0.25, 0.0, 1.5, xTextMatrix, orthoMatrix, viewport, &textPosition[0], &textPosition[1], &textPosition[2]);
			renderText(textPosition[0] - mappedFieldsWidth[0], antzData->io.gl.height - textPosition[1], mappedFields[0], m_oglTextFont);
		}

		if (!mappedFields[1].isEmpty()) {

			gluProject(-0.25, 0.0, 1.5, yTextMatrix, orthoMatrix, viewport, &textPosition[0], &textPosition[1], &textPosition[2]);
			renderText(textPosition[0] - mappedFieldsWidth[1], antzData->io.gl.height - textPosition[1], mappedFields[1], m_oglTextFont);
		}
		
		if (!mappedFields[2].isEmpty()) {

			gluProject(-0.25, 0.0, 1.5, zTextMatrix, orthoMatrix, viewport, &textPosition[0], &textPosition[1], &textPosition[2]);
			renderText(textPosition[0] - mappedFieldsWidth[2], antzData->io.gl.height - textPosition[1], mappedFields[2], m_oglTextFont);
		}

		glEnable(GL_DEPTH_TEST);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glViewport(0, 0, antzData->io.gl.width, antzData->io.gl.height);
	}

	void ANTzForestWidget::DrawHUD() {

		pData antzData = m_antzData->GetData();

		bool reenableDepthTest = glIsEnabled( GL_DEPTH_TEST );
		if ( reenableDepthTest ) {
			glDisable( GL_DEPTH_TEST );
		}

		//Draw region selection rectangle
		if ( ( m_regionSelectionRect.width() > 0 ) && ( m_regionSelectionRect.height() > 0 ) ) {

			glMatrixMode( GL_PROJECTION );
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D( 0, antzData->io.gl.width, 0, antzData->io.gl.height );

			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			glColor4ub( 64, 32, 0, 100 );

			int glRegionLeft = m_regionSelectionRect.x();
			int glRegionRight = m_regionSelectionRect.x() + m_regionSelectionRect.width();
			int glRegionTop = antzData->io.gl.height - m_regionSelectionRect.y();
			int glRegionBottom = glRegionTop - m_regionSelectionRect.height();

			glBegin( GL_QUADS );
			glVertex2i( glRegionLeft, glRegionTop );
			glVertex2f( glRegionLeft, glRegionBottom );
			glVertex2f( glRegionRight, glRegionBottom );
			glVertex2f( glRegionRight, glRegionTop );
			glEnd();

			glColor4ub( 255, 255, 0, 100 );

			glBegin( GL_LINE_LOOP );
			glVertex2i( glRegionLeft, glRegionTop );
			glVertex2f( glRegionLeft, glRegionBottom );
			glVertex2f( glRegionRight, glRegionBottom );
			glVertex2f( glRegionRight, glRegionTop );
			glEnd();

			glMatrixMode( GL_PROJECTION );
			glPopMatrix();
			glMatrixMode( GL_MODELVIEW );
			glPopMatrix();
		}

		//Draw tags
		qglColor( Qt::white );

		Q_FOREACH( const QModelIndex& modelIndex, m_tagIndexes ) {

			pNPnode selectedNode = static_cast<pNPnode>( modelIndex.internalPointer() );
			QString tag = QString::fromStdWString( SynGlyphXANTz::GlyphNodeConverter::GetTag( selectedNode ) );
			if ( m_model->IsTagShownIn3d( tag ) && ( !selectedNode->hide ) ) {

				renderText( selectedNode->screen.x, height() - selectedNode->screen.y, tag, m_oglTextFont );
			}
		}

		//Draw HUD
		QString positionHUD;

		if ( !m_selectionModel->GetFocusList().empty() ) {

			pNPnode lastFocusNode = static_cast<pNPnode>( m_selectionModel->GetFocusList().back().internalPointer() );
			positionHUD = tr("Glyph Position: X: %1, Y: %2, Z: %3").arg(QString::number(lastFocusNode->world.x, 'f', 3), QString::number(lastFocusNode->world.y, 'f', 3), QString::number(lastFocusNode->world.z, 'f', 3));
		}
		else {

			positionHUD = tr( "Camera Position: X: %1, Y: %2, Z: %3" ).arg( QString::number( antzData->map.currentCam->translate.x, 'f', 3 ), QString::number( antzData->map.currentCam->translate.y, 'f', 3 ), QString::number( antzData->map.currentCam->translate.z, 'f', 3 ) );
		}

		QFontMetrics hudFontMetrics( m_oglTextFont );
		renderText( ( width() / 2 ) - ( hudFontMetrics.width( positionHUD ) / 2 ), height() - 16, positionHUD, m_oglTextFont );

		if ( reenableDepthTest ) {
			glEnable( GL_DEPTH_TEST );
		}

		DrawHUDAxisInfoObject();
	}

#ifdef USE_ZSPACE
	void ANTzForestWidget::SetZSpaceMatricesForDrawing( ZSEye eye, const ZSMatrix4& originialViewMatrix, NPcameraPtr camData ) {

		ZSMatrix4 zSpaceEyeViewMatrix;
		ZSError mverror = zsGetFrustumViewMatrix( m_zSpaceFrustum, eye, &zSpaceEyeViewMatrix );

		glMatrixMode( GL_MODELVIEW );

		//Probably should be at bottom of this function
		//glGetFloatv(GL_MODELVIEW_MATRIX, camData->matrix);
		//npInvertMatrixf(camData->matrix, camData->inverseMatrix);

		glLoadMatrixf( zSpaceEyeViewMatrix.f );
		glMultMatrixf( originialViewMatrix.f );

		glMatrixMode( GL_PROJECTION );
		ZSMatrix4 projectionMatrix;
		ZSError projError = zsGetFrustumProjectionMatrix( m_zSpaceFrustum, eye, &projectionMatrix );

		glLoadMatrixf( projectionMatrix.f );
		//glMultMatrixf(m_originialProjectionMatrix.f);

		glGetFloatv( GL_MODELVIEW_MATRIX, camData->matrix );
		npInvertMatrixf( camData->matrix, camData->inverseMatrix );
	}
#endif

	void ANTzForestWidget::OnSelectionUpdated( const QItemSelection& selected, const QItemSelection& deselected ) {

		pData antzData = m_antzData->GetData();

		//unselect all nodes that are no longer selected
		const QModelIndexList& deselectedIndicies = deselected.indexes();
		for ( int i = 0; i < deselectedIndicies.length(); ++i ) {
			if ( deselectedIndicies[i].isValid() ) {
				pNPnode node = static_cast<pNPnode>( deselectedIndicies[i].internalPointer() );
				node->selected = false;
			}
		}

		//select all newly selected nodes
		const QModelIndexList& selectedIndicies = selected.indexes();
		for ( int i = 0; i < selectedIndicies.length(); ++i ) {
			if ( selectedIndicies[i].isValid() ) {
				pNPnode node = static_cast<pNPnode>( selectedIndicies[i].internalPointer() );
				node->selected = true;
			}
		}
	}

	void ANTzForestWidget::OnFocusChanged( const QModelIndexList& indexes ) {

		pData antzData = m_antzData->GetData();

		int nodeRootIndex = 0;
		if ( !indexes.isEmpty() )  {
			const QModelIndex& last = indexes.back();
			if ( last.isValid() ) {

				pNPnode node = static_cast<pNPnode>( last.internalPointer() );
				CenterCameraOnNode( node );
				while ( node->branchLevel != 0 ) {
					node = node->parent;
				}
				nodeRootIndex = node->id;
			}
		}
		else {
			//Clear the proximity value so that camera isn't locked to the node even when not selected
			antzData->map.currentCam->proximity.x = 0;
		}

		antzData->map.nodeRootIndex = nodeRootIndex;
	}

	void ANTzForestWidget::HideFilteredGlyph(pNPnode node, bool hide) {

		if (m_filteredResultsDisplayMode == FilteredResultsDisplayMode::HideUnfiltered) {

			node->hide = hide;
		}
		else {

			if (hide) {

				node->color.a = static_cast<unsigned char>(s_unfilteredTransparencyMultiplier * node->color.a);
			}
			else {

				node->color.a = node->m_originalAlpha;
			}
		}

		for ( unsigned int i = 0; i < node->childCount; ++i ) {

			HideFilteredGlyph(node->child[i], hide);
		}
	}

	void ANTzForestWidget::SetFilteredResultsDisplayMode( FilteredResultsDisplayMode mode ) {

		if ( mode != m_filteredResultsDisplayMode ) {

			HideFilteredGlyph(false);
			m_filteredResultsDisplayMode = mode;
			HideFilteredGlyph(true);
		}
	}

	void ANTzForestWidget::HideFilteredGlyph(bool hide) {

		if (m_filteredResults.empty()) {

			return;
		}

		pData antzData = m_antzData->GetData();
		for ( unsigned int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i ) {

			if (m_filteredResults.count(i - kNPnodeRootPin) == 0) {

				pNPnode node = static_cast<pNPnode>(antzData->map.node[i]);
				HideFilteredGlyph(node, hide);
			}
		}
	}

	void ANTzForestWidget::SetFilteredResults( const SynGlyphX::IndexSet& filteredResults ) {

		HideFilteredGlyph(false);
		m_filteredResults = filteredResults;
		HideFilteredGlyph(true);
	}

	void ANTzForestWidget::ClearFilteredResults() {

		HideFilteredGlyph(false);
		m_filteredResults.clear();
	}

	void ANTzForestWidget::SetCameraToDefaultPosition() {

		pData antzData = m_antzData->GetData();

		antzData->map.currentCam->proximity.x = 0.0f;
		antzData->map.currentCam->translate.x = 0.0f;
		antzData->map.currentCam->translate.y = -345.0f;
		antzData->map.currentCam->translate.z = 345.0f;
		antzData->map.currentCam->rotate.x = m_initialCameraZAngle;
		antzData->map.currentCam->rotate.y = 0.0f;
		antzData->map.currentCam->rotate.z = 0.0f;

		pNPnode node = static_cast<pNPnode>( antzData->map.node[kNPnodeRootGrid] );
		antzData->io.mouse.targeting = false;
		antzData->map.selectedPinNode = node;
		antzData->map.selectedPinIndex = node->id;

		//Always keep current node set to current cam
		antzData->map.currentNode = antzData->map.currentCam;
	}

	void ANTzForestWidget::ResetCamera() {

		pData antzData = m_antzData->GetData();

		//We only want to center the camera when there are actual root nodes
		if ( antzData->map.nodeRootCount >= kNPnodeRootPin ) {

			SetCameraToDefaultPosition();
			update();
		}
		else {

			//If an object is selected make sure that camera doesn't lock into its position
			antzData->map.nodeRootIndex = 0;
			antzData->map.currentCam->proximity.x = 0;
		}
	}

	void ANTzForestWidget::CenterCameraOnNode( pNPnode node ) {

		pData antzData = m_antzData->GetData();

		npSetCamTargetNode( node, antzData );
		antzData->io.mouse.targeting = false;
		antzData->map.selectedPinNode = node;
		antzData->map.selectedPinIndex = node->id;

		//Always keep current node set to current cam
		antzData->map.currentNode = antzData->map.currentCam;
	}

	void ANTzForestWidget::mousePressEvent( QMouseEvent* event ) {

		if ( event->button() == Qt::LeftButton ) {

			Qt::KeyboardModifiers keys = event->modifiers();
			if ( keys & Qt::ShiftModifier ) {

				m_selectionModel->clearSelection();
				m_selectionModel->ClearFocus();
			}
			else {

				SelectAtPoint( event->x(), event->y(), keys & Qt::ControlModifier );
			}
		}

		//Make sure that ANTz combo of left & right button does not cause the context menu to appear
		if ( event->buttons() == Qt::RightButton ) {

			setContextMenuPolicy( Qt::ActionsContextMenu );
		}
		else {

			setContextMenuPolicy( Qt::NoContextMenu );
		}

		m_lastMousePosition = event->pos();
	}

	bool ANTzForestWidget::SelectAtPoint( int x, int y, bool multiSelect ) {

		if ( !rect().contains( x, y ) ) {

			return false;
		}
        
        makeCurrent();

		//emit NewStatusMessage(tr("Selection Attempt At: %1, %2").arg(x).arg(y), 4000);

		int pickID = 0;

		pData antzData = m_antzData->GetData();

		int glX = x;
		int glY = antzData->io.gl.height - y;

		//if (IsInStereoMode()) {

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		npSetLookAtFromCamera( antzData );

		//antzData->io.gl.stereo = false;
		antzData->io.gl.pickPass = true;

		DrawSceneForEye( Eye::Left, false );

		glFinish();
		glReadBuffer( GL_BACK_LEFT );
		pickID = npPickPinStereoSingleEye( glX, glY, antzData );

		/*if ((pickID == 0) && IsInStereoMode()) {

		DrawSceneForEye(Eye::Right, false);
		glFinish();
		glReadBuffer(GL_BACK_RIGHT);
		pickID = npPickPinStereoSingleEye(glX, glY, antzData);
		}*/

		//antzData->io.gl.stereo = true;
		//antzData->io.gl.stereo = IsInStereoMode();
		antzData->io.gl.pickPass = false;	//clear the flag to draw normal colors
		//}
		//else {

		//	pickID = npPickPin(glX, glY, antzData);
		//}

		if ( pickID != 0 ) {

			//pNPnode node = static_cast<pNPnode>(antzData->map.nodeID[pickID]);
			QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect;
			SynGlyphX::ItemFocusSelectionModel::FocusFlags focusFlags = SynGlyphX::ItemFocusSelectionModel::FocusFlag::ClearAndFocus;
			if ( multiSelect ) {

				flags = QItemSelectionModel::Toggle;
				focusFlags = SynGlyphX::ItemFocusSelectionModel::FocusFlag::Toggle;
			}

			QModelIndex modelIndex = m_model->IndexFromANTzID( pickID );
            if (modelIndex.isValid())
            {
                m_selectionModel->select( modelIndex, flags );
                m_selectionModel->SetFocus( modelIndex, focusFlags );
            }
		}

		//emit NewStatusMessage(tr("Selection At: %1, %2 || Pick ID: %3").arg(x).arg(y).arg(pickID), 4000);

		return ( pickID != 0 );
	}

	void ANTzForestWidget::mouseReleaseEvent( QMouseEvent* event ) {

		//Reset ANTz mouse values back to the original values
		pData antzData = m_antzData->GetData();
		antzData->io.mouse.mode = kNPmouseModeNull;
		antzData->io.mouse.tool = kNPtoolNull;
		antzData->io.mouse.buttonR = false;

		if ( !m_regionSelectionRect.isNull() ) {

			QRect glRect( m_regionSelectionRect.x(), height() - m_regionSelectionRect.y() - m_regionSelectionRect.height(), m_regionSelectionRect.width(), m_regionSelectionRect.height() );
			QItemSelection indexesInRegion;
			m_model->FindIndexesInRegion( glRect, indexesInRegion );
			m_selectionModel->select( indexesInRegion, QItemSelectionModel::Select );
			m_selectionModel->SetFocus( indexesInRegion.indexes(), SynGlyphX::ItemFocusSelectionModel::FocusFlag::ClearAndFocus );
			m_regionSelectionRect = QRect();
		}

		m_lastMousePosition = boost::none;
	}

	void ANTzForestWidget::mouseMoveEvent( QMouseEvent* event ) {

		pData antzData = m_antzData->GetData();
		antzData->io.mouse.tool = kNPtoolNull;
		antzData->io.mouse.mode = kNPmouseModeNull;

		if ( event->modifiers() & Qt::ShiftModifier ) {

			int x = ( m_lastMousePosition.get().x() < event->x() ) ? m_lastMousePosition.get().x() : event->x();
			int y = ( m_lastMousePosition.get().y() < event->y() ) ? m_lastMousePosition.get().y() : event->y();
			m_regionSelectionRect.setRect( x, y, std::abs( m_lastMousePosition.get().x() - event->x() ), std::abs( m_lastMousePosition.get().y() - event->y() ) );
		}
		else if ( m_lastMousePosition ) {

			if ( m_regionSelectionRect.isNull() ) {

				antzData->io.mouse.previous.x = m_lastMousePosition.get().x();
				antzData->io.mouse.previous.y = m_lastMousePosition.get().y();
				antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.get().x();
				antzData->io.mouse.delta.y = event->y() - m_lastMousePosition.get().y();
				antzData->io.mouse.x = event->x();
				antzData->io.mouse.y = event->y();

				if ( m_selectionModel->GetFocusList().empty() ) {

					if ( event->buttons() & Qt::LeftButton ) {

						antzData->io.mouse.mode = kNPmouseModeCamLook;
					}
				}
				else {

					if ( event->buttons() & Qt::LeftButton ) {

						if ( event->buttons() & Qt::RightButton ) {

							antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
						}
						else {

							antzData->io.mouse.mode = kNPmouseModeCamExamXY;
						}
					}
					else if ( event->buttons() & Qt::MidButton ) {

						antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
					}
				}
			}
			else {

				m_regionSelectionRect = QRect();
			}
			m_lastMousePosition = event->pos();
		}
	}

	void ANTzForestWidget::keyPressEvent( QKeyEvent* event ) {

		//if (m_selectionModel->selectedIndexes().empty()) {

		pData antzData = m_antzData->GetData();
		if ( ( event->key() == 'w' ) ||
			( event->key() == 'W' ) ||
			( event->key() == 's' ) ||
			( event->key() == 'S' ) ||
			( event->key() == 'a' ) ||
			( event->key() == 'A' ) ||
			( event->key() == 'd' ) ||
			( event->key() == 'D' ) ||
			( event->key() == 'e' ) ||
			( event->key() == 'E' ) ||
			( event->key() == 'q' ) ||
			( event->key() == 'Q' ) ) {

			npCtrlCommand( antzData->io.key.map[kKeyDown][npKeyRAWASCII( event->key() )], antzData );
			return;
		}
		//}

		QWidget::keyPressEvent( event );
	}

	void ANTzForestWidget::keyReleaseEvent( QKeyEvent* event ) {

		//if (m_selectionModel->selectedIndexes().empty()) {

		pData antzData = m_antzData->GetData();
		if ( ( event->key() == 'w' ) ||
			( event->key() == 'W' ) ||
			( event->key() == 's' ) ||
			( event->key() == 'S' ) ||
			( event->key() == 'a' ) ||
			( event->key() == 'A' ) ||
			( event->key() == 'd' ) ||
			( event->key() == 'D' ) ||
			( event->key() == 'e' ) ||
			( event->key() == 'E' ) ||
			( event->key() == 'q' ) ||
			( event->key() == 'Q' ) ) {

			npCtrlCommand( antzData->io.key.map[kKeyUp][npKeyRAWASCII( event->key() )], antzData );
			return;
		}
		//}

		/*if ((event->key() == 't') || (event->key() == 'T')) {

		SelectFromStylus(SynGlyphXANTz::ANTzBoundingBox::Line());
		return;
		}*/

		QWidget::keyReleaseEvent( event );
	}

	void ANTzForestWidget::wheelEvent( QWheelEvent* event ) {

		if ( !m_selectionModel->GetFocusList().empty() ) {

			pData antzData = m_antzData->GetData();
			antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
			//antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
			antzData->io.mouse.delta.y = event->delta() / 10;
			event->accept();
		}
		else {

			event->ignore();
		}
	}
	/*
	void ANTzForestWidget::SetStereo(bool enableStereo) {

	pData antzData = m_antzData->GetData();
	antzData->io.gl.stereo = enableStereo;

	if (enableStereo) {

	setFormat(s_stereoFormat);
	if (IsZSpaceAvailable()) {

	SetZSpacePosition();
	ResizeZSpaceViewport();
	}
	}
	else {

	setFormat(s_format);
	}
	}*/

	bool ANTzForestWidget::IsStereoSupported() const {

		return format().stereo();
	}

	bool ANTzForestWidget::IsInStereoMode() const {

		//pData antzData = m_antzData->GetData();
		//return (antzData->io.gl.stereo);

		return m_isInStereo;
	}



	void ANTzForestWidget::moveEvent( QMoveEvent* event ) {
#ifdef USE_ZSPACE
		SetZSpacePosition();
#endif
		QWidget::moveEvent( event );
	}

#ifdef USE_ZSPACE
	void ANTzForestWidget::SetZSpacePosition() {

		if ( IsInZSpaceStereo() ) {

			QPoint zSpaceViewportPosition = mapToGlobal( QPoint( 0, 0 ) );
			zsSetViewportPosition( m_zSpaceViewport, zSpaceViewportPosition.x(), zSpaceViewportPosition.y() );

			//ZSError error = zsUpdate(m_zSpaceContext);
		}
	}

	void ANTzForestWidget::ResizeZSpaceViewport() {

		if ( IsInZSpaceStereo() ) {

			zsSetViewportSize( m_zSpaceViewport, size().width(), size().height() );
			glGetFloatv( GL_PROJECTION_MATRIX, m_originialProjectionMatrix.f );

			//ZSError error = zsUpdate(m_zSpaceContext);
		}
	}
#endif

	bool ANTzForestWidget::eventFilter( QObject *object, QEvent *event ) {

#ifdef USE_ZSPACE
		if ( ( object == m_topLevelWindow ) && ( event->type() == QEvent::Move ) ) {

			SetZSpacePosition();
		}
#endif

		return false;
	}

#ifdef USE_ZSPACE
	bool ANTzForestWidget::IsInZSpaceStereo() const {

		return ( IsInStereoMode() && IsZSpaceAvailable() );
	}

	bool ANTzForestWidget::IsZSpaceAvailable() const {

		return ( m_zSpaceContext != nullptr );
	}
#endif

	void ANTzForestWidget::SelectFromStylus( const SynGlyphXANTz::ANTzBoundingBox::Line& line ) {

		Qt::KeyboardModifiers keyboardModifiers = QGuiApplication::queryKeyboardModifiers();

		std::map<float, int> distanceIdMap;
		pData antzData = m_antzData->GetData();
		for ( int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i ) {

			pNPnode node = static_cast<pNPnode>( antzData->map.node[i] );
			CheckStylusIntersectionWithNode( node, line, distanceIdMap );
		}

		if ( distanceIdMap.empty() ) {
#ifdef USE_ZSPACE
			zsStartTargetVibration( m_zSpaceStylus, 0.04f, 0.04f, 4 );
#endif
		}
		else {

			if ( ( keyboardModifiers.testFlag( Qt::ControlModifier ) ) || ( keyboardModifiers.testFlag( Qt::ShiftModifier ) ) ) {

				QItemSelection stylusItems;
				for ( auto id : distanceIdMap ) {

					QModelIndex index = m_model->IndexFromANTzID( id.second );
					QItemSelection newStylusItem( index, index );
					stylusItems.merge( newStylusItem, QItemSelectionModel::Select );
				}
				m_selectionModel->select( stylusItems, QItemSelectionModel::Select );
			}
			else {

				QModelIndex index = m_model->IndexFromANTzID( distanceIdMap.begin()->second );
				m_selectionModel->select( index, QItemSelectionModel::ClearAndSelect );
			}
		}
	}

	void ANTzForestWidget::CheckStylusIntersectionWithNode( pNPnode node, const SynGlyphXANTz::ANTzBoundingBox::Line& line, std::map<float, int>& distanceIdMap ) {

		//Only test on visible objects
		if ( ( !node->hide ) && ( node->screen.z >= 0.0f ) && ( node->screen.z <= 1.0f ) ) {

			SynGlyphXANTz::ANTzBoundingBox::Intersection intersection = m_boundingBoxes[node->id].DoesLineIntersect( line );
			if ( intersection.is_initialized() ) {

				distanceIdMap[intersection.get()] = node->id;
			}
		}
		for ( int j = 0; j < node->childCount; ++j ) {

			CheckStylusIntersectionWithNode( node->child[j], line, distanceIdMap );
		}
	}

#ifdef USE_ZSPACE
	void ANTzForestWidget::ZSpaceButtonPressHandler( ZSHandle targetHandle, const ZSTrackerEventData* eventData ) {

		if ( ( eventData->buttonId == 1 ) || ( eventData->buttonId == 2 ) ) {

			m_zSpaceStylusLastPosition.x = eventData->poseMatrix.m03;
			m_zSpaceStylusLastPosition.y = eventData->poseMatrix.m13;
			m_zSpaceStylusLastPosition.z = eventData->poseMatrix.m23;
		}
	}

	void ANTzForestWidget::ZSpaceButtonReleaseHandler( ZSHandle targetHandle, const ZSTrackerEventData* eventData ) {

		if ( eventData->buttonId == 0 ) {

			SelectFromStylus( m_stylusWorldLine );
		}
	}

	void ANTzForestWidget::ZSpaceStylusMoveHandler( ZSHandle targetHandle, const ZSTrackerEventData* eventData ) {

		ZSBool isButton2ButtonPressed = false;
		ZSBool isButton1ButtonPressed = false;
		ZSError error = zsIsTargetButtonPressed( targetHandle, 2, &isButton2ButtonPressed );
		error = zsIsTargetButtonPressed( targetHandle, 1, &isButton1ButtonPressed );
		if ( !isButton2ButtonPressed && !isButton1ButtonPressed ) {

			return;
		}

		pData antzData = m_antzData->GetData();
		antzData->io.mouse.delta.x = ( eventData->poseMatrix.m03 - m_zSpaceStylusLastPosition.x ) * 1000;

		bool isSelectionEmpty = m_selectionModel->selectedIndexes().empty();

		if ( isButton2ButtonPressed ) {

			if ( isSelectionEmpty ) {

				antzData->io.mouse.mode = kNPmouseModeCamLook;
				antzData->io.mouse.delta.y = -( eventData->poseMatrix.m13 - m_zSpaceStylusLastPosition.y ) * 1000;
			}
			else {

				antzData->io.mouse.mode = kNPmouseModeCamExamXY;
				antzData->io.mouse.delta.y = -( eventData->poseMatrix.m13 - m_zSpaceStylusLastPosition.y ) * 1000;
			}
		}
		else if ( isButton1ButtonPressed && !isSelectionEmpty ) {

			antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
			antzData->io.mouse.delta.y = -( eventData->poseMatrix.m23 - m_zSpaceStylusLastPosition.z ) * 1000;
		}

		m_zSpaceStylusLastPosition.x = eventData->poseMatrix.m03;
		m_zSpaceStylusLastPosition.y = eventData->poseMatrix.m13;
		m_zSpaceStylusLastPosition.z = eventData->poseMatrix.m23;
	}

	void ANTzForestWidget::ZSpaceStylusTapHandler( ZSHandle targetHandle, const ZSTrackerEventData* eventData ) {

		//emit NewStatusMessage(tr("Source: (%1, %2, %3)").arg(m_stylusWorldLine.first.x).arg(m_stylusWorldLine.first.y).arg(m_stylusWorldLine.first.z) +
		//	" " + tr("Dest: (%1, %2, %3)").arg(m_stylusWorldLine.second.x).arg(m_stylusWorldLine.second.y).arg(m_stylusWorldLine.second.z), 4000);

		//SelectFromStylus(m_stylusWorldTapLine);
	}
#endif

	void ANTzForestWidget::CreateBoundingBoxes() {

		m_boundingBoxes.clear();
		m_objectsThatNeedBoundingBoxUpdates.clear();

		pData antzData = m_antzData->GetData();
		pNPnode rootGrid = static_cast<pNPnode>( antzData->map.node[kNPnodeRootGrid] );
		for ( int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i ) {

			pNPnode node = static_cast<pNPnode>( antzData->map.node[i] );
			m_boundingBoxes[node->id] = SynGlyphXANTz::ANTzBoundingBox::CreateBoundingBox( node, rootGrid->scale.z );
			bool doesBoundingBoxNeedUpdating = IsNodeAnimated( node );
			if ( doesBoundingBoxNeedUpdating ) {

				m_objectsThatNeedBoundingBoxUpdates.insert( node->id );
			}

			for ( int j = 0; j < node->childCount; ++j ) {

				CreateBoundingBoxes( node->child[j], m_boundingBoxes[node->id].GetTransform(), doesBoundingBoxNeedUpdating );
			}
		}
	}

	void ANTzForestWidget::CreateBoundingBoxes( pNPnode node, const glm::mat4& parentTransform, bool isAncestorBoundingBoxBeingUpdated ) {

		bool doesBoundingBoxNeedUpdating = false;

		if ( !isAncestorBoundingBoxBeingUpdated ) {

			doesBoundingBoxNeedUpdating = IsNodeAnimated( node );
			if ( doesBoundingBoxNeedUpdating ) {

				m_objectsThatNeedBoundingBoxUpdates.insert( node->id );
			}
		}

		m_boundingBoxes[node->id] = SynGlyphXANTz::ANTzBoundingBox::CreateBoundingBox( node, parentTransform );
		for ( int j = 0; j < node->childCount; ++j ) {

			CreateBoundingBoxes( node->child[j], m_boundingBoxes[node->id].GetTransform(), doesBoundingBoxNeedUpdating );
		}
	}

	void ANTzForestWidget::DrawBoundingBoxes() {

		qglColor( Qt::magenta );

		for ( auto boundingBox : m_boundingBoxes ) {

			glm::vec3 minPoint = boundingBox.second.GetMinCorner();
			glm::vec3 maxPoint = boundingBox.second.GetMaxCorner();

			glm::vec4 bll = boundingBox.second.GetTransform() * glm::vec4( minPoint.x, minPoint.y, minPoint.z, 1.0f );
			glm::vec4 blr = boundingBox.second.GetTransform() * glm::vec4( maxPoint.x, minPoint.y, minPoint.z, 1.0f );

			glm::vec4 bur = boundingBox.second.GetTransform() * glm::vec4( maxPoint.x, maxPoint.y, minPoint.z, 1.0f );
			glm::vec4 bul = boundingBox.second.GetTransform() * glm::vec4( minPoint.x, maxPoint.y, minPoint.z, 1.0f );

			glm::vec4 tll = boundingBox.second.GetTransform() * glm::vec4( minPoint.x, minPoint.y, maxPoint.z, 1.0f );
			glm::vec4 tlr = boundingBox.second.GetTransform() * glm::vec4( maxPoint.x, minPoint.y, maxPoint.z, 1.0f );

			glm::vec4 tur = boundingBox.second.GetTransform() * glm::vec4( maxPoint.x, maxPoint.y, maxPoint.z, 1.0f );
			glm::vec4 tul = boundingBox.second.GetTransform() * glm::vec4( minPoint.x, maxPoint.y, maxPoint.z, 1.0f );

			glBegin( GL_LINE_LOOP );
			glVertex3f( bll.x, bll.y, bll.z );
			glVertex3f( blr.x, blr.y, blr.z );
			glVertex3f( bur.x, bur.y, bur.z );
			glVertex3f( bul.x, bul.y, bul.z );
			glEnd();

			glBegin( GL_LINE_LOOP );
			glVertex3f( tll.x, tll.y, tll.z );
			glVertex3f( tlr.x, tlr.y, tlr.z );
			glVertex3f( tur.x, tur.y, tur.z );
			glVertex3f( tul.x, tul.y, tul.z );
			glEnd();

			glBegin( GL_LINE_LOOP );
			glVertex3f( bll.x, bll.y, bll.z );
			glVertex3f( tll.x, tll.y, tll.z );
			glVertex3f( tul.x, tul.y, tul.z );
			glVertex3f( bul.x, bul.y, bul.z );
			glEnd();

			glBegin( GL_LINE_LOOP );
			glVertex3f( blr.x, blr.y, blr.z );
			glVertex3f( tlr.x, tlr.y, tlr.z );
			glVertex3f( tur.x, tur.y, tur.z );
			glVertex3f( bur.x, bur.y, bur.z );
			glEnd();
		}
	}

	void ANTzForestWidget::OnModelReset() {
        
		CreateBoundingBoxes();
		StoreRotationRates();
		ClearAllTags();

		m_filteredResults.clear();

		const QStringList& textures = m_model->GetBaseImageFilenames();

		for ( auto texture : m_textureIDs ) {

			delete texture;
		}
		m_textureIDs.clear();

		Q_FOREACH( const QString& texture, textures ) {

			m_textureIDs.push_back( BindTextureInFile( texture ) );
		}

		pData antzData = m_antzData->GetData();
		antzData->io.gl.textureCount = m_textureIDs.size() + 1;

		pNPnode rootGrid = static_cast<pNPnode>( antzData->map.node[kNPnodeRootGrid] );
		SetGridTexture( rootGrid );
		if ( m_model->rowCount() == 0 ) {

			SetGridLinesColor( rootGrid, Qt::blue );
			rootGrid->segments.x = 12;
			rootGrid->segments.y = 6;
			rootGrid->auxA.x = 30;
			rootGrid->auxA.y = 30;
			rootGrid->auxA.z = 30;
		}
		for ( int i = 0; i < rootGrid->childCount; ++i ) {

			SetGridTexture( rootGrid->child[i] );
		}

		if ( m_model->rowCount() > 0 ) {

			glm::vec3 maxPoint(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
			glm::vec3 minPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
			
			for ( unsigned int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i ) {

				pNPnode rootNode = static_cast<pNPnode>( antzData->map.node[i] );
				maxPoint[0] = std::max(maxPoint[2], rootNode->translate.x);
				maxPoint[1] = std::max(maxPoint[2], rootNode->translate.y);
				maxPoint[2] = std::max(maxPoint[2], rootNode->translate.z);

				minPoint[0] = std::min(minPoint[2], rootNode->translate.x);
				minPoint[1] = std::min(minPoint[2], rootNode->translate.y);
				minPoint[2] = std::min(minPoint[2], rootNode->translate.z);
			}
			m_initialCameraZAngle = 90.0f - (boost::math::float_constants::radian * std::atan2(345.0f - (0.8f * maxPoint[2]), 345.0f));

			maxPoint[0] += 5.0;
			maxPoint[1] += 5.0;
			maxPoint[2] += 5.0;

			minPoint[0] -= 5.0;
			minPoint[1] -= 5.0;
			minPoint[2] = std::min(minPoint[2], 0.0f);

			m_sceneAxisBoundingBox = ANTzBoundingBox(minPoint, maxPoint);
		}
		else {

			m_initialCameraZAngle = 45.0f;
			m_sceneAxisBoundingBox = ANTzBoundingBox(glm::vec3(-185.0f, -95.0f, 0.0f), glm::vec3(185.0f, 95.0f, 0.0f));
		}

		m_isReseting = false;
		update();

		ResetCamera();
	}

	void ANTzForestWidget::SetGridTexture( pNPnode grid ) {

		if ( grid->textureID == 1 ) {

			grid->textureID = m_worldTextureID->textureId();
		}
		else {

			grid->textureID = m_textureIDs[grid->textureID - 2]->textureId();
		}
	}

	void ANTzForestWidget::SetGridLinesColor( pNPnode grid, const QColor& color ) {

		grid->color.r = color.red();
		grid->color.g = color.green();
		grid->color.b = color.blue();
		grid->color.a = 255;
	}

	QOpenGLTexture* ANTzForestWidget::BindTextureInFile( const QString& imageFilename ) {

		QImage image( imageFilename );
		return new QOpenGLTexture( image.mirrored() );
	}

	bool ANTzForestWidget::SetStereoMode( bool stereoOn ) {

		if ( m_isInStereo != stereoOn ) {

			if ( stereoOn && ( !IsStereoSupported() ) ) {

				return false;
			}

			m_isInStereo = stereoOn;
		}

		return true;
	}

	void ANTzForestWidget::StoreRotationRates() {

		m_rotationRates.clear();
		pData antzData = m_antzData->GetData();

		for ( int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i ) {

			StoreRotationRates( static_cast<pNPnode>( antzData->map.node[i] ) );
		}
	}

	void ANTzForestWidget::StoreRotationRates( pNPnode node ) {

		if ( IsNodeAnimated( node ) ) {

			m_rotationRates[node->id] = node->rotateRate;
		}

		for ( int i = 0; i < node->childCount; ++i ) {

			StoreRotationRates( node->child[i] );
		}
	}

	void ANTzForestWidget::ShowAnimatedRotations( bool show ) {

		if ( show != m_showAnimation ) {

			pData antzData = m_antzData->GetData();
			m_showAnimation = show;
			if ( m_showAnimation ) {

				for ( auto idRatePair : m_rotationRates ) {

					pNPnode node = static_cast<pNPnode>( antzData->map.nodeID[idRatePair.first] );
					node->rotateRate.x = idRatePair.second.x;
					node->rotateRate.y = idRatePair.second.y;
					node->rotateRate.z = idRatePair.second.z;
				}
			}
			else {

				for ( auto idRatePair : m_rotationRates ) {

					pNPnode node = static_cast<pNPnode>( antzData->map.nodeID[idRatePair.first] );
					node->rotateRate.x = 0.0f;
					node->rotateRate.y = 0.0f;
					node->rotateRate.z = 0.0f;
				}
			}
		}
	}

	bool ANTzForestWidget::IsNodeAnimated( pNPnode node ) {

		return ( ( node->rotateRate.x != 0.0f ) || ( node->rotateRate.y != 0.0f ) || ( node->rotateRate.z != 0.0f ) );
	}

	void ANTzForestWidget::UpdateBoundingBoxes() {

		pData antzData = m_antzData->GetData();
		pNPnode rootGrid = static_cast<pNPnode>( antzData->map.node[kNPnodeRootGrid] );
		for ( int id : m_objectsThatNeedBoundingBoxUpdates ) {

			pNPnode node = static_cast<pNPnode>( antzData->map.nodeID[id] );
			if ( node->parent == nullptr ) {

				m_boundingBoxes[node->id].SetTransform( SynGlyphXANTz::ANTzBoundingBox::CreateTransform( node, rootGrid->scale.z ) );
				for ( int i = 0; i < node->childCount; ++i ) {

					UpdateBoundingBoxes( node->child[i], m_boundingBoxes[node->id].GetTransform() );
				}
			}
			else {

				UpdateBoundingBoxes( node, m_boundingBoxes[node->parent->id].GetTransform() );
			}
		}
	}

	void ANTzForestWidget::UpdateBoundingBoxes( pNPnode node, const glm::mat4& parentTransform ) {

		m_boundingBoxes[node->id].SetTransform( SynGlyphXANTz::ANTzBoundingBox::CreateTransform( node, parentTransform ) );
		for ( int i = 0; i < node->childCount; ++i ) {

			UpdateBoundingBoxes( node->child[i], m_boundingBoxes[node->id].GetTransform() );
		}
	}

#ifdef USE_ZSPACE
	void ANTzForestWidget::SetZSpaceOptions( const SynGlyphX::ZSpaceOptions& options ) {

		m_zSpaceOptions = options;
	}

	const SynGlyphX::ZSpaceOptions& ANTzForestWidget::GetZSpaceOptions() const {

		return m_zSpaceOptions;
	}
#endif

	ANTzForestWidget::FilteredResultsDisplayMode ANTzForestWidget::GetFilteredResultsDisplayMode() const {

		return m_filteredResultsDisplayMode;
	}

	void ANTzForestWidget::SetBackgroundColor( const SynGlyphX::GlyphColor& color ) {

		pData antzData = m_antzData->GetData();
		antzData->io.clear.r = color[0] / 255.0f;
		antzData->io.clear.g = color[1] / 255.0f;
		antzData->io.clear.b = color[2] / 255.0f;
	}

	void ANTzForestWidget::DrawLogo() {

		bool isLightingEnabled = glIsEnabled( GL_LIGHTING );
		bool isDepthTestEnabled = glIsEnabled( GL_DEPTH_TEST );

		glDisable( GL_LIGHTING );
		glDisable( GL_DEPTH_TEST );
        glEnable( GL_BLEND );

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();

		gluOrtho2D( 0, width(), 0.0, height() );

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glLoadIdentity();

		glColor3f( 1.0f, 1.0f, 1.0f );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_logoTextureID->textureId() );
		glBegin( GL_QUADS );
		glTexCoord2f( 0, 0 );
		glVertex2i( m_logoPosition.left(), m_logoPosition.bottom() );
		glTexCoord2f( 1, 0 );
		glVertex2i( m_logoPosition.right(), m_logoPosition.bottom() );

		glTexCoord2f( 1, 1 );
		glVertex2i( m_logoPosition.right(), m_logoPosition.top() );
		glTexCoord2f( 0, 1 );
		glVertex2i( m_logoPosition.left(), m_logoPosition.top() );
		glEnd();

		glDisable( GL_TEXTURE_2D );

		glMatrixMode( GL_PROJECTION );
		glPopMatrix();

		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();

		if ( isLightingEnabled ) {

			glEnable( GL_DEPTH_TEST );
		}

		if ( isDepthTestEnabled ) {

			glEnable( GL_LIGHTING );
		}
        
        glDisable( GL_BLEND );
	}

	void ANTzForestWidget::SetShowTagsOfSelectedObjects( bool showTagsOfSelectedObjects ) {

		if ( showTagsOfSelectedObjects ) {

			Q_FOREACH( const QModelIndex& modelIndex, m_selectionModel->selectedIndexes() ) {

				m_tagIndexes.insert( modelIndex );
			}
		}
		else {

			Q_FOREACH( const QModelIndex& modelIndex, m_selectionModel->selectedIndexes() ) {

				if ( m_tagIndexes.contains( modelIndex ) ) {

					m_tagIndexes.remove( modelIndex );
				}
			}
		}
	}

	void ANTzForestWidget::ClearAllTags() {

		m_tagIndexes.clear();
	}

	void ANTzForestWidget::SetAxisInfoObjectLocation(HUDLocation location){

		pData antzData = m_antzData->GetData();

		m_hudAxisInfoObjectLocation = location;

		double orthoWidth = antzData->io.gl.width / 15.0;
		if (m_hudAxisInfoObjectLocation == HUDLocation::TopLeft) {

			m_hudAxisInfoViewport = QRect(0, antzData->io.gl.height - 160, antzData->io.gl.width, 150);
			m_hudAxisInfoOrtho = QRectF(-5.0, -5.0, orthoWidth, 10.0);
		}
		else if (m_hudAxisInfoObjectLocation == HUDLocation::BottomLeft) {

			m_hudAxisInfoViewport = QRect(0, 10, antzData->io.gl.width, 150);
			m_hudAxisInfoOrtho = QRectF(-5.0, -5.0, orthoWidth, 10.0);
		}
		else if (m_hudAxisInfoObjectLocation == HUDLocation::BottomRight) {

			m_hudAxisInfoViewport = QRect(0, 10, antzData->io.gl.width, 150);
			m_hudAxisInfoOrtho = QRectF(5.0 - orthoWidth, -5.0, orthoWidth, 10.0);
		}
	}

	ANTzForestWidget::HUDLocation ANTzForestWidget::GetAxisInfoObjectLocation() const {

		return m_hudAxisInfoObjectLocation;
	}

	void ANTzForestWidget::SetShowHUDAxisInfoObject(bool show) {

		m_showHUDAxisInfoObject = show;
	}

	bool ANTzForestWidget::GetShowHUDAxisInfoObject() const {

		return m_showHUDAxisInfoObject;
	}

	void ANTzForestWidget::showEvent(QShowEvent* event) {

		QOpenGLWidget::showEvent(event);

		if (!m_isStereoSetup) {

			m_isStereoSetup = true;

			m_isInStereo = IsStereoSupported();
			emit StereoSetup(m_isInStereo);
			if (IsStereoSupported()) {

#ifdef USE_ZSPACE
				//m_antzData->GetData()->io.gl.stereo = true;
				try {

					ZSError error = zsInitialize(&m_zSpaceContext);

					if (error == ZS_ERROR_RUNTIME_NOT_FOUND) { //If zSpace runtime not found, then don't do anything

						ClearZSpaceContext();
						return;
					}
					CheckZSpaceError(error);

					// Find the zSpace display
					error = zsFindDisplayByType(m_zSpaceContext, ZS_DISPLAY_TYPE_ZSPACE, 0, &m_zSpaceDisplay);

					if (error == ZS_ERROR_DISPLAY_NOT_FOUND) { //If zSpace display not found, then don't do anything

						ClearZSpaceContext();
						return;
					}
					CheckZSpaceError(error);

					error = zsCreateStereoBuffer(m_zSpaceContext, ZS_RENDERER_QUAD_BUFFER_GL, 0, &m_zSpaceBuffer);
					CheckZSpaceError(error);

					error = zsSetStereoBufferFullScreen(m_zSpaceBuffer, false);
					CheckZSpaceError(error);

					error = zsCreateViewport(m_zSpaceContext, &m_zSpaceViewport);
					CheckZSpaceError(error);

					error = zsFindFrustum(m_zSpaceViewport, &m_zSpaceFrustum);
					CheckZSpaceError(error);

					// Grab a handle to the stylus target.
					error = zsFindTargetByType(m_zSpaceContext, ZS_TARGET_TYPE_PRIMARY, 0, &m_zSpaceStylus);
					CheckZSpaceError(error);

					//Get the top level window so that we can track its movements for zSpace viewport
					m_topLevelWindow = parentWidget();
					while (!m_topLevelWindow->isWindow()) {

						m_topLevelWindow = m_topLevelWindow->parentWidget();
					}
					m_topLevelWindow->installEventFilter(this);

					error = zsSetMouseEmulationEnabled(m_zSpaceContext, false);
					CheckZSpaceError(error);
					/*
					error = zsSetMouseEmulationTarget(m_zSpaceContext, m_zSpaceStylus);
					CheckZSpaceError(error);

					error = zsSetMouseEmulationButtonMapping(m_zSpaceContext, 0, ZS_MOUSE_BUTTON_LEFT);
					CheckZSpaceError(error);

					error = zsSetMouseEmulationButtonMapping(m_zSpaceContext, 1, ZS_MOUSE_BUTTON_RIGHT);
					CheckZSpaceError(error);*/

					error = zsSetTrackingEnabled(m_zSpaceContext, true);
					CheckZSpaceError(error);

					error = zsSetTargetVibrationEnabled(m_zSpaceStylus, true);
					CheckZSpaceError(error);

					ConnectZSpaceTrackers();
				}
				catch (const std::exception& e) {

					QMessageBox::critical(nullptr, tr("Startup error"), tr("Error: ") + e.what(), QMessageBox::Ok);
					throw;
				}
				catch (...) {

					QMessageBox::critical(nullptr, tr("Startup error"), tr("Error: Unknown"), QMessageBox::Ok);
					throw;
				}
#endif
			}
		}
	}

} //namespace SynGlyphXANTz
