#include "gridwidget.h"
#include <QtWidgets/QGridLayout>
#include <QtGui/QPaintEvent>

namespace SynGlyphX {

	GridWidget::GridWidget(unsigned int rowCount, unsigned int columnCount, QWidget *parent)
		: QWidget(parent)
	{
		QGridLayout* gridLayout = new QGridLayout(this);
		gridLayout->setHorizontalSpacing(0);
		gridLayout->setVerticalSpacing(0);

		for (unsigned int y = 0; y < rowCount - 1; ++y) {

			for (unsigned int x = 0; x < columnCount - 1; ++x) {
			

			}
		
		}
	}

	GridWidget::~GridWidget()
	{

	}

	void GridWidget::SetColumnStretch(int column, int stretch) {

		QGridLayout* gridLayout = dynamic_cast<QGridLayout*>(layout());
		gridLayout->setColumnStretch(2 * column, stretch);
	}

	void GridWidget::AddWidget(QWidget* widget, int row, int column) {

		QGridLayout* gridLayout = dynamic_cast<QGridLayout*>(layout());
		gridLayout->addWidget(widget, 2 * row, 2 * column);
	}

	/*void GridWidget::AddWidget(QWidget* widget, int fromRow, int fromColumn, int rowSpan, int columnSpan) {

		QGridLayout* gridLayout = dynamic_cast<QGridLayout*>(layout());
		gridLayout->addWidget(widget, fromRow, fromColumn, rowSpan, columnSpan);
	}

	void GridWidget::paintEvent(QPaintEvent *event) {

		QWidget::paintEvent(event);

		QGridLayout* gridLayout = dynamic_cast<QGridLayout*>(layout());
		for (unsigned int x = 0; x < gridLayout->columnCount() - 1; ++x) {

			for (unsigned int y = 0; y < gridLayout->rowCount() - 1; ++y) {


			}
		}
	}*/

} //namespace SynGlyphX