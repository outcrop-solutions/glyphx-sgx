#include "databindingwidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtCore/QModelIndex>
#include "colorbutton.h"
#include "datamappingfunction.h"
#include "mappingfunctionwidget.h"

DataBindingWidget::DataBindingWidget(MinMaxGlyphModel* model, QWidget *parent)
	: QTabWidget(parent),
	m_model(model)
{
	CreatePropertiesTable();
	CreateTagAndDescriptionWidget();
	CreateAnimationTable();

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	QObject::connect(model, &MinMaxGlyphModel::modelReset, this, &DataBindingWidget::OnModelReset);
	OnModelReset();

	//QObject::connect(model, &MinMaxGlyphModel::modelAboutToBeReset, this, &DataBindingWidget::CommitChanges);
}

DataBindingWidget::~DataBindingWidget()
{

}

void DataBindingWidget::CreateAnimationTable() {

	QWidget* widget = new QWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout(this);

	CreateTableHeader(gridLayout);
	CreateGridLines(gridLayout);

	CreateGridLine(gridLayout, QFrame::HLine, 1, 2);

	CreateDoublePropertyWidgets(gridLayout, 2, 13);
	CreateGridLine(gridLayout, QFrame::HLine, 3);
	CreateDoublePropertyWidgets(gridLayout, 4, 14);
	CreateGridLine(gridLayout, QFrame::HLine, 5);
	CreateDoublePropertyWidgets(gridLayout, 6, 15);

	gridLayout->setColumnStretch(8, 1);

	layout->addLayout(gridLayout);
	layout->addStretch(1);

	widget->setLayout(layout);
	addTab(widget, tr("Animation"));
}

void DataBindingWidget::CreateTagAndDescriptionWidget() {

	QWidget* widget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* tagLayout = new QHBoxLayout;

	QLabel* label = new QLabel(m_model->headerData(11, Qt::Vertical, Qt::DisplayRole).toString() + ":", this);
	QFont labelFont = label->font();
	labelFont.setBold(true);
	label->setFont(labelFont);

	m_tagLineEdit = new BindingLineEdit(m_model, this);

	QDataWidgetMapper* tagMapper = new QDataWidgetMapper(this);
	tagMapper->setModel(m_model);
	tagMapper->addMapping(m_tagLineEdit, 3);  //Bind this to section 4 to keep consistent with other input fields

	QObject::connect(m_tagLineEdit, &BindingLineEdit::ValueChangedByUser, tagMapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappers.push_back(tagMapper);

	m_descriptionEdit = new SynGlyphX::RichTextEditor("<b>" + m_model->headerData(12, Qt::Vertical, Qt::DisplayRole).toString() + ":</b>", this);

	QDataWidgetMapper* descriptionMapper = new QDataWidgetMapper(this);
	descriptionMapper->setModel(m_model);
	descriptionMapper->addMapping(m_descriptionEdit, 3);  //Bind this to section 4 to keep consistent with other input fields

	//QObject::connect(m_descriptionEdit, &BindingLineEdit::ValueChangedByUser, mapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappers.push_back(descriptionMapper);

	tagLayout->addWidget(label);
	tagLayout->addWidget(m_tagLineEdit);

	layout->addLayout(tagLayout);
	layout->addWidget(m_descriptionEdit, 1);
	widget->setLayout(layout);

	addTab(widget, tr("Tag && Description"));
}

void DataBindingWidget::CreateGridLines(QGridLayout* gridLayout) {

	CreateGridLine(gridLayout, QFrame::VLine, 1);
	CreateGridLine(gridLayout, QFrame::VLine, 3);
	CreateGridLine(gridLayout, QFrame::VLine, 5);
	CreateGridLine(gridLayout, QFrame::VLine, 7);
}

void DataBindingWidget::CreatePropertiesTable() {

	QWidget* widget = new QWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setVerticalSpacing(0);

	CreateTableHeader(gridLayout);
	CreateGridLines(gridLayout);

	CreateGridLine(gridLayout, QFrame::HLine, 1, 2);

	CreateDoublePropertyWidgets(gridLayout, 2, 0, true);
	CreateGridLine(gridLayout, QFrame::HLine, 3);
	CreateDoublePropertyWidgets(gridLayout, 4, 1, true);
	CreateGridLine(gridLayout, QFrame::HLine, 5);
	CreateDoublePropertyWidgets(gridLayout, 6, 2);
	CreateGridLine(gridLayout, QFrame::HLine, 7);

	CreateDoublePropertyWidgets(gridLayout, 8, 3);
	CreateGridLine(gridLayout, QFrame::HLine, 9);
	CreateDoublePropertyWidgets(gridLayout, 10, 4);
	CreateGridLine(gridLayout, QFrame::HLine, 11);
	CreateDoublePropertyWidgets(gridLayout, 12, 5);
	CreateGridLine(gridLayout, QFrame::HLine, 13);

	CreateDoublePropertyWidgets(gridLayout, 14, 6);
	CreateGridLine(gridLayout, QFrame::HLine, 15);
	CreateDoublePropertyWidgets(gridLayout, 16, 7);
	CreateGridLine(gridLayout, QFrame::HLine, 17);
	CreateDoublePropertyWidgets(gridLayout, 18, 8);
	CreateGridLine(gridLayout, QFrame::HLine, 19);

	CreateColorPropertyWidgets(gridLayout, 20, 9);
	CreateGridLine(gridLayout, QFrame::HLine, 21);
	CreateIntegerPropertyWidgets(gridLayout, 22, 10);

	gridLayout->setColumnStretch(8, 1);

	layout->addLayout(gridLayout);
	layout->addStretch(1);

	widget->setLayout(layout);
	addTab(widget, tr("Base Properties"));
}

void DataBindingWidget::CreateTableHeader(QGridLayout* gridLayout) {

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
}

void DataBindingWidget::CreateRowOfPropertyWidgets(QGridLayout* layout, QWidget* minWidget, QWidget* maxWidget, int row, int header, bool addToPositionXYList) {

	QDataWidgetMapper* mapper = new QDataWidgetMapper(this);

	QLabel* label = new QLabel(m_model->headerData(header, Qt::Vertical, Qt::DisplayRole).toString(), this);
	QFont labelFont = label->font();
	labelFont.setBold(true);
	label->setFont(labelFont);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(this);

	BindingLineEdit* inputBindingLineEdit = new BindingLineEdit(m_model, this, SynGlyphX::MappingFunctionData::Input::Numeric);

	mapper->setModel(m_model);

	mapper->addMapping(minWidget, 0);
	mapper->addMapping(maxWidget, 1);
	mapper->addMapping(mappingFunctionWidget, 2);
	mapper->addMapping(inputBindingLineEdit, 3);

	layout->addWidget(label, row, 0, Qt::AlignHCenter);
	layout->addWidget(minWidget, row, 2, Qt::AlignHCenter);
	layout->addWidget(maxWidget, row, 4, Qt::AlignHCenter);
	layout->addWidget(mappingFunctionWidget, row, 6, Qt::AlignHCenter);
	layout->addWidget(inputBindingLineEdit, row, 8);

	//QObject::connect(inputBindingLineEdit, &BindingLineEdit::ValueChangedByUser, this, [=]() {if (mapper->submit()) { mapper->setCurrentIndex(row / 2 - 1); } });
	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::SupportedInputChanged, inputBindingLineEdit, &BindingLineEdit::SetAcceptedInputTypes);
	QObject::connect(inputBindingLineEdit, &BindingLineEdit::ValueChangedByUser, mapper, &QDataWidgetMapper::submit);
	//QObject::connect(functionComboBox, &QComboBox::currentTextChanged, mapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappers.push_back(mapper);

	if (addToPositionXYList) {

		m_positionXYMinMaxWidgets.push_back(minWidget);
		m_positionXYMinMaxWidgets.push_back(maxWidget);
		m_positionXYMinMaxWidgets.push_back(mappingFunctionWidget);
	}
}

void DataBindingWidget::CreateIntegerPropertyWidgets(QGridLayout* layout, int row, int header) {

	QSpinBox* minSpinBox = new QSpinBox(this);
	minSpinBox->setRange(0, 255);
	QSpinBox* maxSpinBox = new QSpinBox(this);
	maxSpinBox->setRange(0, 255);
	
	CreateRowOfPropertyWidgets(layout, minSpinBox, maxSpinBox, row, header);

	QObject::connect(minSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
	QObject::connect(maxSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
}

void DataBindingWidget::CreateDoublePropertyWidgets(QGridLayout* layout, int row, int header, bool addToPositionXYList) {

	QDoubleSpinBox* minSpinBox = new QDoubleSpinBox(this);
	minSpinBox->setRange(-1000.0, 1000.0);
	QDoubleSpinBox* maxSpinBox = new QDoubleSpinBox(this);
	maxSpinBox->setRange(-1000.0, 1000.0);
	
	CreateRowOfPropertyWidgets(layout, minSpinBox, maxSpinBox, row, header, addToPositionXYList);

	QObject::connect(minSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
	QObject::connect(maxSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
}

void DataBindingWidget::CreateColorPropertyWidgets(QGridLayout* layout, int row, int header) {

	SynGlyphX::ColorButton* minColorButton = new SynGlyphX::ColorButton(false, this);
	SynGlyphX::ColorButton* maxColorButton = new SynGlyphX::ColorButton(false, this);

	CreateRowOfPropertyWidgets(layout, minColorButton, maxColorButton, row, header);

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

	setEnabled(!m_model->IsClear());

	for (int i = 0; i < m_dataWidgetMappers.length(); ++i) {

		if (m_dataWidgetMappers[i] != nullptr) {
			m_dataWidgetMappers[i]->setCurrentIndex(i);
		}
	}

	OnBaseObjectChanged();
}

void DataBindingWidget::CommitChanges() {

	for (QDataWidgetMapper* mapper : m_dataWidgetMappers) {

		if (mapper != nullptr) {
			mapper->submit();
		}
	}
}

void DataBindingWidget::OnBaseObjectChanged() {

	EnablePositionXYMixMaxWidgets(!(m_model->IsCurrentGlyphRoot() && (m_model->GetDataTransformMapping()->GetBaseObjects()[0].GetType() == SynGlyphX::BaseImage::Type::DownloadedMap)));
}

void DataBindingWidget::EnablePositionXYMixMaxWidgets(bool enable) {

	for (QWidget* widget : m_positionXYMinMaxWidgets) {

		if (widget != nullptr) {
			widget->setEnabled(enable);
		}
	}
}