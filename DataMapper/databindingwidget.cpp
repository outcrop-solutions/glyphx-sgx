#include "databindingwidget.h"
#include <QtWidgets/QLabel>

DataBindingWidget::DataBindingWidget(QWidget *parent)
	: QSplitter(Qt::Vertical, parent)
{
	setChildrenCollapsible(false);

	CreatePropertiesTable();
	
	CreateTagAndDescriptionWidget();

	setStretchFactor(0, 2);
	setStretchFactor(1, 0);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

}

DataBindingWidget::~DataBindingWidget()
{

}

void DataBindingWidget::CreateTagAndDescriptionWidget() {

	QWidget* widget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* tagLayout = new QHBoxLayout;

	QLabel* label = new QLabel(tr("<b>Tag:</b>"), this);
	m_tagLineEdit = new BindingLineEdit(this);

	m_descriptionEdit = new SynGlyphX::RichTextEditor(tr("<b>Description:</b>"), this);

	tagLayout->addWidget(label);
	tagLayout->addWidget(m_tagLineEdit);

	layout->addLayout(tagLayout);
	layout->addWidget(m_descriptionEdit, 1);
	widget->setLayout(layout);

	addWidget(widget);
}

void DataBindingWidget::CreatePropertiesTable() {

	QWidget* widget = new QWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout(this);

	for (int i = 0; i < 8; i = i + 2) {

		QLabel* label1 = new QLabel(tr("<b>Property</b>"), this);
		QLabel* label2 = new QLabel(tr("<b>Min</b>"), this);
		QLabel* label3 = new QLabel(tr("<b>Max</b>"), this);
		QLabel* label4 = new QLabel(tr("<b>Input</b>"), this);

		gridLayout->addWidget(label1, i, 0, Qt::AlignHCenter);
		gridLayout->addWidget(label2, i, 2, Qt::AlignHCenter);
		gridLayout->addWidget(label3, i, 4, Qt::AlignHCenter);
		gridLayout->addWidget(label4, i, 6, Qt::AlignHCenter);

		CreateGridLine(gridLayout, QFrame::HLine, i + 1);
	}

	CreateGridLine(gridLayout, QFrame::VLine, 1);
	CreateGridLine(gridLayout, QFrame::VLine, 3);
	CreateGridLine(gridLayout, QFrame::VLine, 5);

	layout->addLayout(gridLayout);
	layout->addStretch(1);

	widget->setLayout(layout);
	addWidget(widget);
}

void DataBindingWidget::CreatePropertyWidgets(QGridLayout* layout) {


}

void DataBindingWidget::CreateGridLine(QGridLayout* layout, QFrame::Shape shape, int index, int thickness) {

	QFrame* line = new QFrame(this);
	line->setFrameStyle(shape | QFrame::Plain);
	line->setLineWidth(thickness);

	if (shape == QFrame::VLine) {
		layout->addWidget(line, 0, index, -1, 1);
	}
	else {
		layout->addWidget(line, index, 0, 1, -1);
	}
}

void DataBindingWidget::SetGlyphModelIndex(const QModelIndex& index) {

}