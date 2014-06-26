#include "colorbutton.h"
#include <QtWidgets/QColorDialog>

namespace SynGlyphX {

	ColorButton::ColorButton(bool showAlphaInColorDialog, QWidget *parent)
		: QToolButton(parent),
		m_showAlphaInColorDialog(showAlphaInColorDialog)
	{
		setAutoFillBackground(true);
		setPopupMode(QToolButton::InstantPopup);

		m_menu = new QMenu(this);

		m_menu->addSeparator();
		QAction* customColorAction = m_menu->addAction(tr("Custom Color"));

		QObject::connect(customColorAction, &QAction::triggered, this, &ColorButton::OnCustomColor);

		setMenu(m_menu);

		SetColor(Qt::gray);
	}

	ColorButton::~ColorButton()
	{

	}

	void ColorButton::SetColor(const QColor& color) {

		m_color = color;
		UpdateButtonColor();
	}

	void ColorButton::SetColor(const SynGlyphX::Color& color) {

		m_color.setRed(color[0]);
		m_color.setGreen(color[1]);
		m_color.setBlue(color[2]);
		//m_color.setAlpha(color[3]);

		UpdateButtonColor();
	}

	const QColor& ColorButton::GetColor() const {
		return m_color;
	}

	void ColorButton::UpdateButtonColor() {

		QColor color = m_color;
		color.setAlpha(255);
		QPixmap pixmap(iconSize());
		pixmap.fill(color);
		setIcon(QIcon(pixmap));
	}

	void ColorButton::OnCustomColor() {

		QColorDialog colorDialog(this);
		colorDialog.setOption(QColorDialog::ShowAlphaChannel, m_showAlphaInColorDialog);
		colorDialog.setCurrentColor(m_color);
		if (colorDialog.exec() == QDialog::Accepted) {
			QColor newColor = colorDialog.currentColor();
			if (m_color != newColor) {
				SetColor(newColor);
				emit ColorChanged(m_color);
			}
		}
	}

	SynGlyphX::Color ColorButton::ConvertQColorToColor(const QColor& qColor) {

		SynGlyphX::Color color;

		color[0] = qColor.red();
		color[1] = qColor.green();
		color[2] = qColor.blue();
		color[3] = qColor.alpha();

		return color;
	}

	QSize ColorButton::minimumSizeHint() const {

		QSize size = QToolButton::minimumSizeHint();
		size.setWidth(1.75 * size.width());
		return size;
	}

} //namespace SynGlyphX