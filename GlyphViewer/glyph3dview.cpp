#include "glyph3dview.h"
#include <QtGui/QResizeEvent>
#include "npctrl.h"

const unsigned int Glyph3DView::s_navigationButtonSize = 24;

Glyph3DView::Glyph3DView(SynGlyphXANTz::GlyphForestModel* model, SynGlyphX::ItemFocusSelectionModel* selectionModel, QWidget *parent)
	: SynGlyphXANTz::ANTzForestWidget(model, selectionModel, parent)
{
	QObject::connect(selectionModel, &SynGlyphX::ItemFocusSelectionModel::FocusChanged, this, &Glyph3DView::ChangeNavButtonMode);

	m_upRotateButton = CreateNavigationButton(tr("Rotate Up"), true);
	m_upRotateButton->setIcon(QIcon(":GlyphViewer/Resources/rotate_up.png"));
	QObject::connect(m_upRotateButton, &QToolButton::pressed, this, &Glyph3DView::OnUpRotate);

	m_leftRotateButton = CreateNavigationButton(tr("Rotate Left"), true);
	m_leftRotateButton->setIcon(QIcon(":GlyphViewer/Resources/rotate_left.png"));
	QObject::connect(m_leftRotateButton, &QToolButton::pressed, this, &Glyph3DView::OnLeftRotate);

	m_rightRotateButton = CreateNavigationButton(tr("Rotate Right"), true);
	m_rightRotateButton->setIcon(QIcon(":GlyphViewer/Resources/rotate_right.png"));
	QObject::connect(m_rightRotateButton, &QToolButton::pressed, this, &Glyph3DView::OnRightRotate);

	m_downRotateButton = CreateNavigationButton(tr("Rotate Down"), true);
	m_downRotateButton->setIcon(QIcon(":GlyphViewer/Resources/rotate_down.png"));
	QObject::connect(m_downRotateButton, &QToolButton::pressed, this, &Glyph3DView::OnDownRotate);

	m_moveForwardButton = CreateNavigationButton(tr("Move Forward"), false);
	m_moveForwardButton->setIcon(QIcon(":SGXGUI/Resources/plus.png"));
	QObject::connect(m_moveForwardButton, &QToolButton::pressed, this, &Glyph3DView::OnMoveForward);

	m_moveBackwardButton = CreateNavigationButton(tr("Move Backward"), false);
	m_moveBackwardButton->setIcon(QIcon(":SGXGUI/Resources/minus.png"));
	QObject::connect(m_moveBackwardButton, &QToolButton::pressed, this, &Glyph3DView::OnMoveBackward);

	m_moveUpButton = CreateNavigationButton(tr("Move Up"), false);
	m_moveUpButton->setIcon(QIcon(":SGXGUI/Resources/up_arrow.png"));
	QObject::connect(m_moveUpButton, &QToolButton::pressed, this, [&, this](){ OnMoveForwardBackwardUpDown('e'); });

	m_moveDownButton = CreateNavigationButton(tr("Move Down"), false);
	m_moveDownButton->setIcon(QIcon(":SGXGUI/Resources/down_arrow.png"));
	QObject::connect(m_moveDownButton, &QToolButton::pressed, this, [&, this](){ OnMoveForwardBackwardUpDown('q'); });
}

Glyph3DView::~Glyph3DView()
{

}

QToolButton* Glyph3DView::CreateNavigationButton(const QString& toolTip, bool autoRepeat) {

	QToolButton* navButton = new QToolButton(this);
	navButton->setFixedSize(s_navigationButtonSize, s_navigationButtonSize);
	navButton->setAutoRepeat(autoRepeat);
	navButton->setToolTip(toolTip);
	navButton->setAutoRepeatDelay(100);
	navButton->setAutoRepeatInterval(50);
	QObject::connect(navButton, &QToolButton::released, this, &Glyph3DView::OnNavigationButtonReleased);

	return navButton;
}

void Glyph3DView::resizeEvent(QResizeEvent* event) {

	SynGlyphXANTz::ANTzForestWidget::resizeEvent(event);

	unsigned int leftPosOfButtonsInHCenter = event->size().width() - 10 - (2 * s_navigationButtonSize);
	unsigned int topPositionOfButton = std::abs(m_logoPosition.height()) + (2 * 10);

	m_upRotateButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

	topPositionOfButton += s_navigationButtonSize;
	m_leftRotateButton->move(QPoint(leftPosOfButtonsInHCenter - s_navigationButtonSize, topPositionOfButton));
	m_rightRotateButton->move(QPoint(leftPosOfButtonsInHCenter + s_navigationButtonSize, topPositionOfButton));

	topPositionOfButton += s_navigationButtonSize;
	m_downRotateButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

	topPositionOfButton += (2 * s_navigationButtonSize);
	m_moveForwardButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

	topPositionOfButton += s_navigationButtonSize;
	m_moveBackwardButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

	topPositionOfButton += (2 * s_navigationButtonSize);
	m_moveUpButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

	topPositionOfButton += s_navigationButtonSize;
	m_moveDownButton->move(QPoint(leftPosOfButtonsInHCenter, topPositionOfButton));

	auto button_tex = [this]( const char* name ){
		return bindTexture( QImage( name ) );
	};

#ifdef __APPLE__
	// Hack to fix other widgets failing to render on top of the QGLWidget on OSX: just draw the nav buttons ourselves with GL.
	// (Eventually we should move to QOpenGLWidget since QGLWidget is deprecated, which also should fix the issue and make this
	// hack unnecessary.)
	ClearRenderedIcons();
#endif // __APPLE__
}

void Glyph3DView::OnUpRotate() {

	pData antzData = m_antzData->GetData();
	antzData->io.mouse.previous.x = 0;
	antzData->io.mouse.previous.y = 10;
	antzData->io.mouse.delta.x = 0;
	antzData->io.mouse.delta.y = -10;
	antzData->io.mouse.x = 0;
	antzData->io.mouse.y = 0;
	if (m_moveUpButton->isEnabled()) {

		antzData->io.mouse.mode = kNPmouseModeCamLook;
	}
	else {

		antzData->io.mouse.mode = kNPmouseModeCamExamXY;
	}
}

void Glyph3DView::OnLeftRotate()  {

	pData antzData = m_antzData->GetData();
	antzData->io.mouse.previous.x = 10;
	antzData->io.mouse.previous.y = 0;
	antzData->io.mouse.delta.x = -10;
	antzData->io.mouse.delta.y = 0;
	antzData->io.mouse.x = 0;
	antzData->io.mouse.y = 0;
	if (m_moveUpButton->isEnabled()) {

		antzData->io.mouse.mode = kNPmouseModeCamLook;
	}
	else {

		antzData->io.mouse.mode = kNPmouseModeCamExamXY;
	}
}

void Glyph3DView::OnRightRotate() {

	pData antzData = m_antzData->GetData();
	antzData->io.mouse.previous.x = 0;
	antzData->io.mouse.previous.y = 0;
	antzData->io.mouse.delta.x = 10;
	antzData->io.mouse.delta.y = 0;
	antzData->io.mouse.x = 10;
	antzData->io.mouse.y = 0;
	if (m_moveUpButton->isEnabled()) {

		antzData->io.mouse.mode = kNPmouseModeCamLook;
	}
	else {

		antzData->io.mouse.mode = kNPmouseModeCamExamXY;
	}
}

void Glyph3DView::OnDownRotate() {

	pData antzData = m_antzData->GetData();
	antzData->io.mouse.previous.x = 0;
	antzData->io.mouse.previous.y = 0;
	antzData->io.mouse.delta.x = 0;
	antzData->io.mouse.delta.y = 10;
	antzData->io.mouse.x = 0;
	antzData->io.mouse.y = 10;
	if (m_moveUpButton->isEnabled()) {

		antzData->io.mouse.mode = kNPmouseModeCamLook;
	}
	else {

		antzData->io.mouse.mode = kNPmouseModeCamExamXY;
	}
}

void Glyph3DView::OnMoveForward() {

	if (m_moveUpButton->isEnabled()) {

		OnMoveForwardBackwardUpDown('w');
	}
	else {

		pData antzData = m_antzData->GetData();
		antzData->io.mouse.previous.x = 0;
		antzData->io.mouse.previous.y = 10;
		antzData->io.mouse.delta.x = 0;
		antzData->io.mouse.delta.y = -10;
		antzData->io.mouse.x = 0;
		antzData->io.mouse.y = 0;
		antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
	}
}

void Glyph3DView::OnMoveBackward() {

	if (m_moveUpButton->isEnabled()) {

		OnMoveForwardBackwardUpDown('s');
	}
	else {

		pData antzData = m_antzData->GetData();
		antzData->io.mouse.previous.x = 0;
		antzData->io.mouse.previous.y = 0;
		antzData->io.mouse.delta.x = 0;
		antzData->io.mouse.delta.y = 10;
		antzData->io.mouse.x = 0;
		antzData->io.mouse.y = 10;
		antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
	}
}

void Glyph3DView::OnMoveForwardBackwardUpDown(char wasd) {

	pData antzData = m_antzData->GetData();
	npCtrlCommand(antzData->io.key.map[kKeyDown][npKeyRAWASCII(wasd)], antzData);
}

void Glyph3DView::OnStopMoveForwardBackwardUpDown(char wasd) {

	pData antzData = m_antzData->GetData();
	npCtrlCommand(antzData->io.key.map[kKeyUp][npKeyRAWASCII(wasd)], antzData);
}

void Glyph3DView::OnNavigationButtonReleased() {

	QToolButton* toolButton = dynamic_cast<QToolButton*>(sender());

	//Since nav buttons are set to auto repeat only execute this code if we know the button has been truly released
	if ((toolButton != nullptr) && (!toolButton->isDown())) {

		if (toolButton == m_moveUpButton) {

			OnStopMoveForwardBackwardUpDown('e');
		}
		else if (toolButton == m_moveDownButton) {

			OnStopMoveForwardBackwardUpDown('q');
		}
		else if (m_moveUpButton->isEnabled() && ((toolButton == m_moveForwardButton) || (toolButton == m_moveBackwardButton))) {

			if (toolButton == m_moveForwardButton) {

				OnStopMoveForwardBackwardUpDown('w');
			}
			else {

				OnStopMoveForwardBackwardUpDown('s');
			}
		}
		else {

			mouseReleaseEvent(nullptr);
		}
	}
}

void Glyph3DView::ChangeNavButtonMode(const QModelIndexList& indexes) {

	bool isInExplorerMode = indexes.empty();

	m_moveUpButton->setEnabled(isInExplorerMode);
	m_moveDownButton->setEnabled(isInExplorerMode);

	m_moveForwardButton->setAutoRepeat(!isInExplorerMode);
	m_moveBackwardButton->setAutoRepeat(!isInExplorerMode);
}