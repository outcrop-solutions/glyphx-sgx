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

ANTzWidget::ANTzWidget(QWidget *parent)
    : QGLWidget(s_format, parent),
	m_antzData(nullptr)
{
	m_antzData = static_cast<pData>(npInitData(0, NULL));
    InitIO();
	npInitFile(m_antzData);
	npInitCh(m_antzData);
	npInitCtrl(m_antzData);

	if (m_selectionModel != nullptr) {
		QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ANTzWidget::UpdateSelection);
	}

	QObject::connect(m_model, &GlyphTreeModel::rowsRemoved, this, &ANTzWidget::OnNodeDeleted);
	QObject::connect(m_model, &GlyphTreeModel::ModelChanged, this, &ANTzWidget::OnModelChanged);
	OnModelChanged();
}

ANTzWidget::~ANTzWidget()
{
	npCloseGL(m_antzData);
	npCloseCtrl(m_antzData);
	npCloseFile(m_antzData);
	npCloseCh(m_antzData);
}

void ANTzWidget::InitIO()
{
    //zz-CAMERA
    //compare intialization values with working project, debug zz

	m_antzData->io.mouse.target.x = 0.0f;
	m_antzData->io.mouse.target.y = 0.0f;
	m_antzData->io.mouse.target.z = 0.0f;

	m_antzData->io.mouse.targetDest.x = 0.0f;
	m_antzData->io.mouse.targetDest.y = 0.0f;
	m_antzData->io.mouse.targetDest.z = 0.0f;

	m_antzData->io.mouse.targetRadius = 0.0f;
	m_antzData->io.mouse.targetRadiusDest = 0.0f;

	m_antzData->io.mouse.targeting = false;

	m_antzData->io.mouse.x = 0;						//raw coordinate
	m_antzData->io.mouse.y = 0;
	m_antzData->io.mouse.z = 0;						//typically the scroll wheel

	m_antzData->io.mouse.mode = kNPmouseModeNull;
    //data->io.mouse.pickMode = kNPmodeNull;
    //	data->io.mouse.pickMode = kNPmodeCamera;
	m_antzData->io.mouse.pickMode = kNPmodePin;
	m_antzData->io.mouse.tool = kNPtoolNull;

	m_antzData->io.mouse.buttonL = false;					//true when pressed
	m_antzData->io.mouse.buttonC = false;
	m_antzData->io.mouse.buttonR = false;

	m_antzData->io.mouse.window.x = 0;					//screen position in pixels
	m_antzData->io.mouse.window.y = 0;
	m_antzData->io.mouse.window.z = 0;

	m_antzData->io.mouse.previous.x = 0;
	m_antzData->io.mouse.previous.y = 0;
	m_antzData->io.mouse.previous.z = 0;

	m_antzData->io.mouse.cmDX = 0.0f;
	m_antzData->io.mouse.cmDY = 0.0f;

	m_antzData->io.mouse.delta.x = 0.0f;					//mouse vector
	m_antzData->io.mouse.delta.y = 0.0f;					//clears every mouse event
	m_antzData->io.mouse.delta.z = 0.0f;					//z is typically scroll wheel

	m_antzData->io.mouse.deltaSum.x = 0.0f;				//clears each NPE period
	m_antzData->io.mouse.deltaSum.y = 0.0f;
	m_antzData->io.mouse.deltaSum.z = 0.0f;

	m_antzData->io.mouse.pinSelected = false;

	m_antzData->io.mouse.createEvent = false;
	m_antzData->io.mouse.singleClick = false;
	m_antzData->io.mouse.doubleClick = false;

	m_antzData->io.mouse.linkA = NULL;

	m_antzData->io.mouse.size = sizeof(NPmouse);
}

void ANTzWidget::initializeGL() {
    
	npInitGL(m_antzData);
    ResetCamera();
}

void ANTzWidget::resizeGL(int w, int h) {

    npGLResizeScene(w, h);
}

void ANTzWidget::paintGL() {

	npUpdateCh(m_antzData);

	npUpdateEngine(m_antzData);		//position, physics, interactions...

	//We may need to have a selected pin node during update to position the camera, but we don't want it during drawing
	m_antzData->map.selectedPinNode = NULL;
	m_antzData->map.selectedPinIndex = 0;

	// zero out our mouse to prevent drifting objects
	m_antzData->io.mouse.delta.x = 0.0f;
	m_antzData->io.mouse.delta.y = 0.0f;

	BeforeDrawScene();

	//antzData->io.mouse.pickMode = kNPmodePin;
	npGLDrawScene(m_antzData);
	//antzData->io.mouse.pickMode = kNPmodeNull;

	AfterDrawScene();

    int err = glGetError();
    if (err) {
        printf("err: 2388 - OpenGL error: %d\n", err);
    }

    //ANTz assumes that redraw constantly happens.  Need to put this in a thread
    update();

    //QGLWidget takes care of swapping buffers
}

void ANTzWidget::UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected) {

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

	m_antzData->map.nodeRootIndex = nodeRootIndex;
}

void ANTzWidget::ResetCamera() {
    
    pData antzData = m_model->GetANTzData();
    pNPnode root = m_model->GetRootGlyph();

    if (root != NULL) {

        CenterCameraOnNode(root);
        
    }
}

void ANTzWidget::CenterCameraOnNode(pNPnode node) {

	if (node != nullptr) {

		npSetCamTargetNode(node, m_antzData);
		m_antzData->io.mouse.targeting = false;
		m_antzData->map.selectedPinNode = node;
		m_antzData->map.selectedPinIndex = node->id;
	}

    //Always keep current node set to current cam
	m_antzData->map.currentNode = antzData->map.currentCam;
}

bool ANTzWidget::IsRootNodeSelected() const {

	const QModelIndexList& selectedIndexes = m_selectionModel->selectedIndexes();
	if (!selectedIndexes.empty()) {

		return (!selectedIndexes.back().parent().isValid());
	}

	return false;
}

void ANTzWidget::mousePressEvent(QMouseEvent* event) {

	int pickID = npPickPin(event->x(), m_antzData->io.gl.height - event->y(), antzData);

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
	m_antzData->io.mouse.mode = kNPmouseModeNull;
	m_antzData->io.mouse.tool = kNPtoolNull;
	m_antzData->io.mouse.buttonR = false;
}

void ANTzWidget::mouseMoveEvent(QMouseEvent* event) {

	m_antzData->io.mouse.previous.x = m_lastMousePosition.x();
	m_antzData->io.mouse.previous.y = m_lastMousePosition.y();
	m_antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
	m_antzData->io.mouse.delta.y = event->y() - m_lastMousePosition.y();
	m_antzData->io.mouse.x = event->x();
	m_antzData->io.mouse.y = event->y();

    if (event->modifiers() == Qt::ShiftModifier) {

		if ((m_editingMode == EditingMode::Move) && (IsRootNodeSelected())) {

			//lock move mode if root node is selected
			m_antzData->io.mouse.tool = kNPtoolNull;
		}
		else {

			m_antzData->io.mouse.tool = kNPtoolMove + m_editingMode;
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
		m_antzData->io.mouse.tool = kNPtoolNull;
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

void ANTzWidget::OnModelChanged() {

	setEnabled(m_model->rowCount() > 0);
}