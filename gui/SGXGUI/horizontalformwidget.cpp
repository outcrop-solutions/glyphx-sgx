#include "horizontalformwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QApplication>
#include <QtGUi/QFocusEvent>

namespace SynGlyphX {

	HorizontalFormWidget::HorizontalFormWidget(QWidget *parent)
		: QWidget(parent)
	{
		setFocusPolicy(Qt::StrongFocus);

		QHBoxLayout* layout = new QHBoxLayout(this);
		//setContentsMargins(0, 0, 0, 0);
		//layout->setContentsMargins(0, 0, 0, 0);
		setLayout(layout);
	}

	HorizontalFormWidget::~HorizontalFormWidget()
	{

	}

	void HorizontalFormWidget::AddWidget(const QString& labelText, QWidget* widget) {

		QHBoxLayout* currentLayout = dynamic_cast<QHBoxLayout*>(layout());

		//unsigned int widgetCount = currentLayout->count();
		//if (widgetCount == 0) {

		//	setFocusProxy(widget);
		//}

		QLabel* label = new QLabel(labelText, this);
		currentLayout->addWidget(label);
		currentLayout->addWidget(widget, 1);
		
		currentLayout->invalidate();
	}

	bool HorizontalFormWidget::focusNextPrevChild(bool next) {

		QHBoxLayout* currentLayout = dynamic_cast<QHBoxLayout*>(layout());

		int focusWidgetIndex = currentLayout->indexOf(QApplication::focusWidget());
		if (focusWidgetIndex != -1) {

			if (next) {

				focusWidgetIndex += 2;
				if (focusWidgetIndex < currentLayout->count()) {

					currentLayout->itemAt(focusWidgetIndex)->widget()->setFocus(Qt::TabFocusReason);
					return true;
				}
			}
			else {

				focusWidgetIndex -= 2;
				if (focusWidgetIndex > 0) {

					currentLayout->itemAt(focusWidgetIndex)->widget()->setFocus(Qt::BacktabFocusReason);
					return true;
				}
			}
		}
		
		return QWidget::focusNextPrevChild(next);
	}

	void HorizontalFormWidget::focusInEvent(QFocusEvent* event) {

		if (layout()->count() >= 2) {

			if (event->reason() == Qt::FocusReason::TabFocusReason) {

				layout()->itemAt(1)->widget()->setFocus(Qt::FocusReason::TabFocusReason);
				event->accept();
			}
			else if (event->reason() == Qt::FocusReason::BacktabFocusReason) {

				layout()->itemAt(layout()->count() - 1)->widget()->setFocus(Qt::FocusReason::BacktabFocusReason);
				event->accept();
			}
		}
		
		QWidget::focusInEvent(event);
	}
}