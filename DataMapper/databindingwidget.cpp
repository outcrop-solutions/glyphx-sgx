#include "databindingwidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtCore/QModelIndex>
#include "colorbutton.h"

DataBindingWidget::DataBindingWidget(MinMaxGlyphModel* model, QWidget *parent)
	: QSplitter(Qt::Vertical, parent),
	m_model(model)
{
	setChildrenCollapsible(false);

	CreatePropertiesTable();
	
	CreateTagAndDescriptionWidget();

	setStretchFactor(0, 2);
	setStretchFactor(1, 0);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	QObject::connect(model, &MinMaxGlyphModel::modelReset, this, &DataBindingWidget::OnModelReset);
	OnModelReset();

	//QObject::connect(model, &MinMaxGlyphModel::modelAboutToBeReset, this, &DataBindingWidget::CommitChanges);
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

	QList<QLabel*> columnHeaders;
	columnHeaders.push_back(new QLabel(tr("Property"), this));
	
	for (int j = 0; j < m_model->columnCount(); ++j) {

		columnHeaders.push_back(new QLabel(m_model->headerData(j, Qt::Horizontal, Qt::DisplayRole).toString(), this));
	}

	for (int i = 0; i < columnHeaders.length(); ++i) {

		QLabel* label = columnHeaders[i];
		QFont labelFont = label->font();
		labelFont.setBold(true);
		label->setFont(labelFont);
		gridLayout->addWidget(label, 0, i * 2, Qt::AlignHCenter);
	}

	CreateGridLine(gridLayout, QFrame::VLine, 1);
	CreateGridLine(gridLayout, QFrame::VLine, 3);
	CreateGridLine(gridLayout, QFrame::VLine, 5);

	CreateGridLine(gridLayout, QFrame::HLine, 1, 2);

	CreateDoublePropertyWidgets(gridLayout, 2);
	CreateGridLine(gridLayout, QFrame::HLine, 3);
	CreateDoublePropertyWidgets(gridLayout, 4);
	CreateGridLine(gridLayout, QFrame::HLine, 5);
	CreateDoublePropertyWidgets(gridLayout, 6);
	CreateGridLine(gridLayout, QFrame::HLine, 7);

	CreateDoublePropertyWidgets(gridLayout, 8);
	CreateGridLine(gridLayout, QFrame::HLine, 9);
	CreateDoublePropertyWidgets(gridLayout, 10);
	CreateGridLine(gridLayout, QFrame::HLine, 11);
	CreateDoublePropertyWidgets(gridLayout, 12);
	CreateGridLine(gridLayout, QFrame::HLine, 13);

	CreateDoublePropertyWidgets(gridLayout, 14);
	CreateGridLine(gridLayout, QFrame::HLine, 15);
	CreateDoublePropertyWidgets(gridLayout, 16);
	CreateGridLine(gridLayout, QFrame::HLine, 17);
	CreateDoublePropertyWidgets(gridLayout, 18);
	CreateGridLine(gridLayout, QFrame::HLine, 19);

	CreateColorPropertyWidgets(gridLayout, 20);
	CreateGridLine(gridLayout, QFrame::HLine, 21);
	CreateIntegerPropertyWidgets(gridLayout, 22);

	gridLayout->setColumnStretch(0, 1);
	gridLayout->setColumnStretch(2, 1);
	gridLayout->setColumnStretch(4, 1);
	gridLayout->setColumnStretch(6, 2);

	layout->addLayout(gridLayout);
	layout->addStretch(1);

	widget->setLayout(layout);
	addWidget(widget);
}

void DataBindingWidget::CreateRowOfPropertyWidgets(QGridLayout* layout, QWidget* minWidget, QWidget* maxWidget, int row) {

	QDataWidgetMapper* mapper = new QDataWidgetMapper(this);

	QString str = m_model->headerData(row / 2 - 1, Qt::Vertical, Qt::DisplayRole).toString();
	QLabel* label = new QLabel(m_model->headerData(row/2 - 1, Qt::Vertical, Qt::DisplayRole).toString(), this);
	QFont labelFont = label->font();
	labelFont.setBold(true);
	label->setFont(labelFont);

	BindingLineEdit* inputBindingLineEdit = new BindingLineEdit(this);

	//mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	mapper->setModel(m_model);

	mapper->addMapping(minWidget, 0);
	mapper->addMapping(maxWidget, 1);
	mapper->addMapping(inputBindingLineEdit, 2);

	layout->addWidget(label, row, 0, Qt::AlignHCenter);
	layout->addWidget(minWidget, row, 2, Qt::AlignHCenter);
	layout->addWidget(maxWidget, row, 4, Qt::AlignHCenter);
	layout->addWidget(inputBindingLineEdit, row, 6);

	QObject::connect(inputBindingLineEdit, &BindingLineEdit::ValueChangedByUser, mapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappers.push_back(mapper);
}

void DataBindingWidget::CreateIntegerPropertyWidgets(QGridLayout* layout, int row) {

	QSpinBox* minSpinBox = new QSpinBox(this);
	minSpinBox->setRange(0, 255);
	QSpinBox* maxSpinBox = new QSpinBox(this);
	maxSpinBox->setRange(0, 255);
	
	CreateRowOfPropertyWidgets(layout, minSpinBox, maxSpinBox, row);
}

void DataBindingWidget::CreateDoublePropertyWidgets(QGridLayout* layout, int row) {

	QDoubleSpinBox* minSpinBox = new QDoubleSpinBox(this);
	minSpinBox->setRange(-1000.0, 1000.0);
	QDoubleSpinBox* maxSpinBox = new QDoubleSpinBox(this);
	maxSpinBox->setRange(-1000.0, 1000.0);
	
	CreateRowOfPropertyWidgets(layout, minSpinBox, maxSpinBox, row);
}

void DataBindingWidget::CreateColorPropertyWidgets(QGridLayout* layout, int row) {

	SynGlyphX::ColorButton* minColorButton = new SynGlyphX::ColorButton(false, this);
	SynGlyphX::ColorButton* maxColorButton = new SynGlyphX::ColorButton(false, this);

	CreateRowOfPropertyWidgets(layout, minColorButton, maxColorButton, row);

	QObject::connect(minColorButton, &SynGlyphX::ColorButton::ColorChanged, m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
	QObject::connect(maxColorButton, &SynGlyphX::ColorButton::ColorChanged, m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
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

void DataBindingWidget::OnModelReset() {

	for (int i = 0; i < m_dataWidgetMappers.length(); ++i) {
		m_dataWidgetMappers[i]->setCurrentIndex(i);
	}
}

void DataBindingWidget::CommitChanges() {

	for (QDataWidgetMapper* mapper : m_dataWidgetMappers) {

		mapper->submit();
	}
}