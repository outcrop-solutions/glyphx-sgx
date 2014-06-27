#include "databindingwidget.h"

DataBindingWidget::DataBindingWidget(QWidget *parent)
	: QSplitter(Qt::Vertical, parent)
{
	setChildrenCollapsible(false);
	
	m_tagLineEdit = new BindingLineEdit(this);
	addWidget(m_tagLineEdit);

	m_descriptionEdit = new QTextEdit(this);
	addWidget(m_descriptionEdit);

	setStretchFactor(0, 1);
	setStretchFactor(1, 0);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

DataBindingWidget::~DataBindingWidget()
{

}
