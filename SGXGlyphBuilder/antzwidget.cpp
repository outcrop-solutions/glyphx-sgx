#include "antzwidget.h"

#include "io/npfile.h"
#include "io/npch.h"
#include "npctrl.h"
#include "io/npgl.h"
#include "npui.h"

//The default QGLFormat works for now except we want alpha enabled.  May want to turn on stereo at some point
QGLFormat ANTzWidget::s_format(QGL::AlphaChannel);

ANTzWidget::ANTzWidget(GlyphTreeModel* model, QWidget *parent)
    : QGLWidget(s_format, parent),
    m_model(model)
{
    void* antzData = m_model->GetANTzData();
    InitIO();
    npInitFile(antzData);
    npInitCh(antzData);
    npInitCtrl(antzData);
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
    data->io.mouse.pickMode = kNPmodePin;	//needs to match npInitTools, zz debug
    data->io.mouse.tool = kNPtoolCombo;		//needs to match npInitTools, zz debug

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

    void* antzData = m_model->GetANTzData();
    npUpdateCtrl(antzData);
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
    if (!selected.indexes()[0].isValid()) {
        npSelectNode(NULL, antzData);
    }
    else {
        npSelectNode(static_cast<pNPnode>(selected.indexes()[0].internalPointer()), antzData);
    }
    npSetCamTarget(antzData);
    //update();
}

void ANTzWidget::ResetCamera() {
    pData data = (pData)m_model->GetANTzData();
    //npSelectNode(m_model->GetRootGlyph(), m_model->GetANTzData());
    int rootIndex = npGetRootIndex(m_model->GetRootGlyph(), data);
    //data->map.selectedPinIndex = rootIndex;
    data->map.selectedPinNode = m_model->GetRootGlyph();
    data->map.currentNode = m_model->GetRootGlyph();
    data->map.nodeRootIndex = rootIndex;
    npSetCamTarget(data);
}