#include "antzviewerwidget.h"
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
#include "sourcedataselectionmodel.h"

//The default QGLFormat works for now except we want alpha enabled.  Also want to try and get a stereo enabled context
QGLFormat ANTzViewerWidget::s_format(QGL::AlphaChannel);
QGLFormat ANTzViewerWidget::s_stereoFormat(QGL::AlphaChannel | QGL::StereoBuffers);

//Length of zSpace stylus line
const float ANTzViewerWidget::s_stylusLength = 0.1f;  // Meters

ANTzViewerWidget::ANTzViewerWidget(const QGLFormat& format, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
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
	m_hideUnselectedGlyphTrees(false)
{
	setFocusPolicy(Qt::StrongFocus);

	QFont newFont = font();
	newFont.setPointSize(12);
	setFont(newFont);

    InitIO();

	//Change fly speeds
	m_antzData->GetData()->ctrl.slow = 0.5f;
	m_antzData->GetData()->ctrl.fast = 0.75f;

	if (m_selectionModel != nullptr) {
		QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ANTzViewerWidget::UpdateSelection);
	}

	QObject::connect(m_model, &GlyphForestModel::modelReset, this, &ANTzViewerWidget::OnModelReset);
	QObject::connect(m_model, &GlyphForestModel::modelAboutToBeReset, this, [this]{ m_isReseting = true; });

	if (IsInStereoMode()) {

		m_antzData->GetData()->io.gl.stereo = true;
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

			error = zsAddTrackerEventHandler(m_zSpaceStylus, ZS_TRACKER_EVENT_BUTTON_PRESS, &ZSpaceEventDispatcher, this);
			CheckZSpaceError(error);
			error = zsAddTrackerEventHandler(m_zSpaceStylus, ZS_TRACKER_EVENT_BUTTON_RELEASE, &ZSpaceEventDispatcher, this);
			CheckZSpaceError(error);
			error = zsAddTrackerEventHandler(m_zSpaceStylus, ZS_TRACKER_EVENT_MOVE, &ZSpaceEventDispatcher, this);
			CheckZSpaceError(error);

			error = zsSetTargetVibrationEnabled(m_zSpaceStylus, true);
			CheckZSpaceError(error);
		}
		catch (const std::exception& e) {
			throw;
		}
	}

	setFocus();
}

ANTzViewerWidget::~ANTzViewerWidget()
{
	if (m_worldTextureID != 0) {

		deleteTexture(m_worldTextureID);
		m_worldTextureID = 0;
	}

	ClearZSpaceContext();
	npCloseGL(m_antzData->GetData());
}

void ANTzViewerWidget::ClearZSpaceContext() {

	if (m_zSpaceContext != nullptr) {

		zsShutdown(m_zSpaceContext);
	}
}

void ANTzViewerWidget::CheckZSpaceError(ZSError error) {

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

void ANTzViewerWidget::InitIO()
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

void ANTzViewerWidget::initializeGL() {
    
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

void ANTzViewerWidget::resizeGL(int w, int h) {

	ResizeZSpaceViewport();

    npGLResizeScene(w, h);
}

void ANTzViewerWidget::paintGL() {

	if (m_isReseting) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}

	pData antzData = m_antzData->GetData();

	npUpdateCh(antzData);

	npUpdateEngine(antzData);		//position, physics, interactions...

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

	if (IsInStereoMode())  {

		DrawSceneForEye(Eye::Right, true);
	}
	
	DrawSceneForEye(Eye::Left, true);

    int err = glGetError();
    if (err) {
        printf("err: 2388 - OpenGL error: %d\n", err);
    }

    //ANTz assumes that redraw constantly happens.  Need to put this in a thread
    update();

    //QGLWidget takes care of swapping buffers
}

void ANTzViewerWidget::DrawSceneForEye(Eye eye, bool drawHUD) {

	pData antzData = m_antzData->GetData();
	pNPnode camNode = npGetActiveCam(antzData);
	NPcameraPtr camData = static_cast<NPcameraPtr>(camNode->data);

	ZSEye zSpaceEye;
	if (eye = Eye::Left) {

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

		ZSMatrix4 originalCameraMatrix;
		npInvertMatrixf(m_originialViewMatrix.f, originalCameraMatrix.f);

		// Transform the stylus pose from camera space to world space.
		// This is done by multiplying the pose (camera space) by the 
		// application's camera matrix.
		npMultMatrix(m_zSpaceStylusWorldMatrix.f, originalCameraMatrix.f, stylusPose.matrix.f);

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

	if (drawHUD) {

		ZSFloat stylusLength = s_stylusLength;
		QColor stylusColor = Qt::green; // Qt::red;
		DrawZSpaceStylus(m_zSpaceStylusWorldMatrix, stylusLength, stylusColor);
		DrawHUD();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void ANTzViewerWidget::DrawZSpaceStylus(const ZSMatrix4& stylusMatrix, ZSFloat stylusLength, QColor color) {

	if (IsInZSpaceStereo()) {

		qglColor(color);

		// Multiply the model-view matrix by the stylus world matrix.
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(stylusMatrix.f);

		ZSMatrix4 trackerPosition;
		ZSError error = zsTransformMatrix(m_zSpaceViewport, ZS_COORDINATE_SPACE_TRACKER, ZS_COORDINATE_SPACE_DISPLAY, &trackerPosition);

		// Draw the line.
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, -stylusLength);

		glEnd();

		glPopMatrix();
	}
}

void ANTzViewerWidget::DrawHUD() {

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

	pNPnode selectedNode = nullptr;
	if (!m_selectionModel->selectedIndexes().empty()) {

		selectedNode = static_cast<pNPnode>(m_selectionModel->selectedIndexes().front().internalPointer());
		if (strcmp(selectedNode->tag->title, "No Tag") != 0) {

			renderText(selectedNode->world.x, selectedNode->world.y, selectedNode->world.z, selectedNode->tag->title, m_oglTextFont);
		}
	}

	//Draw HUD
	QString positionHUD;
	if (selectedNode != nullptr) {

		positionHUD = tr("Glyph Position: X: %1, Y: %2, Z: %3").arg(QString::number(selectedNode->world.x), QString::number(selectedNode->world.y), QString::number(selectedNode->world.z));
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

void ANTzViewerWidget::SetZSpaceMatricesForDrawing(ZSEye eye, const ZSMatrix4& originialViewMatrix, NPcameraPtr camData) {

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

void ANTzViewerWidget::UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected) {

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

    //Set the cam target if there is only one object selected
    const QModelIndexList& currentSelection = m_selectionModel->selectedIndexes();
    
    int nodeRootIndex = 0;
    if (!currentSelection.isEmpty())  {
        const QModelIndex& last = currentSelection.back();
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

void ANTzViewerWidget::UpdateGlyphTreesShowHideForSelection() {

	if (m_hideUnselectedGlyphTrees) {

		const QModelIndexList& currentSelection = m_selectionModel->selectedIndexes();
		if (currentSelection.empty()) {

			ShowAllGlyphTrees();
		}
		else {

			pData antzData = m_antzData->GetData();
			SynGlyphX::IndexSet selectedGlyphTrees = SourceDataSelectionModel::GetRootRows(currentSelection);
			for (unsigned int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i) {

				pNPnode node = static_cast<pNPnode>(antzData->map.node[i]);
				node->hide = (selectedGlyphTrees.count(i - kNPnodeRootPin) == 0);
			}
		}
	}
}

void ANTzViewerWidget::SetHideUnselectedGlyphTrees(bool hideUnselectedGlyphTrees) {

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

void ANTzViewerWidget::ShowAllGlyphTrees() {

	pData antzData = m_antzData->GetData();
	for (unsigned int i = kNPnodeRootPin; i < antzData->map.nodeRootCount; ++i) {
	
		pNPnode node = static_cast<pNPnode>(antzData->map.node[i]);
		node->hide = false;
	}
}

void ANTzViewerWidget::SetCameraToDefaultPosition() {

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

void ANTzViewerWidget::ResetCamera() {
    
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

void ANTzViewerWidget::CenterCameraOnNode(pNPnode node) {
	
	pData antzData = m_antzData->GetData();

    npSetCamTargetNode(node, antzData);
	antzData->io.mouse.targeting = false;
    antzData->map.selectedPinNode = node;
	antzData->map.selectedPinIndex = node->id;

    //Always keep current node set to current cam
    antzData->map.currentNode = antzData->map.currentCam;
}

void ANTzViewerWidget::mousePressEvent(QMouseEvent* event) {

    if (event->button() == Qt::LeftButton) {
		
		Qt::KeyboardModifiers keys = event->modifiers();
		if (keys & Qt::ShiftModifier) {

			m_selectionModel->clearSelection();
		}
		else {

			SelectAtPoint(event->x(), event->y(), keys & Qt::ControlModifier);
		}
    }

    m_lastMousePosition = event->pos();
}

bool ANTzViewerWidget::SelectAtPoint(int x, int y, bool multiSelect) {

	int pickID = 0;

	pData antzData = m_antzData->GetData();
	//bool isStereo = IsInStereoMode();
	//antzData->io.gl.stereo = false;

	if (IsInStereoMode()) {

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		npSetLookAtFromCamera(antzData);

		antzData->io.gl.stereo = false;
		antzData->io.gl.pickPass = true;
		glReadBuffer(GL_BACK_LEFT);

		DrawSceneForEye(Eye::Left, false);

		pickID = npPickPinStereoSingleEye(x, antzData->io.gl.height - y, antzData);

		if (pickID == 0) {

			glReadBuffer(GL_BACK_RIGHT);
			DrawSceneForEye(Eye::Right, false);
			pickID = npPickPinStereoSingleEye(x, antzData->io.gl.height - y, antzData);
		}

		antzData->io.gl.stereo = true;
		antzData->io.gl.pickPass = false;	//clear the flag to draw normal colors
	}
	else {

		pickID = npPickPin(x, antzData->io.gl.height - y, antzData);
	}

	if (pickID != 0) {

		pNPnode node = static_cast<pNPnode>(antzData->map.nodeID[pickID]);
		QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect;
		if (multiSelect) {

			flags = QItemSelectionModel::Toggle;
		}

		m_selectionModel->select(m_model->IndexFromANTzID(pickID), flags);
	}

	emit NewStatusMessage(tr("Selection Attempt At: %1, %2").arg(x).arg(y), 4000);
	
	return (pickID != 0);
}

void ANTzViewerWidget::mouseReleaseEvent(QMouseEvent* event) {

    //Reset ANTz mouse values back to the original values
	pData antzData = m_antzData->GetData();
    antzData->io.mouse.mode = kNPmouseModeNull;
    antzData->io.mouse.tool = kNPtoolNull;
    antzData->io.mouse.buttonR = false;

	if (!m_regionSelectionRect.isNull()) {
		
		QRect glRect(m_regionSelectionRect.x(), height() - m_regionSelectionRect.y() - m_regionSelectionRect.height(), m_regionSelectionRect.width(), m_regionSelectionRect.height());
		QModelIndexList indexesInRegion = m_model->FindIndexesInRegion(glRect);
		Q_FOREACH(const QModelIndex& index, indexesInRegion) {

			m_selectionModel->select(index, QItemSelectionModel::Select);
		}
		m_regionSelectionRect = QRect();
	}
}

void ANTzViewerWidget::mouseMoveEvent(QMouseEvent* event) {

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

			if (m_selectionModel->selectedIndexes().empty()) {

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

void ANTzViewerWidget::keyPressEvent(QKeyEvent* event) {

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

void ANTzViewerWidget::keyReleaseEvent(QKeyEvent* event) {

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

	QGLWidget::keyReleaseEvent(event);
}

void ANTzViewerWidget::wheelEvent(QWheelEvent* event) {

	if (!m_selectionModel->selectedIndexes().empty()) {

		pData antzData = m_antzData->GetData();
		antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
		//antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
		antzData->io.mouse.delta.y = event->delta() / 10;
		event->accept();
	}

	event->ignore();
}
/*
void ANTzViewerWidget::SetStereo(bool enableStereo) {

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

bool ANTzViewerWidget::IsStereoSupported() const {

	return (format().stereo());
}*/

bool ANTzViewerWidget::IsInStereoMode() const {

	//pData antzData = m_antzData->GetData();
	//return (antzData->io.gl.stereo);

	return (format().stereo());
}



void ANTzViewerWidget::moveEvent(QMoveEvent* event) {

	SetZSpacePosition();
	QGLWidget::moveEvent(event);
}

void ANTzViewerWidget::SetZSpacePosition() {

	if (IsInZSpaceStereo()) {

		QPoint zSpaceViewportPosition = mapToGlobal(QPoint(0, 0));
		zsSetViewportPosition(m_zSpaceViewport, zSpaceViewportPosition.x(), zSpaceViewportPosition.y());

		//ZSError error = zsUpdate(m_zSpaceContext);
	}
}

void ANTzViewerWidget::ResizeZSpaceViewport() {

	if (IsInZSpaceStereo()) {

		zsSetViewportSize(m_zSpaceViewport, size().width(), size().height());
		glGetFloatv(GL_PROJECTION_MATRIX, m_originialProjectionMatrix.f);

		//ZSError error = zsUpdate(m_zSpaceContext);
	}
}

bool ANTzViewerWidget::eventFilter(QObject *object, QEvent *event) {

	if ((object == m_topLevelWindow) && (event->type() == QEvent::Move)) {

		SetZSpacePosition();
	}

	return false;
}

bool ANTzViewerWidget::IsInZSpaceStereo() const {

	return (IsInStereoMode() && IsZSpaceAvailable());
}

bool ANTzViewerWidget::IsZSpaceAvailable() const {

	return (m_zSpaceContext != nullptr);
}

void ANTzViewerWidget::ZSpaceButtonPressHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData) {

	if (eventData->buttonId == 2) {

		m_zSpaceStylusLastPosition.x = eventData->poseMatrix.m03;
		m_zSpaceStylusLastPosition.y = eventData->poseMatrix.m13;
		m_zSpaceStylusLastPosition.z = eventData->poseMatrix.m23;
	}
}

void ANTzViewerWidget::ZSpaceButtonReleaseHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData) {

	if (eventData->buttonId == 0) {

		ZSTrackerPose stylusPose;
		ZSError error = zsGetTargetPose(m_zSpaceStylus, &stylusPose);

		glPushMatrix();
		glMultMatrixf(m_zSpaceStylusWorldMatrix.f);
		glTranslatef(0.0f, 0.0f, -s_stylusLength);
		NPfloatXYZ offset = { 0.0f, 0.0f, 0.0f };
		NPfloatXYZ screenPoint = npProjectWorldToScreen(&offset);
		glPopMatrix();
		if (!SelectAtPoint(screenPoint.x, screenPoint.y, false)) {

			zsStartTargetVibration(m_zSpaceStylus, 0.04f, 0.04f, 4);
		}
	}
}

void ANTzViewerWidget::ZSpaceStylusMoveHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData) {

	//QMessageBox::information(this, tr("Stylus Move Handler"), tr("Button ") + QString::number(eventData->buttonId));
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

void ANTzViewerWidget::ZSpaceEventDispatcher(ZSHandle targetHandle, const ZSTrackerEventData* eventData, const void* userData) {

	if (userData != nullptr) {

		ANTzViewerWidget* widget = const_cast<ANTzViewerWidget*>(static_cast<const ANTzViewerWidget*>(userData));
		if (eventData->type == ZS_TRACKER_EVENT_BUTTON_RELEASE) {
			
			widget->ZSpaceButtonReleaseHandler(targetHandle, eventData);
		}
		else if (eventData->type == ZS_TRACKER_EVENT_MOVE) {

			widget->ZSpaceStylusMoveHandler(targetHandle, eventData);
		}
		else if (eventData->type == ZS_TRACKER_EVENT_BUTTON_PRESS) {

			widget->ZSpaceButtonPressHandler(targetHandle, eventData);
		}
	}
}

void ANTzViewerWidget::OnModelReset() {

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

void ANTzViewerWidget::SetGridTexture(pNPnode grid) {

	if (grid->textureID == 1) {

		grid->textureID = m_worldTextureID;
	}
	else {

		grid->textureID = m_textureIDs[grid->textureID - 2];
	}
}

unsigned int ANTzViewerWidget::BindTextureInFile(const QString& imageFilename) {

	QImage image(imageFilename);
	return bindTexture(image);
}

const QGLFormat& ANTzViewerWidget::GetNonStereoFormat() {

	return s_format;
}

const QGLFormat& ANTzViewerWidget::GetStereoFormat() {

	return s_stereoFormat;
}