#include "stretchsurroundedwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace SynGlyphX {

	StretchSurroundedWidget::StretchSurroundedWidget(StretchType stretchType, QWidget* innerWidget, QWidget *parent)
		: QWidget(parent)
	{
		innerWidget->setParent(this);

		QVBoxLayout* verticalLayout = new QVBoxLayout(this);

		if (stretchType & StretchType::Vertical) {

			verticalLayout->addStretch(1);
		}

		QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
		if (stretchType & StretchType::Horizontal) {

			horizontalLayout->addStretch(1);
		}
		horizontalLayout->addWidget(innerWidget);
		if (stretchType & StretchType::Horizontal) {

			horizontalLayout->addStretch(1);
		}
		verticalLayout->addLayout(horizontalLayout);

		if (stretchType & StretchType::Vertical) {

			verticalLayout->addStretch(1);
		}

		setLayout(verticalLayout);
	}

	StretchSurroundedWidget::~StretchSurroundedWidget()
	{

	}

} //namespace SynGlyphX