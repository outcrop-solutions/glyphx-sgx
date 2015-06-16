#include "antzforestwidget.h"
#include <QtGUI/QOpenGLContext>
#include <QtGUI/QMouseEvent>
#include <QtGUI/QFont>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include "io/npfile.h"
#include "io/npch.h"
#include "npctrl.h"
#include "npui.h"
#include "ctrl/npengine.h"
#include "npctrl.h"
#include "io/npgl.h"
#include "io/gl/nptags.h"
#include "application.h"
#include "itemfocusselectionmodel.h"
#include <stack>

namespace SynGlyphXANTz {

	//The default QGLFormat works for now except we want alpha enabled.  Also want to try and get a stereo enabled context
	QGLFormat ANTzForestWidget::s_format(QGL::AlphaChannel);
	QGLFormat ANTzForestWidget::s_stereoFormat(QGL::AlphaChannel | QGL::StereoBuffers);

	ANTzForestWidget::ANTzForestWidget(const QGLFormat& format, GlyphForestModel* model, SynGlyphX::ItemFocusSelectionModel* selectionModel, QWidget *parent)
		: QGLWidget(format, parent),
		m_model(model),
		m_selectionModel(selectionModel),
		m_antzData(model->GetANTzData()),
		m_zSpaceContext(nullptr),
		m_zSpaceDisplay(nullptr),
		m_zSpaceBuffer(nullptr),
		m_zSpaceViewport(nullptr),
		m_zSpaceFrustum(nullptr),
		m_zSpaceStylus(nullptr),
		m_topLevelWindow(nullptr),
		m_oglTextFont("Arial", 12, QFont::Normal),
		m_isReseting(false),
		m_worldTextureID(0),
		m_regionSelectionRect(QRect()),
		m_hideUnselectedGlyphTrees(false),
		m_drawHUD(true),
		m_zSpaceOptions(),
		m_showAnimation(true)
	{
		setAutoBufferSwap(false);
		setFocusPolicy(Qt::StrongFocus);

		QFont newFont = font();
		newFont.setPointSize(12);
		setFont(newFont);

		InitIO();

		//Change fly speeds
		m_antzData->GetData()->ctrl.slow = 0.5f;
		m_antzData->GetData()->ctrl.fast = 0.75f;

		if (m_selectionModel != nullptr) {
			QObject::connect(m_selectionModel, &SynGlyphX::ItemFocusSelectionModel::selectionChanged, this, &ANTzForestWidget::OnSelectionUpdated);
			QObject::connect(m_selectionModel, &SynGlyphX::ItemFocusSelectionModel::FocusChanged, this, &ANTzForestWidget::OnFocusChanged);
		}

		QObject::connect(m_model, &GlyphForestModel::modelReset, this, &ANTzForestWidget::OnModelReset);
		QObject::connect(m_model, &GlyphForestModel::modelAboutToBeReset, this, [this]{ m_isReseting = true; });

		if (IsInStereoMode()) {

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
			catch(...) {

				QMessageBox::critical(nullptr, tr("Startup error"), tr("Error: Unknown"), QMessageBox::Ok);
				throw;
			}
		}

		setFocus();
	}

	ANTzForestWidget::~ANTzForestWidget()
	{
		if (m_worldTextureID != 0) {

			deleteTexture(m_worldTextureID);
			m_worldTextureID = 0;
		}

		ClearZSpaceContext();
		npCloseGL(m_antzData->GetData());
	}

	void ANTzForestWidget::ClearZSpaceContext() {

		if (m_zSpaceContext != nullptr) {

			zsShutdown(m_zSpaceContext);
		}
	}

	void ANTzForestWidget::ConnectZSpaceTrackers() {

		ZSError error = zsAddTrackerEventHandler(m_zSpaceStylus, ZS_TRACKER_EVENT_BUTTON_PRESS, &SynGlyphX::ZSpaceEventDispatcher::Dispatch, &m_zSpaceEventDispatcher);
		CheckZSpaceError(error);
		QObject::connect(&m_zSpaceEventDispatcher, &SynGlyphX::ZSpaceEventDispatcher::ZSpaceButtonPressed, this, &ANTzForestWidget::ZSpaceButtonPressHandler, Qt::BlockingQueuedConnection);
		//QObject::connect(&m_zSpaceEventDispatcher, &ZSpaceEventDispatcher::ZSpaceButtonPressed, this, &ANTzForestWidget::ZSpaceButtonReleaseHandler, Qt::BlockingQueuedConnection);

		error = zsAddTrackerEventHandler(m_zSpaceStylus, ZS_TRACKER_EVENT_BUTTON_RELEASE, &SynGlyphX::ZSpaceEventDispatcher::Dispatch, &m_zSpaceEventDispatcher);
		CheckZSpaceError(error);
		QObject::connect(&m_zSpaceEventDispatcher, &SynGlyphX::ZSpaceEventDispatcher::ZSpaceButtonReleased, this, &ANTzForestWidget::ZSpaceButtonReleaseHandler, Qt::BlockingQueuedConnection);

		error = zsAddTrackerEventHandler(m_zSpaceStylus, ZS_TRACKER_EVENT_MOVE, &SynGlyphX::ZSpaceEventDispatcher::Dispatch, &m_zSpaceEventDispatcher);
		CheckZSpaceError(error);
		QObject::connect(&m_zSpaceEventDispatcher, &SynGlyphX::ZSpaceEventDispatcher::ZSpaceStylusMoved, this, &ANTzForestWidget::ZSpaceStylusMoveHandler, Qt::BlockingQueuedConnection);

		//error = zsAddTrackerEventHandler(m_zSpaceStylus, ZS_TRACKER_EVENT_TAP_SINGLE, &ZSpaceEventDispatcher, this);
		//CheckZSpaceError(error);
	}

	void ANTzForestWidget::CheckZSpaceError(ZSError error) {

		if (error != ZS_ERROR_OKAY) {

			QString zSpaceErrorString = tr("zSpace error in ");
			zSpaceErrorString += ":";
			if (error == ZS_ERROR_NOT_IMPLEMENTED) {

				zSpaceErrorString += "Not Implemented";
			}
			else if (error == ZS_ERROR_NOT_INITIALIZED) {

				zSpaceErrorString += "Not Initalized";
			}
			else if (error == ZS_ERROR_ALREADY_INITIALIZED) {

				zSpaceErrorString += "Already Initalized";
			}
			else if (error == ZS_ERROR_INVALID_PARAMETER) {

				zSpaceErrorString += "Invalid Parameter";
			}
			else if (error == ZS_ERROR_INVALID_CONTEXT) {

				zSpaceErrorString += "Invalid Context";
			}
			else if (error == ZS_ERROR_INVALID_HANDLE) {

				zSpaceErrorString += "Invalid Handle";
			}
			else if (error == ZS_ERROR_RUNTIME_INCOMPATIBLE) {

				zSpaceErrorString += "Incompatible Runtime";
			}
			else if (error == ZS_ERROR_RUNTIME_NOT_FOUND) {

				zSpaceErrorString += "Runtime Not Found";
			}
			else if (error == ZS_ERROR_SYMBOL_NOT_FOUND) {

				zSpaceErrorString += "Symbol Not Found";
			}
			else if (error == ZS_ERROR_DISPLAY_NOT_FOUND) {

				zSpaceErrorString += "Display Not Found";
			}
			else if (error == ZS_ERROR_DEVICE_NOT_FOUND) {

				zSpaceErrorString += "Device Not Found";
			}
			else if (error == ZS_ERROR_TARGET_NOT_FOUND) {

				zSpaceErrorString += "Target Not Found";
			}
			else if (error == ZS_ERROR_CAPABILITY_NOT_FOUND) {

				zSpaceErrorString += "Error Capability Not Found";
			}
			else if (error == ZS_ERROR_BUFFER_TOO_SMALL) {

				zSpaceErrorString += "Buffer too small";
			}
		
			throw std::exception(zSpaceErrorString.toStdString().c_str());
		}
	}

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

		data->io.mouse.size = sizeof(NPmouse);
	}

	void ANTzForestWidget::initializeGL() {
    
		pData antzData = m_antzData->GetData();
		npInitGLDraw(antzData);
		npInitGLPrimitive(antzData);
		npInitTags(antzData);
	
		m_worldTextureID = BindTextureInFile(QDir::toNativeSeparators(SynGlyphX::Application::applicationDirPath() + QDir::separator() + "world.png"));
		pNPnode rootGrid = static_cast<pNPnode>(antzData->map.node[kNPnodeRootGrid]);
		SetGridTexture(rootGrid);
		antzData->io.gl.textureCount = 1;
		SetCameraToDefaultPosition();
	}

	void ANTzForestWidget::resizeGL(int w, int h) {

		ResizeZSpaceViewport();

		npGLResizeScene(w, h);
	}

	void ANTzForestWidget::paintGL() {

		if (m_isReseting) {

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			return;
		}

		pData antzData = m_antzData->GetData();

		if (antzData->io.gl.pickPass) {

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			return;
		}

		npUpdateCh(antzData);

		npUpdateEngine(antzData);		//position, physics, interactions...
		UpdateBoundingBoxes();

		//We may need to have a selected pin node during update to position the camera, but we don't want it during drawing
		antzData->map.selectedPinNode = NULL;
		antzData->map.selectedPinIndex = 0;

		// zero out our mouse to prevent drifting objects
		antzData->io.mouse.delta.x = 0.0f;
		antzData->io.mouse.delta.y = 0.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		npSetLookAtFromCamera(antzData);

		if (IsInZSpaceStereo()) {

			ZSError error = zsUpdate(m_zSpaceContext);
			error = zsBeginStereoBufferFrame(m_zSpaceBuffer);
			glGetFloatv(GL_MODELVIEW_MATRIX, m_originialViewMatrix.f);
		}

		//DrawSceneForEye(Eye::Left, true);
		DrawSceneForEye(Eye::Right, true);
		if (IsInStereoMode())  {

			glDrawBuffer(GL_BACK_LEFT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//DrawSceneForEye(Eye::Right, false);
		}

		//int err = glGetError();
		//if (err) {
	   //     printf("err: 2388 - OpenGL error: %d\n", err);
	   // }

		if (doubleBuffer()) {

			swapBuffers();
		}
		else {

			glFinish();
		}

		//ANTz assumes that redraw constantly happens.  Need to put this in a thread
		update();
	}

	void ANTzForestWidget::DrawSceneForEye(Eye eye, bool getStylusWorldPosition) {

		pData antzData = m_antzData->GetData();
		pNPnode camNode = npGetActiveCam(antzData);
		NPcameraPtr camData = static_cast<NPcameraPtr>(camNode->data);

		ZSEye zSpaceEye;
		if (eye == Eye::Left) {

			glDrawBuffer(GL_BACK_LEFT);
			zSpaceEye = ZS_EYE_LEFT;
		}
		else {

			glDrawBuffer(GL_BACK_RIGHT);
			zSpaceEye = ZS_EYE_RIGHT;
		}

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		if (IsInZSpaceStereo()) {

			ZSTrackerPose stylusPose;
			ZSError error = zsGetTargetPose(m_zSpaceStylus, &stylusPose);

			// Transform the stylus pose from tracker to camera space.
			error = zsTransformMatrix(m_zSpaceViewport, ZS_COORDINATE_SPACE_TRACKER, ZS_COORDINATE_SPACE_CAMERA, &stylusPose.matrix);

			ZSMatrix4 displayToCameraMatrix;
			error = zsTransformMatrix(m_zSpaceViewport, ZS_COORDINATE_SPACE_DISPLAY, ZS_COORDINATE_SPACE_CAMERA, &displayToCameraMatrix);

			ZSMatrix4 originalCameraMatrix;
			npInvertMatrixf(m_originialViewMatrix.f, originalCameraMatrix.f);

			// Transform the stylus pose from camera space to world space.
			// This is done by multiplying the pose (camera space) by the 
			// application's camera matrix.
			npMultMatrix(m_zSpaceStylusWorldMatrix.f, originalCameraMatrix.f, stylusPose.matrix.f);
			npMultMatrix(m_zSpaceDisplayWorldMatrix.f, originalCameraMatrix.f, displayToCameraMatrix.f);

			SetZSpaceMatricesForDrawing(zSpaceEye, m_originialViewMatrix, camData);
		}
		else if (!IsInStereoMode()) {

			glGetFloatv(GL_MODELVIEW_MATRIX, camData->matrix);
			npInvertMatrixf(camData->matrix, camData->inverseMatrix);
		}

		glMatrixMode(GL_MODELVIEW);

		npGLLighting(antzData);
		npGLShading(antzData);
		npDrawNodes(antzData);

		//Leave this here so that bounding boxes can be shown for debugging
		//DrawBoundingBoxes();

		DrawZSpaceStylus(m_zSpaceStylusWorldMatrix, getStylusWorldPosition);

		if (m_drawHUD) {

			DrawHUD();
		}

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void ANTzForestWidget::DrawZSpaceStylus(const ZSMatrix4& stylusMatrix, bool getStylusWorldPosition) {

		if (IsInZSpaceStereo()) {

			pData antzData = m_antzData->GetData();
			pNPnode camNode = npGetActiveCam(antzData);
			NPcameraPtr camData = static_cast<NPcameraPtr>(camNode->data);

			qglColor(m_zSpaceOptions.GetStylusColor());

			// Multiply the model-view matrix by the stylus world matrix.
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMultMatrixf(stylusMatrix.f);

			// Draw the line.
			glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, -m_zSpaceOptions.GetStylusLength());

			glEnd();

			if (getStylusWorldPosition) {

				NPfloatXYZ world = { 0.0f, 0.0f, 0.0f };
				GLfloat modelView[16];
				glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
				npLocalToWorld(&world, camData->inverseMatrix, modelView);

				m_stylusWorldLine.first.x = world.x;
				m_stylusWorldLine.first.y = world.y;
				m_stylusWorldLine.first.z = world.z;

				//m_stylusWorldTapLine.first = m_stylusWorldLine.first;

				glTranslatef(0.0f, 0.0f, -m_zSpaceOptions.GetStylusLength());
				//glScalef(0.0025f, 0.0025f, 0.0025f);
				//npGLPrimitive(kNPgeoSphere, 0.1f);
				glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
				npLocalToWorld(&world, camData->inverseMatrix, modelView);

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

	void ANTzForestWidget::DrawHUD() {

		pData antzData = m_antzData->GetData();

		bool reenableDepthTest = glIsEnabled(GL_DEPTH_TEST);
		if (reenableDepthTest) {
			glDisable(GL_DEPTH_TEST);
		}

		//Draw region selection rectangle
		if ((m_regionSelectionRect.width() > 0) && (m_regionSelectionRect.height() > 0)) {

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(0, antzData->io.gl.width, 0, antzData->io.gl.height);
		
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glColor4ub(64, 32, 0, 100);

			int glRegionLeft = m_regionSelectionRect.x();
			int glRegionRight = m_regionSelectionRect.x() + m_regionSelectionRect.width();
			int glRegionTop = antzData->io.gl.height - m_regionSelectionRect.y();
			int glRegionBottom = glRegionTop - m_regionSelectionRect.height();

			glBegin(GL_QUADS);
			glVertex2i(glRegionLeft, glRegionTop);
			glVertex2f(glRegionLeft, glRegionBottom);
			glVertex2f(glRegionRight, glRegionBottom);
			glVertex2f(glRegionRight, glRegionTop);
			glEnd();

			glColor4ub(255, 255, 0, 100);

			glBegin(GL_LINE_LOOP);
			glVertex2i(glRegionLeft, glRegionTop);
			glVertex2f(glRegionLeft, glRegionBottom);
			glVertex2f(glRegionRight, glRegionBottom);
			glVertex2f(glRegionRight, glRegionTop);
			glEnd();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}

		//Draw tags
		qglColor(Qt::white);

		Q_FOREACH(const QModelIndex& modelIndex, m_selectionModel->selectedIndexes()) {

			pNPnode selectedNode = static_cast<pNPnode>(modelIndex.internalPointer());
			if (m_model->IsTagShownIn3d(selectedNode->tag->title)) {

				renderText(selectedNode->world.x, selectedNode->world.y, selectedNode->world.z, selectedNode->tag->title, m_oglTextFont);
			}
		}

		//Draw HUD
		QString positionHUD;
		
		if (!m_selectionModel->GetFocusList().empty()) {

			pNPnode lastFocusNode = static_cast<pNPnode>(m_selectionModel->GetFocusList().back().internalPointer());
			positionHUD = tr("Glyph Position: X: %1, Y: %2, Z: %3").arg(QString::number(lastFocusNode->world.x), QString::number(lastFocusNode->world.y), QString::number(lastFocusNode->world.z));
		}
		else {

			positionHUD = tr("Camera Position: X: %1, Y: %2, Z: %3").arg(QString::number(antzData->map.currentCam->translate.x), QString::number(antzData->map.currentCam->translate.y), QString::number(antzData->map.currentCam->translate.z));
		}

		QFontMetrics hudFontMetrics(m_oglTextFont);
		renderText((width() / 2) - (hudFontMetrics.width(positionHUD) / 2), height() - 16, positionHUD, m_oglTextFont);

		if (reenableDepthTest) {
			glEnable(GL_DEPTH_TEST);
		}
	}

	void ANTzForestWidget::SetZSpaceMatricesForDrawing(ZSEye eye, const ZSMatrix4& originialViewMatrix, NPcameraPtr camData) {

		ZSMatrix4 zSpaceEyeViewMatrix;
		ZSError mverror = zsGetFrustumViewMatrix(m_zSpaceFrustum, eye, &zSpaceEyeViewMatrix);

		glMatrixMode(GL_MODELVIEW);

		//Probably should be at bottom of this function
		//glGetFloatv(GL_MODELVIEW_MATRIX, camData->matrix);
		//npInvertMatrixf(camData->matrix, camData->inverseMatrix);

		glLoadMatrixf(zSpaceEyeViewMatrix.f);
		glMultMatrixf(originialViewMatrix.f);

		glMatrixMode(GL_PROJECTION);
		ZSMatrix4 projectionMatrix;
		ZSError projError = zsGetFrustumProjectionMatrix(m_zSpaceFrustum, eye, &projectionMatrix);

		glLoadMatrixf(projectionMatrix.f);
		//glMultMatrixf(m_originialProjectionMatrix.f);

		glGetFloatv(GL_MODELVIEW_MATRIX, camData->matrix);
		npInvertMatrixf(camData->matrix, camData->inverseMatrix);
	}

	void ANTzForestWidget::OnSelectionUpdated(const QItemSelection& selected, const QItemSelection& deselected) {

		UpdateGlyphTreesShowHideForSelection();

		pData antzData = m_antzData->GetData();

		//unselect all nodes that are no longer selected
		const QModelIndexList& deselectedIndicies = deselected.indexes();
		for (int i = 0; i < deselectedIndicies.length(); ++i) {
			if (deselectedIndicies[i].isValid()) {
				pNPnode node = static_cast<pNPnode>(deselectedIndicies[i].internalPointer());
				node->selected = false;
			}
		}

		//select all newly selected nodes
		const QModelIndexList& selectedIndicies = selected.indexes();
		for (int i = 0; i < selectedIndicies.length(); ++i) {
			if (selectedIndicies[i].isValid()) {
				pNPnode node = static_cast<pNPnode>(selectedIndicies[i].internalPointer());
				node->selected = true;
			}
		}
	}

	void ANTzForestWidget::OnFocusChanged(const QModelIndexList& indexes) {

		pData antzData = m_antzData->GetData();

		int nodeRootIndex = 0;
		if (!indexes.isEmpty())  {
			const QModelIndex& last = indexes.back();
			if (last.isValid()) {

				pNPnode node = static_cast<pNPnode>(last.internalPointer());
				CenterCameraOnNode(node);
				while (node->branchLevel != 0) {
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

	void ANTzForestWidget::UpdateGlyphTreesShowHideForSelection() {

		if (m_hideUnselectedGlyphTrees) {

			const QModelIndexList& currentSelection = m_selectionModel->selectedIndexes();
			if (currentSelection.empty()) {

				ShowAllGlyphTrees();
			}
			else {

				pData antzData = m_antzData->GetData();
				SynGlyphX::IndexSet selectedGlyphTrees = SynGlyphX::ItemFocusSelectionModel::GetRootRows(currentSelection);
				for (unsigned int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i) {

					pNPnode node = static_cast<pNPnode>(antzData->map.node[i]);
					node->hide = (selectedGlyphTrees.count(i - kNPnodeRootPin) == 0);
				}
			}
		}
	}

	void ANTzForestWidget::SetHideUnselectedGlyphTrees(bool hideUnselectedGlyphTrees) {

		if (hideUnselectedGlyphTrees != m_hideUnselectedGlyphTrees) {

			m_hideUnselectedGlyphTrees = hideUnselectedGlyphTrees;
			if (m_selectionModel != nullptr) {

				const QModelIndexList& currentSelection = m_selectionModel->selectedIndexes();
				if (m_hideUnselectedGlyphTrees) {

					if (!currentSelection.empty()) {

						UpdateGlyphTreesShowHideForSelection();
					}
				}
				else {

					if (!currentSelection.empty()) {

						ShowAllGlyphTrees();
					}
				}
			}
		}
	}

	void ANTzForestWidget::ShowAllGlyphTrees() {

		pData antzData = m_antzData->GetData();
		for (unsigned int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i) {
	
			pNPnode node = static_cast<pNPnode>(antzData->map.node[i]);
			node->hide = false;
		}
	}

	void ANTzForestWidget::SetCameraToDefaultPosition() {

		pData antzData = m_antzData->GetData();

		antzData->map.currentCam->proximity.x = 0;
		antzData->map.currentCam->translate.x = 0.0;
		antzData->map.currentCam->translate.y = -345.0;
		antzData->map.currentCam->translate.z = 345.0;
		antzData->map.currentCam->rotate.x = 45.0;
		antzData->map.currentCam->rotate.y = 0;
		antzData->map.currentCam->rotate.z = 0;

		pNPnode node = static_cast<pNPnode>(antzData->map.node[kNPnodeRootGrid]);
		antzData->io.mouse.targeting = false;
		antzData->map.selectedPinNode = node;
		antzData->map.selectedPinIndex = node->id;

		//Always keep current node set to current cam
		antzData->map.currentNode = antzData->map.currentCam;
	}

	void ANTzForestWidget::ResetCamera() {
    
		pData antzData = m_antzData->GetData();

		//We only want to center the camera when there are actual root nodes
		if (antzData->map.nodeRootCount > kNPnodeRootPin) {

			SetCameraToDefaultPosition();
			updateGL();
		}
		else {

			//If an object is selected make sure that camera doesn't lock into its position
			antzData->map.nodeRootIndex = 0;
			antzData->map.currentCam->proximity.x = 0;
		}
	}

	void ANTzForestWidget::CenterCameraOnNode(pNPnode node) {
	
		pData antzData = m_antzData->GetData();

		npSetCamTargetNode(node, antzData);
		antzData->io.mouse.targeting = false;
		antzData->map.selectedPinNode = node;
		antzData->map.selectedPinIndex = node->id;

		//Always keep current node set to current cam
		antzData->map.currentNode = antzData->map.currentCam;
	}

	void ANTzForestWidget::mousePressEvent(QMouseEvent* event) {

		if (event->button() == Qt::LeftButton) {
		
			Qt::KeyboardModifiers keys = event->modifiers();
			if (keys & Qt::ShiftModifier) {

				m_selectionModel->clearSelection();
				m_selectionModel->ClearFocus();
			}
			else {

				SelectAtPoint(event->x(), event->y(), keys & Qt::ControlModifier);
			}
		}

		m_lastMousePosition = event->pos();
	}

	bool ANTzForestWidget::SelectAtPoint(int x, int y, bool multiSelect) {
	
		if (!rect().contains(x, y)) {

			return false;
		}

		//emit NewStatusMessage(tr("Selection Attempt At: %1, %2").arg(x).arg(y), 4000);

		int pickID = 0;

		pData antzData = m_antzData->GetData();
	
		int glX = x;
		int glY = antzData->io.gl.height - y;

		//if (IsInStereoMode()) {

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		npSetLookAtFromCamera(antzData);

		//antzData->io.gl.stereo = false;
		antzData->io.gl.pickPass = true;

		DrawSceneForEye(Eye::Left, false);

		glFinish();
		glReadBuffer(GL_BACK_LEFT);
		pickID = npPickPinStereoSingleEye(glX, glY, antzData);

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

		if (pickID != 0) {

			//pNPnode node = static_cast<pNPnode>(antzData->map.nodeID[pickID]);
			QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect;
			SynGlyphX::ItemFocusSelectionModel::FocusFlags focusFlags = SynGlyphX::ItemFocusSelectionModel::FocusFlag::ClearAndFocus;
			if (multiSelect) {

				flags = QItemSelectionModel::Toggle;
				focusFlags = SynGlyphX::ItemFocusSelectionModel::FocusFlag::Toggle;
			}

			QModelIndex modelIndex = m_model->IndexFromANTzID(pickID);
			m_selectionModel->select(modelIndex, flags);
			m_selectionModel->SetFocus(modelIndex, focusFlags);
		}

		//emit NewStatusMessage(tr("Selection At: %1, %2 || Pick ID: %3").arg(x).arg(y).arg(pickID), 4000);
	
		return (pickID != 0);
	}

	void ANTzForestWidget::mouseReleaseEvent(QMouseEvent* event) {

		//Reset ANTz mouse values back to the original values
		pData antzData = m_antzData->GetData();
		antzData->io.mouse.mode = kNPmouseModeNull;
		antzData->io.mouse.tool = kNPtoolNull;
		antzData->io.mouse.buttonR = false;

		if (!m_regionSelectionRect.isNull()) {
		
			QRect glRect(m_regionSelectionRect.x(), height() - m_regionSelectionRect.y() - m_regionSelectionRect.height(), m_regionSelectionRect.width(), m_regionSelectionRect.height());
			QItemSelection indexesInRegion;
			m_model->FindIndexesInRegion(glRect, indexesInRegion);
			m_selectionModel->select(indexesInRegion, QItemSelectionModel::Select);
			m_regionSelectionRect = QRect();
		}
	}

	void ANTzForestWidget::mouseMoveEvent(QMouseEvent* event) {

		pData antzData = m_antzData->GetData();
		antzData->io.mouse.tool = kNPtoolNull;
		antzData->io.mouse.mode = kNPmouseModeNull;

		if (event->modifiers() & Qt::ShiftModifier) {

			int x = (m_lastMousePosition.x() < event->x()) ? m_lastMousePosition.x() : event->x();
			int y = (m_lastMousePosition.y() < event->y()) ? m_lastMousePosition.y() : event->y();
			m_regionSelectionRect.setRect(x, y, std::abs(m_lastMousePosition.x() - event->x()), std::abs(m_lastMousePosition.y() - event->y()));
		}
		else {

			if (m_regionSelectionRect.isNull()) {

				antzData->io.mouse.previous.x = m_lastMousePosition.x();
				antzData->io.mouse.previous.y = m_lastMousePosition.y();
				antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
				antzData->io.mouse.delta.y = event->y() - m_lastMousePosition.y();
				antzData->io.mouse.x = event->x();
				antzData->io.mouse.y = event->y();

				if (m_selectionModel->GetFocusList().empty()) {

					antzData->io.mouse.mode = kNPmouseModeCamLook;
				}
				else {

					if (event->buttons() & Qt::LeftButton) {
						if (event->buttons() & Qt::RightButton) {
							antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
						}
						else {
							antzData->io.mouse.mode = kNPmouseModeCamExamXY;
						}
					}
				}
			}
			else {

				m_regionSelectionRect = QRect();
			}
			m_lastMousePosition = event->pos();
		}
	}

	void ANTzForestWidget::keyPressEvent(QKeyEvent* event) {

		//if (m_selectionModel->selectedIndexes().empty()) {

			pData antzData = m_antzData->GetData();
			if ((event->key() == 'w') ||
				(event->key() == 'W') ||
				(event->key() == 's') ||
				(event->key() == 'S') ||
				(event->key() == 'a') ||
				(event->key() == 'A') ||
				(event->key() == 'd') ||
				(event->key() == 'D') ||
				(event->key() == 'e') ||
				(event->key() == 'E') ||
				(event->key() == 'q') ||
				(event->key() == 'Q')) {

				npCtrlCommand(antzData->io.key.map[kKeyDown][npKeyRAWASCII(event->key())], antzData);
				return;
			}
		//}

		QGLWidget::keyPressEvent(event);
	}

	void ANTzForestWidget::keyReleaseEvent(QKeyEvent* event) {

		//if (m_selectionModel->selectedIndexes().empty()) {

			pData antzData = m_antzData->GetData();
			if ((event->key() == 'w') ||
				(event->key() == 'W') ||
				(event->key() == 's') ||
				(event->key() == 'S') ||
				(event->key() == 'a') ||
				(event->key() == 'A') ||
				(event->key() == 'd') ||
				(event->key() == 'D') ||
				(event->key() == 'e') ||
				(event->key() == 'E') ||
				(event->key() == 'q') ||
				(event->key() == 'Q')) {

				npCtrlCommand(antzData->io.key.map[kKeyUp][npKeyRAWASCII(event->key())], antzData);
				return;
			}
		//}

			/*if ((event->key() == 't') || (event->key() == 'T')) {

				SelectFromStylus(SynGlyphXANTz::ANTzBoundingBox::Line());
				return;
			}*/

		QGLWidget::keyReleaseEvent(event);
	}

	void ANTzForestWidget::wheelEvent(QWheelEvent* event) {

		if (!m_selectionModel->GetFocusList().empty()) {

			pData antzData = m_antzData->GetData();
			antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
			//antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
			antzData->io.mouse.delta.y = event->delta() / 10;
			event->accept();
		}

		event->ignore();
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
	}

	bool ANTzForestWidget::IsStereoSupported() const {

		return (format().stereo());
	}*/

	bool ANTzForestWidget::IsInStereoMode() const {

		//pData antzData = m_antzData->GetData();
		//return (antzData->io.gl.stereo);

		return (format().stereo());
	}



	void ANTzForestWidget::moveEvent(QMoveEvent* event) {

		SetZSpacePosition();
		QGLWidget::moveEvent(event);
	}

	void ANTzForestWidget::SetZSpacePosition() {

		if (IsInZSpaceStereo()) {

			QPoint zSpaceViewportPosition = mapToGlobal(QPoint(0, 0));
			zsSetViewportPosition(m_zSpaceViewport, zSpaceViewportPosition.x(), zSpaceViewportPosition.y());

			//ZSError error = zsUpdate(m_zSpaceContext);
		}
	}

	void ANTzForestWidget::ResizeZSpaceViewport() {

		if (IsInZSpaceStereo()) {

			zsSetViewportSize(m_zSpaceViewport, size().width(), size().height());
			glGetFloatv(GL_PROJECTION_MATRIX, m_originialProjectionMatrix.f);

			//ZSError error = zsUpdate(m_zSpaceContext);
		}
	}

	bool ANTzForestWidget::eventFilter(QObject *object, QEvent *event) {

		if ((object == m_topLevelWindow) && (event->type() == QEvent::Move)) {

			SetZSpacePosition();
		}

		return false;
	}

	bool ANTzForestWidget::IsInZSpaceStereo() const {

		return (IsInStereoMode() && IsZSpaceAvailable());
	}

	bool ANTzForestWidget::IsZSpaceAvailable() const {

		return (m_zSpaceContext != nullptr);
	}

	void ANTzForestWidget::SelectFromStylus(const SynGlyphXANTz::ANTzBoundingBox::Line& line) {

		Qt::KeyboardModifiers keyboardModifiers = QGuiApplication::queryKeyboardModifiers();

		std::map<float, int> distanceIdMap;
		pData antzData = m_antzData->GetData();
		for (int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i) {

			pNPnode node = static_cast<pNPnode>(antzData->map.node[i]);
			CheckStylusIntersectionWithNode(node, line, distanceIdMap);
		}

		if (distanceIdMap.empty()) {

			zsStartTargetVibration(m_zSpaceStylus, 0.04f, 0.04f, 4);
		}
		else {

			if ((keyboardModifiers.testFlag(Qt::ControlModifier)) || (keyboardModifiers.testFlag(Qt::ShiftModifier))) {

				QItemSelection stylusItems;
				for (auto id : distanceIdMap) {

					QModelIndex index = m_model->IndexFromANTzID(id.second);
					QItemSelection newStylusItem(index, index);
					stylusItems.merge(newStylusItem, QItemSelectionModel::Select);
				}
				m_selectionModel->select(stylusItems, QItemSelectionModel::Select);	
			}
			else {

				QModelIndex index = m_model->IndexFromANTzID(distanceIdMap.begin()->second);
				m_selectionModel->select(index, QItemSelectionModel::ClearAndSelect);		
			}
		}
	}

	void ANTzForestWidget::CheckStylusIntersectionWithNode(pNPnode node, const SynGlyphXANTz::ANTzBoundingBox::Line& line, std::map<float, int>& distanceIdMap) {

		//Only test on visible objects
		if ((!node->hide) && (node->screen.z >= 0.0f) && (node->screen.z <= 1.0f)) {

			SynGlyphXANTz::ANTzBoundingBox::Intersection intersection = m_boundingBoxes[node->id].DoesLineIntersect(line);
			if (intersection.is_initialized()) {

				distanceIdMap[intersection.get()] = node->id;
			}
		}
		for (int j = 0; j < node->childCount; ++j) {

			CheckStylusIntersectionWithNode(node->child[j], line, distanceIdMap);
		}
	}

	void ANTzForestWidget::ZSpaceButtonPressHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData) {

		if ((eventData->buttonId == 1) || (eventData->buttonId == 2)) {

			m_zSpaceStylusLastPosition.x = eventData->poseMatrix.m03;
			m_zSpaceStylusLastPosition.y = eventData->poseMatrix.m13;
			m_zSpaceStylusLastPosition.z = eventData->poseMatrix.m23;
		}
	}

	void ANTzForestWidget::ZSpaceButtonReleaseHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData) {

		if (eventData->buttonId == 0) {

			SelectFromStylus(m_stylusWorldLine);
		}
	}

	void ANTzForestWidget::ZSpaceStylusMoveHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData) {

		ZSBool isButton2ButtonPressed = false;
		ZSBool isButton1ButtonPressed = false;
		ZSError error = zsIsTargetButtonPressed(targetHandle, 2, &isButton2ButtonPressed);
		error = zsIsTargetButtonPressed(targetHandle, 1, &isButton1ButtonPressed);
		if (!isButton2ButtonPressed && !isButton1ButtonPressed) {

			return;
		}

		pData antzData = m_antzData->GetData();
		antzData->io.mouse.delta.x = (eventData->poseMatrix.m03 - m_zSpaceStylusLastPosition.x) * 1000;

		bool isSelectionEmpty = m_selectionModel->selectedIndexes().empty();

		if (isButton2ButtonPressed) {

			if (isSelectionEmpty) {

				antzData->io.mouse.mode = kNPmouseModeCamLook;
				antzData->io.mouse.delta.y = -(eventData->poseMatrix.m13 - m_zSpaceStylusLastPosition.y) * 1000;
			}
			else {

				antzData->io.mouse.mode = kNPmouseModeCamExamXY;
				antzData->io.mouse.delta.y = -(eventData->poseMatrix.m13 - m_zSpaceStylusLastPosition.y) * 1000;
			}
		}
		else if (isButton1ButtonPressed && !isSelectionEmpty) {

			antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
			antzData->io.mouse.delta.y = -(eventData->poseMatrix.m23 - m_zSpaceStylusLastPosition.z) * 1000;
		}

		m_zSpaceStylusLastPosition.x = eventData->poseMatrix.m03;
		m_zSpaceStylusLastPosition.y = eventData->poseMatrix.m13;
		m_zSpaceStylusLastPosition.z = eventData->poseMatrix.m23;
	}

	void ANTzForestWidget::ZSpaceStylusTapHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData) {

		//emit NewStatusMessage(tr("Source: (%1, %2, %3)").arg(m_stylusWorldLine.first.x).arg(m_stylusWorldLine.first.y).arg(m_stylusWorldLine.first.z) +
		//	" " + tr("Dest: (%1, %2, %3)").arg(m_stylusWorldLine.second.x).arg(m_stylusWorldLine.second.y).arg(m_stylusWorldLine.second.z), 4000);

		//SelectFromStylus(m_stylusWorldTapLine);
	}

	void ANTzForestWidget::CreateBoundingBoxes() {

		m_boundingBoxes.clear();
		m_objectsThatNeedBoundingBoxUpdates.clear();

		pData antzData = m_antzData->GetData();
		pNPnode rootGrid = static_cast<pNPnode>(antzData->map.node[kNPnodeRootGrid]);
		for (int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i) {

			pNPnode node = static_cast<pNPnode>(antzData->map.node[i]);
			m_boundingBoxes[node->id] = SynGlyphXANTz::ANTzBoundingBox::CreateBoundingBox(node, rootGrid->scale.z);
			bool doesBoundingBoxNeedUpdating = IsNodeAnimated(node);
			if (doesBoundingBoxNeedUpdating) {

				m_objectsThatNeedBoundingBoxUpdates.insert(node->id);
			}

			for (int j = 0; j < node->childCount; ++j) {

				CreateBoundingBoxes(node->child[j], m_boundingBoxes[node->id].GetTransform(), doesBoundingBoxNeedUpdating);
			}
		}
	}

	void ANTzForestWidget::CreateBoundingBoxes(pNPnode node, const glm::mat4& parentTransform, bool isAncestorBoundingBoxBeingUpdated) {

		bool doesBoundingBoxNeedUpdating = false;
	
		if (!isAncestorBoundingBoxBeingUpdated) {

			doesBoundingBoxNeedUpdating = IsNodeAnimated(node);
			if (doesBoundingBoxNeedUpdating) {

				m_objectsThatNeedBoundingBoxUpdates.insert(node->id);
			}
		}

		m_boundingBoxes[node->id] = SynGlyphXANTz::ANTzBoundingBox::CreateBoundingBox(node, parentTransform);
		for (int j = 0; j < node->childCount; ++j) {

			CreateBoundingBoxes(node->child[j], m_boundingBoxes[node->id].GetTransform(), doesBoundingBoxNeedUpdating);
		}
	}

	void ANTzForestWidget::DrawBoundingBoxes() {

		qglColor(Qt::magenta);

		for (auto boundingBox : m_boundingBoxes) {

			glm::vec3 minPoint = boundingBox.second.GetMinCorner();
			glm::vec3 maxPoint = boundingBox.second.GetMaxCorner();

			glm::vec4 bll = boundingBox.second.GetTransform() * glm::vec4(minPoint.x, minPoint.y, minPoint.z, 1.0f);
			glm::vec4 blr = boundingBox.second.GetTransform() * glm::vec4(maxPoint.x, minPoint.y, minPoint.z, 1.0f);

			glm::vec4 bur = boundingBox.second.GetTransform() * glm::vec4(maxPoint.x, maxPoint.y, minPoint.z, 1.0f);
			glm::vec4 bul = boundingBox.second.GetTransform() * glm::vec4(minPoint.x, maxPoint.y, minPoint.z, 1.0f);

			glm::vec4 tll = boundingBox.second.GetTransform() * glm::vec4(minPoint.x, minPoint.y, maxPoint.z, 1.0f);
			glm::vec4 tlr = boundingBox.second.GetTransform() * glm::vec4(maxPoint.x, minPoint.y, maxPoint.z, 1.0f);

			glm::vec4 tur = boundingBox.second.GetTransform() * glm::vec4(maxPoint.x, maxPoint.y, maxPoint.z, 1.0f);
			glm::vec4 tul = boundingBox.second.GetTransform() * glm::vec4(minPoint.x, maxPoint.y, maxPoint.z, 1.0f);

			glBegin(GL_LINE_LOOP);
			glVertex3f(bll.x, bll.y, bll.z);
			glVertex3f(blr.x, blr.y, blr.z);
			glVertex3f(bur.x, bur.y, bur.z);
			glVertex3f(bul.x, bul.y, bul.z);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glVertex3f(tll.x, tll.y, tll.z);
			glVertex3f(tlr.x, tlr.y, tlr.z);
			glVertex3f(tur.x, tur.y, tur.z);
			glVertex3f(tul.x, tul.y, tul.z);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glVertex3f(bll.x, bll.y, bll.z);
			glVertex3f(tll.x, tll.y, tll.z);
			glVertex3f(tul.x, tul.y, tul.z);
			glVertex3f(bul.x, bul.y, bul.z);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glVertex3f(blr.x, blr.y, blr.z);
			glVertex3f(tlr.x, tlr.y, tlr.z);
			glVertex3f(tur.x, tur.y, tur.z);
			glVertex3f(bur.x, bur.y, bur.z);
			glEnd();
		}
	}

	void ANTzForestWidget::OnModelReset() {

		CreateBoundingBoxes();
		StoreRotationRates();

		const QStringList& textures = m_model->GetBaseImageFilenames();

		for (unsigned int textureID : m_textureIDs) {

			deleteTexture(textureID);
		}
		m_textureIDs.clear();
	
		Q_FOREACH(const QString& texture, textures) {

			m_textureIDs.push_back(BindTextureInFile(texture));
		}

		pData antzData = m_antzData->GetData();
		antzData->io.gl.textureCount = m_textureIDs.size() + 1;

		pNPnode rootGrid = static_cast<pNPnode>(antzData->map.node[kNPnodeRootGrid]);
		SetGridTexture(rootGrid);
		for (int i = 0; i < rootGrid->childCount; ++i) {

			SetGridTexture(rootGrid->child[i]);
		}
	
		m_isReseting = false;
		update();

		ResetCamera();
	}

	void ANTzForestWidget::SetGridTexture(pNPnode grid) {

		if (grid->textureID == 1) {

			grid->textureID = m_worldTextureID;
		}
		else {

			grid->textureID = m_textureIDs[grid->textureID - 2];
		}
	}

	unsigned int ANTzForestWidget::BindTextureInFile(const QString& imageFilename) {

		QImage image(imageFilename);
		return bindTexture(image);
	}

	const QGLFormat& ANTzForestWidget::GetNonStereoFormat() {

		return s_format;
	}

	const QGLFormat& ANTzForestWidget::GetStereoFormat() {

		return s_stereoFormat;
	}

	void ANTzForestWidget::StoreRotationRates() {

		m_rotationRates.clear();
		pData antzData = m_antzData->GetData();
	
		for (int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i) {

			StoreRotationRates(static_cast<pNPnode>(antzData->map.node[i]));
		}
	}

	void ANTzForestWidget::StoreRotationRates(pNPnode node) {

		if (IsNodeAnimated(node)) {

			m_rotationRates[node->id] = node->rotateRate;
		}

		for (int i = 0; i < node->childCount; ++i) {

			StoreRotationRates(node->child[i]);
		}
	}

	void ANTzForestWidget::ShowAnimatedRotations(bool show) {

		if (show != m_showAnimation) {

			pData antzData = m_antzData->GetData();
			m_showAnimation = show;
			if (m_showAnimation) {

				for (auto idRatePair : m_rotationRates) {

					pNPnode node = static_cast<pNPnode>(antzData->map.nodeID[idRatePair.first]);
					node->rotateRate.x = idRatePair.second.x;
					node->rotateRate.y = idRatePair.second.y;
					node->rotateRate.z = idRatePair.second.z;
				}
			}
			else {

				for (auto idRatePair : m_rotationRates) {

					pNPnode node = static_cast<pNPnode>(antzData->map.nodeID[idRatePair.first]);
					node->rotateRate.x = 0.0f;
					node->rotateRate.y = 0.0f;
					node->rotateRate.z = 0.0f;
				}
			}
		}
	}

	bool ANTzForestWidget::IsNodeAnimated(pNPnode node) {

		return ((node->rotateRate.x != 0.0f) || (node->rotateRate.y != 0.0f) || (node->rotateRate.z != 0.0f));
	}

	void ANTzForestWidget::UpdateBoundingBoxes() {

		pData antzData = m_antzData->GetData();
		pNPnode rootGrid = static_cast<pNPnode>(antzData->map.node[kNPnodeRootGrid]);
		for (int id : m_objectsThatNeedBoundingBoxUpdates) {

			pNPnode node = static_cast<pNPnode>(antzData->map.nodeID[id]);
			if (node->parent == nullptr) {

				m_boundingBoxes[node->id].SetTransform(SynGlyphXANTz::ANTzBoundingBox::CreateTransform(node, rootGrid->scale.z));
				for (int i = 0; i < node->childCount; ++i) {

					UpdateBoundingBoxes(node->child[i], m_boundingBoxes[node->id].GetTransform());
				}
			}
			else {

				UpdateBoundingBoxes(node, m_boundingBoxes[node->parent->id].GetTransform());
			}
		}
	}

	void ANTzForestWidget::UpdateBoundingBoxes(pNPnode node, const glm::mat4& parentTransform) {

		m_boundingBoxes[node->id].SetTransform(SynGlyphXANTz::ANTzBoundingBox::CreateTransform(node, parentTransform));
		for (int i = 0; i < node->childCount; ++i) {

			UpdateBoundingBoxes(node->child[i], m_boundingBoxes[node->id].GetTransform());
		}
	}

	void ANTzForestWidget::SetZSpaceOptions(const SynGlyphX::ZSpaceOptions& options) {

		m_zSpaceOptions = options;
	}

	const SynGlyphX::ZSpaceOptions& ANTzForestWidget::GetZSpaceOptions() const {

		return m_zSpaceOptions;
	}

	bool ANTzForestWidget::GetHideUnselectedGlyphTrees() const {

		return m_hideUnselectedGlyphTrees;
	}

} //namespace SynGlyphXANTz