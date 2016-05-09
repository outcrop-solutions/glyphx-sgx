#include "vscrollgridwidget.h"
#include <QtGui/QStandardItemModel>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QVBoxLayout>

namespace SynGlyphX {

	VScrollGridWidget::VScrollGridWidget(const QStringList& headerLabels, QWidget *parent)
		: VerticalScrollArea(parent),
		m_rowCountIncludingLines(0)
	{
		QStandardItemModel* headerModel = new QStandardItemModel(0, headerLabels.count(), this);
		headerModel->setHorizontalHeaderLabels(headerLabels);

		m_columnMinimumWidths.resize(headerLabels.count() - 1, 0);

		m_headerView = new QHeaderView(Qt::Horizontal, this);
		m_headerView->setModel(headerModel);
		m_headerView->move(0, 0);
		m_headerView->setStretchLastSection(true);
		ResetHeaderSectionSizesToContents();

		setViewportMargins(0, m_headerView->sizeHint().height(), 0, 0);

		QWidget* viewportWidget = new QWidget(this);
		QVBoxLayout* viewportLayout = new QVBoxLayout(viewportWidget);
		viewportLayout->setContentsMargins(0, 0, 0, 0);
		
		m_scrollAreaLayout = new QGridLayout(viewportWidget);
		m_scrollAreaLayout->setHorizontalSpacing(0);
		m_scrollAreaLayout->setVerticalSpacing(0);

		viewportLayout->addLayout(m_scrollAreaLayout);
		viewportLayout->addStretch(1);
		viewportWidget->setLayout(viewportLayout);

		setWidget(viewportWidget);
	}

	VScrollGridWidget::~VScrollGridWidget()
	{

	}

	void VScrollGridWidget::AddRow(QList<QWidget*> widgets) {

		unsigned int columnCount = m_headerView->model()->columnCount();
		if (columnCount != widgets.count()) {

			throw std::invalid_argument("VScrollGridWidget rows must match the number of headers");
		}

		for (int i = 0; i < columnCount; ++i) {

			m_scrollAreaLayout->addWidget(widgets[i], m_rowCountIncludingLines, 2 * i);
		}

		if (m_rowCountIncludingLines == 0) {

			//Recreate vertical lines
			for (unsigned int i = 0; i < columnCount - 1; ++i) {

				QFrame* vLine = new QFrame(this);
				vLine->setFrameStyle(QFrame::Shape::VLine | QFrame::Shadow::Sunken);
				//vLine->setLineWidth(0);
				//vLine->setMidLineWidth(0);

				m_scrollAreaLayout->addWidget(vLine, 0, 2 * i + 1, -1, 1);
			}
		}

		QFrame* hLine = new QFrame(this);
		hLine->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
		//hLine->setLineWidth(0);
		//hLine->setMidLineWidth(0);
		
		m_scrollAreaLayout->addWidget(hLine, ++m_rowCountIncludingLines, 0, 1, -1);
		++m_rowCountIncludingLines;

		ResizeColumns(widgets);
	}

	void VScrollGridWidget::RemoveAllWidgets() {

		while (QLayoutItem* item = m_scrollAreaLayout->takeAt(0)) {

			if (item->widget() != nullptr) {

				delete item->widget();
			}
			delete item;
		}

		m_rowCountIncludingLines = 0;
		ResetHeaderSectionSizesToContents();
	}

	void VScrollGridWidget::ResizeColumns(QList<QWidget*> widgets) {

		for (unsigned int i = 0; i < widgets.size() - 1; ++i) {

			unsigned int gridColumnIndex = 2 * i;
			unsigned int minNewWidgetWidth = widgets[i]->minimumSizeHint().width() + 8; //8 is to make sure that there is a margin for now.
			if (minNewWidgetWidth <= m_columnMinimumWidths[i]) {

				widgets[i]->setFixedWidth(m_columnMinimumWidths[i]);
			}
			else {

				m_columnMinimumWidths[i] = minNewWidgetWidth;
				for (unsigned int j = 0; j < m_rowCountIncludingLines; j += 2) {

					m_scrollAreaLayout->itemAtPosition(j, gridColumnIndex)->widget()->setFixedWidth(m_columnMinimumWidths[i]);
				}
				m_headerView->resizeSection(i, m_columnMinimumWidths[i] + 1); //1 is added because resize handle width has to be included
			}
		}

		widget()->update();
	}

	void VScrollGridWidget::ResetHeaderSectionSizesToContents() {

		m_headerView->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
		for (unsigned int i = 0; i < m_headerView->model()->columnCount() - 1; ++i) {

			m_headerView->setSectionResizeMode(i, QHeaderView::ResizeMode::Fixed);
			m_columnMinimumWidths[i] = m_headerView->sectionSize(i);
		}
	}

	void VScrollGridWidget::resizeEvent(QResizeEvent* event) {

		m_headerView->setFixedWidth(event->size().width());
		VerticalScrollArea::resizeEvent(event);
	}

} //namespace SynGlyphX