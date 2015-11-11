#include "colorbutton.h"
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidgetAction>

namespace SynGlyphX {

	ColorButton::ColorButton(bool showAlphaInColorDialog, QWidget *parent)
		: QToolButton(parent),
		m_showAlphaInColorDialog(showAlphaInColorDialog)
	{
		setAutoFillBackground(true);
		setPopupMode(QToolButton::InstantPopup);

		m_menu = new QMenu(this);

		QAction* action = m_menu->addAction(CreateColorIcon(Qt::white, QSize(16, 16)), tr("White"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(Qt::white); });
		action = m_menu->addAction(CreateColorIcon(Qt::black, QSize(16, 16)), tr("Black"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(Qt::black); });
		action = m_menu->addAction(CreateColorIcon(Qt::red, QSize(16, 16)), tr("Red"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(Qt::red); });
		action = m_menu->addAction(CreateColorIcon(Qt::green, QSize(16, 16)), tr("Green"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(Qt::green); });
		action = m_menu->addAction(CreateColorIcon(Qt::blue, QSize(16, 16)), tr("Blue"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(Qt::blue); });
		action = m_menu->addAction(CreateColorIcon(Qt::cyan, QSize(16, 16)), tr("Cyan"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(Qt::cyan); });
		action = m_menu->addAction(CreateColorIcon(Qt::magenta, QSize(16, 16)), tr("Magenta"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(Qt::magenta); });
		action = m_menu->addAction(CreateColorIcon(Qt::yellow, QSize(16, 16)), tr("Yellow"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(Qt::yellow); });
		action = m_menu->addAction(CreateColorIcon(Qt::gray, QSize(16, 16)), tr("Gray"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(Qt::gray); });

		action = m_menu->addAction(CreateColorIcon(QColor(128, 0, 128), QSize(16, 16)), tr("Purple"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(QColor(128, 0, 128)); });
		action = m_menu->addAction(CreateColorIcon(QColor(255, 127, 0), QSize(16, 16)), tr("Orange"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(QColor(255, 127, 0)); });
		action = m_menu->addAction(CreateColorIcon(QColor(155, 75, 0), QSize(16, 16)), tr("Brown"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(QColor(155, 75, 0)); });
		action = m_menu->addAction(CreateColorIcon(QColor(255, 203, 219), QSize(16, 16)), tr("Pink"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(QColor(255, 145, 175)); });
		action = m_menu->addAction(CreateColorIcon(QColor(255, 215, 0), QSize(16, 16)), tr("Gold"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(QColor(255, 215, 0)); });
		action = m_menu->addAction(CreateColorIcon(QColor(192, 192, 192), QSize(16, 16)), tr("Silver"));
		QObject::connect(action, &QAction::triggered, this, [this]{ SetColorFromMenu(QColor(192, 192, 192)); });

		m_menu->addSeparator();
		QAction* customColorAction = m_menu->addAction(tr("Custom Color"));

		QObject::connect(customColorAction, &QAction::triggered, this, &ColorButton::OnCustomColor);

		setMenu(m_menu);

		SetColor(Qt::gray);
	}

	ColorButton::~ColorButton()
	{

	}

	QIcon ColorButton::CreateColorIcon(const QColor& color, const QSize& size) {

		QPixmap pixmap(size);
		pixmap.fill(color);
		QIcon icon;
		icon.addPixmap(pixmap, QIcon::Normal);
		icon.addPixmap(pixmap, QIcon::Disabled);
		icon.addPixmap(pixmap, QIcon::Active);
		icon.addPixmap(pixmap, QIcon::Selected);
		return icon;
	}

	void ColorButton::SetColorFromMenu(const QColor& color) {

		SetColor(color);
		emit ColorChanged(m_color);
	}

	void ColorButton::SetColor(const QColor& color) {

		m_color = color;
		UpdateButtonColor();
	}

	const QColor& ColorButton::GetColor() const {
		return m_color;
	}

	void ColorButton::UpdateButtonColor() {

		QColor color = m_color;
		color.setAlpha(255);
		setIcon(CreateColorIcon(color, iconSize()));
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

	QSize ColorButton::minimumSizeHint() const {

		QSize size = QToolButton::minimumSizeHint();
		size.setWidth(1.75 * size.width());
		return size;
	}

} //namespace SynGlyphX