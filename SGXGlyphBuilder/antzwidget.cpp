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

ANTzWidget::ANTzWidget(GlyphTreeModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
    : QGLWidget(s_format, parent),
    m_model(model),
    m_selectionModel(selectionModel)
{
    void* antzData = m_model->GetANTzData();
    InitIO();
    npInitFile(antzData);
    npInitCh(antzData);
    npInitCtrl(antzData);

    QObject::connect(m_selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(UpdateSelection(const QItemSelection&, const QItemSelection&)));

    //setMouseTracking(true);
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
    //	data->io.mouse.pickMode = kNPmodeNull;
    //	data->io.mouse.pickMode = kNPmodeCamera;
    data->io.mouse.pickMode = kNPmodePin;
    data->io.mouse.tool = kNPtoolCombo;

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

    pData antzData = m_model->GetANTzData();

    npUpdateCh(antzData);
    
    npUpdateEngine(antzData);		//position, physics, interactions...

    // zero out our mouse to prevent drifting objects
    antzData->io.mouse.delta.x = 0.0f;											//debug, zz
    antzData->io.mouse.delta.y = 0.0f;

    npGLDrawScene(antzData);

    int err = glGetError();
    if (err) {
        printf("err: 2388 - OpenGL error: %d\n", err);
    }

    //ANTz assumes that redraw constantly happens.  Need to put this in a thread
    update();

    //QGLWidget takes care of swapping buffers
}

void ANTzWidget::UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected) {

    void* antzData = m_model->GetANTzData();
    const QModelIndexList& indicies = selected.indexes();
    if ((indicies.length() > 0) && (indicies[0].isValid())) {
        npSelectNode(static_cast<pNPnode>(indicies[0].internalPointer()), antzData);
    }
    else {
        npSelectNode(NULL, antzData);
    }
    npSetCamTarget(antzData);
    //update();
}

void ANTzWidget::ResetCamera() {
    
    pData antzData = m_model->GetANTzData();
    //npSelectNode(m_model->GetRootGlyph(), m_model->GetANTzData());
    pNPnode root = m_model->GetRootGlyph();

    if (root != NULL) {
        int rootIndex = root->id;
        antzData->map.selectedPinNode = root;
        antzData->map.currentNode = root;
        antzData->map.nodeRootIndex = rootIndex;
        antzData->map.selectedPinIndex = rootIndex;
        npSetCamTarget(antzData);
    }
}

void ANTzWidget::mousePressEvent(QMouseEvent* event) {

    if (event->button() == Qt::LeftButton) {
        pData antzData = m_model->GetANTzData();
        npPick(event->x(), antzData->io.gl.height - event->y(), antzData);

        if (antzData->io.gl.pickID != 0) {
            m_selectionModel->select(m_model->IndexFromANTzID(antzData->io.gl.pickID), QItemSelectionModel::ClearAndSelect);
        }
    }

    m_lastMousePosition = event->pos();
}

void ANTzWidget::mouseReleaseEvent(QMouseEvent* event) {

    pData antzData = m_model->GetANTzData();
    antzData->io.mouse.mode = kNPmouseModeNull;
}

void ANTzWidget::mouseMoveEvent(QMouseEvent* event) {

    pData antzData = m_model->GetANTzData();
        
    //npSelectNode(antzData->map.currentCam, antzData);
    antzData->map.currentNode = antzData->map.currentCam;

    antzData->io.mouse.previous.x = m_lastMousePosition.x();
    antzData->io.mouse.previous.y = m_lastMousePosition.y();
    antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
    antzData->io.mouse.delta.y = event->y() - m_lastMousePosition.y();
    antzData->io.mouse.x = event->x();
    antzData->io.mouse.y = event->y();

    if (event->buttons() & Qt::LeftButton) {
        antzData->io.mouse.mode = kNPmouseModeCamExamXY;
    }
    else if (event->buttons() & Qt::RightButton) {
        antzData->io.mouse.mode = kNPmouseModeCamFlyA;
    }

    m_lastMousePosition = event->pos();
}