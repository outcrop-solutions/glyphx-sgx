#include "antzviewerwidget.h"

#include <QtGUI/QMouseEvent>

#include "io/npfile.h"
#include "io/npch.h"
#include "npctrl.h"
#include "io/npgl.h"
#include "npui.h"
#include "ctrl/npengine.h"
#include "npctrl.h"

//The default QGLFormat works for now except we want alpha enabled.  May want to turn on stereo at some point
QGLFormat ANTzViewerWidget::s_format(QGL::AlphaChannel);

ANTzViewerWidget::ANTzViewerWidget(GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
    : QGLWidget(s_format, parent),
	m_model(model),
    m_selectionModel(selectionModel)
{
	setFocusPolicy(Qt::StrongFocus);

    pData antzData = m_model->GetANTzData();
    InitIO();
    npInitFile(antzData);
    npInitCh(antzData);
    npInitCtrl(antzData);

	//Change fly speeds
	//antzData->ctrl.slow = 0.5f;
	//antzData->ctrl.fast = 0.75f;

	if (m_selectionModel != nullptr) {
		QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ANTzViewerWidget::UpdateSelection);
	}
}

ANTzViewerWidget::~ANTzViewerWidget()
{
    void* antzData = m_model->GetANTzData();
    npCloseGL(antzData);
    npCloseCtrl(antzData);
    npCloseFile(antzData);
    npCloseCh(antzData);
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

    npGLResizeScene(w, h);
}

void ANTzViewerWidget::paintGL() {

	//if (m_model->GetRootGlyph() != nullptr) {
		pData antzData = m_model->GetANTzData();

		npUpdateCh(antzData);

		npUpdateEngine(antzData);		//position, physics, interactions...



		//We may need to have a selected pin node during update to position the camera, but we don't want it during drawing
		antzData->map.selectedPinNode = NULL;
		antzData->map.selectedPinIndex = 0;

		// zero out our mouse to prevent drifting objects
		antzData->io.mouse.delta.x = 0.0f;
		antzData->io.mouse.delta.y = 0.0f;

		//antzData->io.mouse.pickMode = kNPmodePin;
		npGLDrawScene(antzData);
		//antzData->io.mouse.pickMode = kNPmodeNull;
	//}

    int err = glGetError();
    if (err) {
        printf("err: 2388 - OpenGL error: %d\n", err);
    }

    //ANTz assumes that redraw constantly happens.  Need to put this in a thread
    update();

    //QGLWidget takes care of swapping buffers
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

    antzData->map.nodeRootIndex = nodeRootIndex;
}

void ANTzViewerWidget::ResetCamera() {
    
    pData antzData = m_model->GetANTzData();

	//We only want to center the camera when there are actual root nodes
	if (antzData->map.nodeRootCount > kNPnodeRootPin) {

		CenterCameraOnNode(static_cast<pNPnode>(antzData->map.node[kNPnodeRootPin]));
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
				
			m_selectionModel->select(m_model->IndexFromANTzID(pickID), QItemSelectionModel::ClearAndSelect);
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