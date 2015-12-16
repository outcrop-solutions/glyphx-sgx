#include "glyphrolessubsettablewidget.h"
#include <QtGui/QResizeEvent>

GlyphRolesSubsetTableWidget::GlyphRolesSubsetTableWidget(GlyphRolesTableModel* model, QWidget *parent)
	: QAbstractScrollArea(parent),
	m_model(model)
{
	m_headerView = new QHeaderView(Qt::Horizontal, this);
	m_headerView->setModel(model);
	m_headerView->setSectionsClickable(false);
	m_headerView->setStretchLastSection(true);
	m_headerView->setDefaultAlignment(Qt::AlignCenter);

	setViewportMargins(0, m_headerView->sizeHint().height(), 0, 0);
}

GlyphRolesSubsetTableWidget::~GlyphRolesSubsetTableWidget()
{

}

void GlyphRolesSubsetTableWidget::resizeEvent(QResizeEvent* event) {

	QRect vg = viewport()->geometry();
	m_headerView->setGeometry(vg.left(), vg.top() - m_headerView->sizeHint().height(), vg.width(), m_headerView->sizeHint().height());

	QAbstractScrollArea::resizeEvent(event);
}
