#include "horizontalformwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

namespace SynGlyphX {

	HorizontalFormWidget::HorizontalFormWidget(QWidget *parent)
		: QWidget(parent)
	{
		QHBoxLayout* layout = new QHBoxLayout(this);
		//setContentsMargins(0, 0, 0, 0);
		//layout->setContentsMargins(0, 0, 0, 0);
		setLayout(layout);
	}

	HorizontalFormWidget::~HorizontalFormWidget()
	{

	}

	void HorizontalFormWidget::AddWidget(const QString& labelText, QWidget* widget) {

		QLayout* currentLayout = layout();

		QLabel* label = new QLabel(labelText, this);
		currentLayout->addWidget(label);
		currentLayout->addWidget(widget);

		currentLayout->invalidate();
	}
}