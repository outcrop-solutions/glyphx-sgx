#include "antzwidget.h"

#include <QtGUI/QMouseEvent>

#include "io/npfile.h"
#include "io/npch.h"
#include "npctrl.h"
#include "io/npgl.h"
#include "npui.h"
#include "ctrl/npengine.h"

//The default QGLFormat works for now except we want alpha enabled.  May want to turn on stereo at some point
QGLFormat ANTzWidget::s_format(QGL::AlphaChannel);

ANTzWidget::ANTzWidget(GlyphTreeModel* model, QItemSelectionModel* selectionModel, bool allowMultiSelection, QWidget *parent)
    : QGLWidget(s_format, parent),
    m_model(model),
    m_selectionModel(selectionModel),
    m_editingMode(Move),
    m_selectionEdited(false),
	m_allowMultiSelection(allowMultiSelection)
{
    void* antzData = m_model->GetANTzData();
    InitIO();
    npInitFile(antzData);
    npInitCh(antzData);
    npInitCtrl(antzData);

	if (m_selectionModel != nullptr) {
		QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ANTzWidget::UpdateSelection);
	}

	QObject::connect(m_model, &GlyphTreeModel::rowsRemoved, this, &ANTzWidget::OnNodeDeleted);
}

ANTzWidget::~ANTzWidget()
{
    void* antzData = m_model->GetANTzData();
    npCloseGL(antzData);
    npCloseCtrl(antzData);
    npCloseFile(antzData);
    npCloseCh(antzData);
}

void ANTzWidget::InitIO()
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

void ANTzWidget::initializeGL() {
    
    npInitGL(m_model->GetANTzData());
    ResetCamera();
}

void ANTzWidget::resizeGL(int w, int h) {

    npGLResizeScene(w, h);
}

void ANTzWidget::paintGL() {

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

		//since changes from editing don't happen until drawing emit the ObjectEdited signal here
		if (m_selectionEdited) {
			const QModelIndexList& selected = m_selectionModel->selectedIndexes();
			if (!selected.isEmpty()) {
				
				emit ObjectEdited(selected.back());
			}
			m_selectionEdited = false;
		}
	//}

    int err = glGetError();
    if (err) {
        printf("err: 2388 - OpenGL error: %d\n", err);
    }

    //ANTz assumes that redraw constantly happens.  Need to put this in a thread
    update();

    //QGLWidget takes care of swapping buffers
}

void ANTzWidget::UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected) {

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
            CenterCameraOnNode(static_cast<pNPnode>(last.internalPointer()));
            nodeRootIndex = m_model->GetRootGlyph()->id;
        }
    }

    antzData->map.nodeRootIndex = nodeRootIndex;
}

void ANTzWidget::ResetCamera() {
    
    pData antzData = m_model->GetANTzData();
    pNPnode root = m_model->GetRootGlyph();

    if (root != NULL) {

        CenterCameraOnNode(root);
        
    }
}

void ANTzWidget::CenterCameraOnNode(pNPnode node) {
	
    pData antzData = m_model->GetANTzData();

	if (node != nullptr) {

		npSetCamTargetNode(node, antzData);
		antzData->io.mouse.targeting = false;
		antzData->map.selectedPinNode = node;
		antzData->map.selectedPinIndex = node->id;
	}

    //Always keep current node set to current cam
    antzData->map.currentNode = antzData->map.currentCam;
}

bool ANTzWidget::IsRootNodeSelected() const {

	const QModelIndexList& selectedIndexes = m_selectionModel->selectedIndexes();
	if (!selectedIndexes.empty()) {

		return (!selectedIndexes.back().parent().isValid());
	}

	return false;
}

void ANTzWidget::mousePressEvent(QMouseEvent* event) {

    pData antzData = m_model->GetANTzData();
	int pickID = npPickPin(event->x(), antzData->io.gl.height - event->y(), antzData);

	if (pickID != 0) {

		if ((event->modifiers() == Qt::ControlModifier) && m_allowMultiSelection) {

			if (event->button() == Qt::LeftButton) {

				m_selectionModel->select(m_model->IndexFromANTzID(pickID), QItemSelectionModel::Toggle);
			}
        }
		else {
				
			m_selectionModel->select(m_model->IndexFromANTzID(pickID), QItemSelectionModel::ClearAndSelect);
        }
	}

    m_lastMousePosition = event->pos();
}

void ANTzWidget::mouseReleaseEvent(QMouseEvent* event) {

    //Reset ANTz mouse values back to the original values
    pData antzData = m_model->GetANTzData();
    antzData->io.mouse.mode = kNPmouseModeNull;
    antzData->io.mouse.tool = kNPtoolNull;
    antzData->io.mouse.buttonR = false;
}

void ANTzWidget::mouseMoveEvent(QMouseEvent* event) {

    pData antzData = m_model->GetANTzData();

    antzData->io.mouse.previous.x = m_lastMousePosition.x();
    antzData->io.mouse.previous.y = m_lastMousePosition.y();
    antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
    antzData->io.mouse.delta.y = event->y() - m_lastMousePosition.y();
    antzData->io.mouse.x = event->x();
    antzData->io.mouse.y = event->y();

    if (event->modifiers() == Qt::ShiftModifier) {

		if ((m_editingMode == EditingMode::Move) && (IsRootNodeSelected())) {

			//lock move mode if root node is selected
			antzData->io.mouse.tool = kNPtoolNull;
		}
		else {

			antzData->io.mouse.tool = kNPtoolMove + m_editingMode;
			if (event->buttons() & Qt::RightButton) {
				antzData->io.mouse.mode = kNPmouseModeDragXZ;
				antzData->io.mouse.buttonR = true;
				m_selectionEdited = true;
			}
			else if (event->buttons() & Qt::LeftButton) {
				antzData->io.mouse.mode = kNPmouseModeDragXY;
				antzData->io.mouse.buttonR = false;
				m_selectionEdited = true;
			}
		}
    }
    else {
        antzData->io.mouse.tool = kNPtoolNull;
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

void ANTzWidget::SetEditingMode(EditingMode mode) {

    m_editingMode = mode;
}

void ANTzWidget::OnNodeDeleted(const QModelIndex& parent, int start, int end) {

	if (!m_selectionModel->hasSelection()) {

		if (parent.isValid()) {

			pNPnode parentNode = static_cast<pNPnode>(parent.internalPointer());
			CenterCameraOnNode(parentNode);
		}
		else {
			CenterCameraOnNode(m_model->GetRootGlyph());
		}

		pData antzData = m_model->GetANTzData();
		antzData->map.nodeRootIndex = 0;
	}
}