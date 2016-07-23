#include "verticaltabordertableview.h"
#include <QtWidgets/QApplication>

namespace SynGlyphX {

	VerticalTabOrderTableView::VerticalTabOrderTableView(QWidget *parent)
		: QTableView(parent)
	{
		
	}

	VerticalTabOrderTableView::~VerticalTabOrderTableView()
	{

	}

	bool VerticalTabOrderTableView::focusNextPrevChild(bool next) {

		QWidget* widgetWithFocus = QApplication::focusWidget()->parentWidget();
		
		if (m_widgetToModelIndexMap.count(widgetWithFocus) > 0) {

			QModelIndex index = m_widgetToModelIndexMap.at(widgetWithFocus);
			if (index.isValid()) {

				QAbstractItemModel* currentModel = model();
				unsigned int row = index.row();
				unsigned int column = index.column();

				if (next) {

					if (row == (currentModel->rowCount() - 1)) {

						if (column < (currentModel->columnCount() - 1)) {

							QWidget* widget = indexWidget(currentModel->index(0, column + 1));
							if (widget != nullptr) {

								widget->setFocus();
								return true;
							}
						}
					}
					else {

						QWidget* widget = indexWidget(currentModel->index(row + 1, column));
						if (widget != nullptr) {

							widget->setFocus();
							return true;
						}
					}
				}
				else {

					if (row == 0) {

						if (column > 0) {

							QWidget* widget = indexWidget(currentModel->index(currentModel->rowCount() - 1, column - 1));
							if (widget != nullptr) {

								widget->setFocus();
								return true;
							}
						}
					}
					else {

						QWidget* widget = indexWidget(currentModel->index(row - 1, column));
						if (widget != nullptr) {

							widget->setFocus();
							return true;
						}
					}
				}
			}
		}

		return QTableView::focusNextPrevChild(next);
	}

	void VerticalTabOrderTableView::setIndexWidget(const QModelIndex& index, QWidget* widget) {

		m_widgetToModelIndexMap[widget] = QPersistentModelIndex(index);
		widget->installEventFilter(this);
		QTableView::setIndexWidget(index, widget);
	}

	/*bool VerticalTabOrderTableView::eventFilter(QObject* watched, QEvent* event) {

		if (event->type() == QEvent::FocusIn) {
		
			if (m_widgetToModelIndexMap.count(watched) > 0) {

				setCurrentIndex(m_widgetToModelIndexMap.at(watched));
			}
		}
		return false;
	}*/

} //namespace SynGlyphX