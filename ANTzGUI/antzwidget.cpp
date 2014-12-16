#include "antzwidget.h"
#include "io/npfile.h"
#include "io/npch.h"
#include "npctrl.h"
#include "npui.h"
#include "ctrl/npengine.h"
#include "io/npgl.h"
#include "io/gl/nptags.h"

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
}

ANTzWidget::~ANTzWidget()
{
	npCloseGL(m_antzData);
	npCloseCtrl(m_antzData);
	npCloseFile(m_antzData);
	npCloseCh(m_antzData);
	npCloseData();
	m_antzData = nullptr;
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
    
	npInitGLDraw(m_antzData);
	npInitGLPrimitive(m_antzData);
	npInitTags(m_antzData);
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

void ANTzWidget::ResetCamera() {

	npDataCameraPreset(-1, m_antzData->map.currentCam, m_antzData);
}

void ANTzWidget::CenterCameraOnNode(pNPnode node) {

	if (node != nullptr) {

		npSetCamTargetNode(node, m_antzData);
		m_antzData->io.mouse.targeting = false;
		m_antzData->map.selectedPinNode = node;
		m_antzData->map.selectedPinIndex = node->id;
	}

    //Always keep current node set to current cam
	m_antzData->map.currentNode = m_antzData->map.currentCam;
}

int ANTzWidget::PickPinAtPoint(const QPoint& point) const {

	return npPickPin(point.x(), m_antzData->io.gl.height - point.y(), m_antzData);
}

void ANTzWidget::DeleteNode(pNPnode node) {

	npNodeRemove(true, node, m_antzData);
}

void ANTzWidget::DeleteChildren(pNPnode parent, unsigned int first, unsigned int count) {

	if (count > 0) {

		for (int i = first + count - 1; i >= static_cast<int>(first); --i) {

			npNodeRemove(true, parent->child[i], m_antzData);
		}
	}
}

unsigned int ANTzWidget::BindTextureInFile(const QString& imageFilename) {

	QImage image(imageFilename);
	return bindTexture(image);
}