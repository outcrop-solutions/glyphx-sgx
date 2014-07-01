#include "databindingwidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtCore/QModelIndex>
#include "colorbutton.h"

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

	QLabel* label1 = new QLabel(tr("<b>Property</b>"), this);
	QLabel* label2 = new QLabel(tr("<b>Min</b>"), this);
	QLabel* label3 = new QLabel(tr("<b>Max</b>"), this);
	QLabel* label4 = new QLabel(tr("<b>Input</b>"), this);

	gridLayout->addWidget(label1, 0, 0, Qt::AlignHCenter);
	gridLayout->addWidget(label2, 0, 2, Qt::AlignHCenter);
	gridLayout->addWidget(label3, 0, 4, Qt::AlignHCenter);
	gridLayout->addWidget(label4, 0, 6, Qt::AlignHCenter);

	CreateGridLine(gridLayout, QFrame::VLine, 1);
	CreateGridLine(gridLayout, QFrame::VLine, 3);
	CreateGridLine(gridLayout, QFrame::VLine, 5);

	CreateGridLine(gridLayout, QFrame::HLine, 1, 2);

	CreateDoublePropertyWidgets(gridLayout, tr("Position X"), 2);
	CreateGridLine(gridLayout, QFrame::HLine, 3);
	CreateDoublePropertyWidgets(gridLayout, tr("Position Y"), 4);
	CreateGridLine(gridLayout, QFrame::HLine, 5);
	CreateDoublePropertyWidgets(gridLayout, tr("Position Z"), 6);
	CreateGridLine(gridLayout, QFrame::HLine, 7);

	CreateDoublePropertyWidgets(gridLayout, tr("Rotation X"), 8);
	CreateGridLine(gridLayout, QFrame::HLine, 9);
	CreateDoublePropertyWidgets(gridLayout, tr("Rotation Y"), 10);
	CreateGridLine(gridLayout, QFrame::HLine, 11);
	CreateDoublePropertyWidgets(gridLayout, tr("Rotation Z"), 12);
	CreateGridLine(gridLayout, QFrame::HLine, 13);

	CreateDoublePropertyWidgets(gridLayout, tr("Scale X"), 14);
	CreateGridLine(gridLayout, QFrame::HLine, 15);
	CreateDoublePropertyWidgets(gridLayout, tr("Scale Y"), 16);
	CreateGridLine(gridLayout, QFrame::HLine, 17);
	CreateDoublePropertyWidgets(gridLayout, tr("Scale Z"), 18);
	CreateGridLine(gridLayout, QFrame::HLine, 19);

	CreateColorPropertyWidgets(gridLayout, tr("Color"), 20);
	CreateGridLine(gridLayout, QFrame::HLine, 21);
	CreateIntegerPropertyWidgets(gridLayout, tr("Transparency"), 22);

	gridLayout->setColumnStretch(0, 1);
	gridLayout->setColumnStretch(2, 1);
	gridLayout->setColumnStretch(4, 1);
	gridLayout->setColumnStretch(6, 2);

	layout->addLayout(gridLayout);
	layout->addStretch(1);

	widget->setLayout(layout);
	addWidget(widget);
}

void DataBindingWidget::CreateRowOfPropertyWidgets(QGridLayout* layout, const QString& name, QWidget* minWidget, QWidget* maxWidget, int row) {

	QDataWidgetMapper* mapper = new QDataWidgetMapper(this);
	QLabel* label = new QLabel(name, this);
	BindingLineEdit* inputBindingLineEdit = new BindingLineEdit(this);

	mapper->addMapping(minWidget, 0);
	mapper->addMapping(maxWidget, 1);
	mapper->addMapping(inputBindingLineEdit, 2);

	m_dataWidgetMappers.push_back(mapper);

	layout->addWidget(label, row, 0, Qt::AlignHCenter);
	layout->addWidget(minWidget, row, 2, Qt::AlignHCenter);
	layout->addWidget(maxWidget, row, 4, Qt::AlignHCenter);
	layout->addWidget(inputBindingLineEdit, row, 6);
}

void DataBindingWidget::CreateIntegerPropertyWidgets(QGridLayout* layout, const QString& name, int row) {

	QSpinBox* minSpinBox = new QSpinBox(this);
	QSpinBox* maxSpinBox = new QSpinBox(this);
	
	CreateRowOfPropertyWidgets(layout, name, minSpinBox, maxSpinBox, row);
}

void DataBindingWidget::CreateDoublePropertyWidgets(QGridLayout* layout, const QString& name, int row) {

	QDoubleSpinBox* minSpinBox = new QDoubleSpinBox(this);
	QDoubleSpinBox* maxSpinBox = new QDoubleSpinBox(this);
	
	CreateRowOfPropertyWidgets(layout, name, minSpinBox, maxSpinBox, row);
}

void DataBindingWidget::CreateColorPropertyWidgets(QGridLayout* layout, const QString& name, int row) {

	SynGlyphX::ColorButton* minColorButton = new SynGlyphX::ColorButton(false, this);
	SynGlyphX::ColorButton* maxColorButton = new SynGlyphX::ColorButton(false, this);

	CreateRowOfPropertyWidgets(layout, name, minColorButton, maxColorButton, row);
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

	int numChildren = index.model()->rowCount(index);
	if (numChildren != m_dataWidgetMappers.length()) {

		throw std::exception("Data Binding Mismatch");
	}

	for (int i = 0; i < numChildren; ++i) {
		
	}
}