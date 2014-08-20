#include "antzviewerwidget.h"

#include <QtGUI/QMouseEvent>
#include <QtGUI/QFont>

#include "io/npfile.h"
#include "io/npch.h"
#include "npctrl.h"
#include "io/npgl.h"
#include "npui.h"
#include "ctrl/npengine.h"
#include "npctrl.h"

//The default QGLFormat works for now except we want alpha enabled.  Also want to try and get a stereo enabled context
QGLFormat ANTzViewerWidget::s_format(QGL::AlphaChannel | QGL::StereoBuffers);

ANTzViewerWidget::ANTzViewerWidget(GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QGLWidget(s_format, parent),
	m_model(model),
    m_selectionModel(selectionModel),
	m_zSpaceContext(nullptr),
	m_zSpaceDisplay(nullptr),
	m_zSpaceBuffer(nullptr),
	m_zSpaceViewport(nullptr),
	m_zSpaceFrustum(nullptr),
	m_zSpaceStylus(nullptr),
	m_topLevelWindow(nullptr)
{
	setFocusPolicy(Qt::StrongFocus);

	QFont newFont = font();
	newFont.setPointSize(12);
	setFont(newFont);

    pData antzData = m_model->GetANTzData();
    InitIO();
    npInitFile(antzData);
    npInitCh(antzData);
    npInitCtrl(antzData);

	//Change fly speeds
	antzData->ctrl.slow = 0.5f;
	antzData->ctrl.fast = 0.75f;

	if (m_selectionModel != nullptr) {
		QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ANTzViewerWidget::UpdateSelection);
	}

	QObject::connect(m_model, &GlyphForestModel::modelReset, this, &ANTzViewerWidget::ResetCamera);

	if (IsStereoSupported()) {

		try {
			ZSError error = zsInitialize(&m_zSpaceContext);
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

			// Find the zSpace display
			error = zsFindDisplayByType(m_zSpaceContext, ZS_DISPLAY_TYPE_ZSPACE, 0, &m_zSpaceDisplay);
			CheckZSpaceError(error);

			//Get the top level window so that we can track its movements for zSpace viewport
			m_topLevelWindow = parentWidget();
			while (!m_topLevelWindow->isWindow()) {

				m_topLevelWindow = m_topLevelWindow->parentWidget();
			}
			m_topLevelWindow->installEventFilter(this);

			error = zsSetMouseEmulationEnabled(m_zSpaceContext, true);
			CheckZSpaceError(error);

			error = zsSetMouseEmulationTarget(m_zSpaceContext, m_zSpaceStylus);
			CheckZSpaceError(error);

			error = zsSetMouseEmulationButtonMapping(m_zSpaceContext, 0, ZS_MOUSE_BUTTON_LEFT);
			CheckZSpaceError(error);

			error = zsSetMouseEmulationButtonMapping(m_zSpaceContext, 1, ZS_MOUSE_BUTTON_RIGHT);
			CheckZSpaceError(error);

			antzData->io.gl.stereo = true;
		}
		catch (const std::exception& e) {
			throw;
		}
	}
}

ANTzViewerWidget::~ANTzViewerWidget()
{
	if (m_zSpaceContext != nullptr) {

		zsShutdown(m_zSpaceContext);
	}

    void* antzData = m_model->GetANTzData();
    npCloseGL(antzData);
    npCloseCtrl(antzData);
    npCloseFile(antzData);
    npCloseCh(antzData);
}

void ANTzViewerWidget::CheckZSpaceError(ZSError error) {

	if (error != ZS_ERROR_OKAY) {
		throw std::exception(QString("zSpace error: %1").arg(error).toStdString().c_str());
	}
}

void ANTzViewerWidget::InitIO()
{
    pData data = (pData)m_model->GetANTzData();

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
    
    npInitGL(m_model->GetANTzData());
    ResetCamera();
}

void ANTzViewerWidget::resizeGL(int w, int h) {

	ResizeZSpaceViewport();

    npGLResizeScene(w, h);
}

void ANTzViewerWidget::paintGL() {

	pData antzData = m_model->GetANTzData();
	pNPnode camNode = npGetActiveCam(antzData);
	NPcameraPtr camData = static_cast<NPcameraPtr>(camNode->data);

	npUpdateCh(antzData);

	npUpdateEngine(antzData);		//position, physics, interactions...

	//We may need to have a selected pin node during update to position the camera, but we don't want it during drawing
	antzData->map.selectedPinNode = NULL;
	antzData->map.selectedPinIndex = 0;

	// zero out our mouse to prevent drifting objects
	antzData->io.mouse.delta.x = 0.0f;
	antzData->io.mouse.delta.y = 0.0f;

	if (IsInZSpaceStereo()) {

		ZSError error = zsBeginStereoBufferFrame(m_zSpaceBuffer);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	npSetLookAtFromCamera(antzData);

	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	if (IsInStereoMode()) {

		ZSMatrix4 originialViewMatrix;

		glDrawBuffer(GL_BACK_RIGHT);

		if (m_zSpaceContext != nullptr) {

			glGetFloatv(GL_MODELVIEW_MATRIX, originialViewMatrix.f);
			SetZSpaceMatricesForDrawing(ZS_EYE_RIGHT, originialViewMatrix, camData);
		}

		glMatrixMode(GL_MODELVIEW);

		npGLLighting(antzData);
		npGLShading(antzData);
		npDrawNodes(antzData);

		DrawSelectedNodeAndHUDText();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glPushMatrix();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glDrawBuffer(GL_BACK_LEFT);
		if (m_zSpaceContext != nullptr) {

			SetZSpaceMatricesForDrawing(ZS_EYE_LEFT, originialViewMatrix, camData);
		}
	}
	else {

		glGetFloatv(GL_MODELVIEW_MATRIX, camData->matrix);
		npInvertMatrixf(camData->matrix, camData->inverseMatrix);
	}

	glMatrixMode(GL_MODELVIEW);

	npGLLighting(antzData);
	npGLShading(antzData);
	npDrawNodes(antzData);

	DrawSelectedNodeAndHUDText();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

    int err = glGetError();
    if (err) {
        printf("err: 2388 - OpenGL error: %d\n", err);
    }

    //ANTz assumes that redraw constantly happens.  Need to put this in a thread
    update();

    //QGLWidget takes care of swapping buffers
}

void ANTzViewerWidget::DrawSelectedNodeAndHUDText() {

	pData antzData = m_model->GetANTzData();

	bool reenableDepthTest = glIsEnabled(GL_DEPTH_TEST);
	if (reenableDepthTest) {
		glDisable(GL_DEPTH_TEST);
	}

	//Draw tags
	qglColor(Qt::white);

	pNPnode selectedNode = nullptr;
	if (!m_selectionModel->selectedIndexes().empty()) {

		selectedNode = static_cast<pNPnode>(m_selectionModel->selectedIndexes().front().internalPointer());
		if (strcmp(selectedNode->tag->title, "No Tag") != 0) {

			renderText(selectedNode->world.x, selectedNode->world.y, selectedNode->world.z, selectedNode->tag->title);
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

	const QFontMetrics& hudFontMetrics = fontMetrics();
	renderText((width() / 2) - (hudFontMetrics.width(positionHUD) / 2), height() - 16, positionHUD);

	if (reenableDepthTest) {
		glEnable(GL_DEPTH_TEST);
	}
}

void ANTzViewerWidget::SetZSpaceMatricesForDrawing(ZSEye eye, const ZSMatrix4& originialViewMatrix, NPcameraPtr camData) {

	ZSMatrix4 zSpaceEyeViewMatrix;
	ZSError mverror = zsGetFrustumViewMatrix(m_zSpaceFrustum, eye, &zSpaceEyeViewMatrix);

	glMatrixMode(GL_MODELVIEW);

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

    pData antzData = m_model->GetANTzData();

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
    if (currentSelection.length() > 0)  {
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

void ANTzViewerWidget::ResetCamera() {
    
    pData antzData = m_model->GetANTzData();

	//We only want to center the camera when there are actual root nodes
	if (antzData->map.nodeRootCount > kNPnodeRootPin) {

		//Need to draw once so that camera centering works properly
		updateGL();

		CenterCameraOnNode(static_cast<pNPnode>(antzData->map.node[kNPnodeRootPin]));
		
		//Need to draw again so that centering on the node works properly before resetting proximity
		updateGL();

		antzData->map.currentCam->proximity.x = 0;
    }
}

void ANTzViewerWidget::CenterCameraOnNode(pNPnode node) {
	
    pData antzData = m_model->GetANTzData();

    npSetCamTargetNode(node, antzData);
	antzData->io.mouse.targeting = false;
    antzData->map.selectedPinNode = node;
	antzData->map.selectedPinIndex = node->id;

    //Always keep current node set to current cam
    antzData->map.currentNode = antzData->map.currentCam;
}

void ANTzViewerWidget::mousePressEvent(QMouseEvent* event) {

    if (event->button() == Qt::LeftButton) {
		pData antzData = m_model->GetANTzData();
		int pickID = npPickPin(event->x(), antzData->io.gl.height - event->y(), antzData);

		if (pickID != 0) {

			pNPnode node = static_cast<pNPnode>(antzData->map.nodeID[pickID]);
			QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::Select;
			if (node->selected) {
				flags = QItemSelectionModel::Clear;
			}
			else {
				m_selectionModel->clearSelection();
			}
			
			m_selectionModel->select(m_model->IndexFromANTzID(pickID), flags);
        }
    }

    m_lastMousePosition = event->pos();
}

void ANTzViewerWidget::mouseReleaseEvent(QMouseEvent* event) {

    //Reset ANTz mouse values back to the original values
    pData antzData = m_model->GetANTzData();
    antzData->io.mouse.mode = kNPmouseModeNull;
    antzData->io.mouse.tool = kNPtoolNull;
    antzData->io.mouse.buttonR = false;
}

void ANTzViewerWidget::mouseMoveEvent(QMouseEvent* event) {

    pData antzData = m_model->GetANTzData();

    antzData->io.mouse.previous.x = m_lastMousePosition.x();
    antzData->io.mouse.previous.y = m_lastMousePosition.y();
    antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
    antzData->io.mouse.delta.y = event->y() - m_lastMousePosition.y();
    antzData->io.mouse.x = event->x();
    antzData->io.mouse.y = event->y();
	antzData->io.mouse.tool = kNPtoolNull;

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

    m_lastMousePosition = event->pos();
}

void ANTzViewerWidget::keyPressEvent(QKeyEvent* event) {

	if (m_selectionModel->selectedIndexes().empty()) {

		pData antzData = m_model->GetANTzData();
		if ((event->key() == 'w') ||
			(event->key() == 'W') ||
			(event->key() == 'a') ||
			(event->key() == 'A') ||
			(event->key() == 's') ||
			(event->key() == 'S') ||
			(event->key() == 'd') ||
			(event->key() == 'D') ||
			(event->key() == 'e') ||
			(event->key() == 'E') ||
			(event->key() == 'q') ||
			(event->key() == 'Q')) {

			npCtrlCommand(antzData->io.key.map[kKeyDown][npKeyRAWASCII(event->key())], antzData);
			return;
		}
	}

	QGLWidget::keyPressEvent(event);
}

void ANTzViewerWidget::keyReleaseEvent(QKeyEvent* event) {

	if (m_selectionModel->selectedIndexes().empty()) {

		pData antzData = m_model->GetANTzData();
		if ((event->key() == 'w') ||
			(event->key() == 'W') ||
			(event->key() == 'a') ||
			(event->key() == 'A') ||
			(event->key() == 's') ||
			(event->key() == 'S') ||
			(event->key() == 'd') ||
			(event->key() == 'D') ||
			(event->key() == 'e') ||
			(event->key() == 'E') ||
			(event->key() == 'q') ||
			(event->key() == 'Q')) {

			npCtrlCommand(antzData->io.key.map[kKeyUp][npKeyRAWASCII(event->key())], antzData);
			return;
		}
	}

	QGLWidget::keyReleaseEvent(event);
}

void ANTzViewerWidget::SetStereo(bool enableStereo) {

	pData antzData = m_model->GetANTzData();
	antzData->io.gl.stereo = enableStereo;

	if ((m_zSpaceContext != nullptr) && enableStereo) {

		SetZSpacePosition();
		ResizeZSpaceViewport();
	}
}

bool ANTzViewerWidget::IsInStereoMode() const {

	pData antzData = m_model->GetANTzData();
	return (antzData->io.gl.stereo);
}

bool ANTzViewerWidget::IsStereoSupported() const {

	return (context()->format().stereo());
}

void ANTzViewerWidget::moveEvent(QMoveEvent* event) {

	SetZSpacePosition();
	QGLWidget::moveEvent(event);
}

void ANTzViewerWidget::SetZSpacePosition() {

	if (IsInZSpaceStereo()) {

		QPoint zSpaceViewportPosition = mapToGlobal(QPoint(0, 0));
		zsSetViewportPosition(m_zSpaceViewport, zSpaceViewportPosition.x(), zSpaceViewportPosition.y());

		ZSError error = zsUpdate(m_zSpaceContext);
	}
}

void ANTzViewerWidget::ResizeZSpaceViewport() {

	if (IsInZSpaceStereo()) {

		zsSetViewportSize(m_zSpaceViewport, size().width(), size().height());
		glGetFloatv(GL_PROJECTION_MATRIX, m_originialProjectionMatrix.f);

		ZSError error = zsUpdate(m_zSpaceContext);
	}
}

bool ANTzViewerWidget::eventFilter(QObject *object, QEvent *event) {

	if ((object == m_topLevelWindow) && (event->type() == QEvent::Move)) {

		SetZSpacePosition();
	}

	return false;
}

bool ANTzViewerWidget::IsInZSpaceStereo() const {

	return (IsInStereoMode() && (m_zSpaceContext != nullptr));
}