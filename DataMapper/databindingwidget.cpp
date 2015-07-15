#include "databindingwidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtCore/QModelIndex>
#include "colorminmaxwidget.h"
#include "doubleminmaxwidget.h"
#include "intminmaxwidget.h"
#include "datamappingfunction.h"
#include "mappingfunctionwidget.h"
#include "glyphenumcombobox.h"

DataBindingWidget::DataBindingWidget(SingleGlyphRolesTableModel* model, QWidget *parent)
	: QTabWidget(parent),
	m_model(model)
{
	CreatePropertiesTable();
	CreateTagAndDescriptionWidget();
	CreateAnimationTable();
	CreateGeometryTopologyTab();

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	QObject::connect(model, &SingleGlyphRolesTableModel::modelReset, this, &DataBindingWidget::OnModelReset);
	OnModelReset();

	//QObject::connect(model, &SingleGlyphRolesTableModel::modelAboutToBeReset, this, &DataBindingWidget::CommitChanges);
}

DataBindingWidget::~DataBindingWidget()
{

}

void DataBindingWidget::CreateGeometryTopologyTab() {

	QWidget* widget = new QWidget(this);
	QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
	//widgetLayout->setContentsMargins(0, 0, 0, 0);
	QGridLayout* gridLayout = new QGridLayout(widget);
	gridLayout->setVerticalSpacing(0);

	CreateTableHeader(gridLayout);
	CreateVerticalGridLines(gridLayout, 3);
	CreateGridLine(gridLayout, QFrame::HLine, -1, 2);

	CreateVirtualTopologyTypePropertyWidgets(gridLayout, 16);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateGeometryShapePropertyWidgets(gridLayout, 17);

	gridLayout->setColumnStretch(6, 1);

	widgetLayout->addLayout(gridLayout);

	QHBoxLayout* nonMappableLayout = new QHBoxLayout(widget);

	m_nonMappableGeometryWidget = new SynGlyphX::NonMappableGeometryWidget(widget);
	nonMappableLayout->addWidget(m_nonMappableGeometryWidget);
	nonMappableLayout->addStretch(1);

	widgetLayout->addLayout(nonMappableLayout);
	widgetLayout->addStretch(1);
	widget->setLayout(widgetLayout);

	addTab(widget, tr("Geometry && Topology"));

	QObject::connect(m_nonMappableGeometryWidget, &SynGlyphX::NonMappableGeometryWidget::SurfaceChanged, this, &DataBindingWidget::OnSurfaceUpdated);
	QObject::connect(m_nonMappableGeometryWidget, &SynGlyphX::NonMappableGeometryWidget::TorusRatioChanged, this, &DataBindingWidget::OnTorusRatioUpdated);
}

void DataBindingWidget::CreateAnimationTable() {

	QWidget* widget = new QWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setVerticalSpacing(0);

	CreateTableHeader(gridLayout);
	CreateVerticalGridLines(gridLayout, 3);

	CreateGridLine(gridLayout, QFrame::HLine, -1, 2);

	CreateDoublePropertyWidgets(gridLayout, 13, -1000.0, 1000.0);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateDoublePropertyWidgets(gridLayout, 14, -1000.0, 1000.0);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateDoublePropertyWidgets(gridLayout, 15, -1000.0, 1000.0);

	gridLayout->setColumnStretch(6, 1);

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
	m_descriptionEdit->setEnabled(false);

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

void DataBindingWidget::CreateVerticalGridLines(QGridLayout* gridLayout, unsigned int count) {

	if (count < 1) {

		throw std::invalid_argument("Can only create one or more vertical grid lines");
	}

	for (int i = 1; i < (count * 2); i += 2) {

		CreateGridLine(gridLayout, QFrame::VLine, i);
	}
}

void DataBindingWidget::CreatePropertiesTable() {

	QWidget* widget = new QWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setVerticalSpacing(0);

	CreateTableHeader(gridLayout);
	CreateVerticalGridLines(gridLayout, 3);

	CreateGridLine(gridLayout, QFrame::HLine, -1, 2);

	CreateDoublePropertyWidgets(gridLayout, 0, -1000.0, 1000.0, true);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateDoublePropertyWidgets(gridLayout, 1, -1000.0, 1000.0, true);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateDoublePropertyWidgets(gridLayout, 2, 0.0, 1000.0);
	CreateGridLine(gridLayout, QFrame::HLine);

	CreateDoublePropertyWidgets(gridLayout, 3, -360.0, 360.0);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateDoublePropertyWidgets(gridLayout, 4, -360.0, 360.0);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateDoublePropertyWidgets(gridLayout, 5, -360.0, 360.0);
	CreateGridLine(gridLayout, QFrame::HLine);

	CreateDoublePropertyWidgets(gridLayout, 6, 0.0, 1000.0);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateDoublePropertyWidgets(gridLayout, 7, 0.0, 1000.0);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateDoublePropertyWidgets(gridLayout, 8, 0.0, 1000.0);
	CreateGridLine(gridLayout, QFrame::HLine);

	CreateColorPropertyWidgets(gridLayout, 9);
	CreateGridLine(gridLayout, QFrame::HLine);
	CreateDoublePropertyWidgets(gridLayout, 10, 0.0, 255.0);
	//CreateIntegerPropertyWidgets(gridLayout, 10);

	gridLayout->setColumnStretch(6, 1);

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

void DataBindingWidget::CreateRowOfPropertyWidgets(QGridLayout* layout, QWidget* valueWidget, MappingFunctionWidget* mappingFunctionWidget, int modelRow, bool addToPositionXYList) {

	QDataWidgetMapper* mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

	QLabel* label = new QLabel(m_model->headerData(modelRow, Qt::Vertical, Qt::DisplayRole).toString(), this);
	QFont labelFont = label->font();
	labelFont.setBold(true);
	label->setFont(labelFont);

	BindingLineEdit* inputBindingLineEdit = new BindingLineEdit(m_model, this, SynGlyphX::MappingFunctionData::Input::Numeric);

	mapper->setModel(m_model);

	mapper->addMapping(valueWidget, 0);
	mapper->addMapping(mappingFunctionWidget, 1);
	mapper->addMapping(inputBindingLineEdit, 2);

	int newRow = layout->rowCount();
	layout->addWidget(label, newRow, 0, Qt::AlignHCenter);
	layout->addWidget(valueWidget, newRow, 2, Qt::AlignHCenter);
	layout->addWidget(mappingFunctionWidget, newRow, 4, Qt::AlignHCenter);
	layout->addWidget(inputBindingLineEdit, newRow, 6);

	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::FunctionChanged, mapper, &QDataWidgetMapper::submit);
	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::SupportedInputChanged, inputBindingLineEdit, &BindingLineEdit::SetAcceptedInputTypes);
	QObject::connect(inputBindingLineEdit, &BindingLineEdit::ValueChangedByUser, mapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappers.push_back(mapper);

	if (addToPositionXYList) {

		m_positionXYMinMaxWidgets.push_back(valueWidget);
		m_positionXYMinMaxWidgets.push_back(mappingFunctionWidget);
	}
}

void DataBindingWidget::CreateIntegerPropertyWidgets(QGridLayout* layout, int modelRow, int min, int max) {

	SynGlyphX::IntMinMaxWidget* minMaxWidget = new SynGlyphX::IntMinMaxWidget(this);
	minMaxWidget->layout()->setContentsMargins(0, 0, 0, 0);
	minMaxWidget->SetKeyboardTracking(false);
	minMaxWidget->SetRange(min, max);
	
	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Numeric, m_model, modelRow, this);
	mappingFunctionWidget->SetDialogOutputMinMax(min, max);
	CreateRowOfPropertyWidgets(layout, minMaxWidget, mappingFunctionWidget, modelRow);

	QObject::connect(minMaxWidget, &SynGlyphX::IntMinMaxWidget::ValueChanged, m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
}

void DataBindingWidget::CreateDoublePropertyWidgets(QGridLayout* layout, int modelRow, double min, double max, bool addToPositionXYList) {

	SynGlyphX::DoubleMinMaxWidget* minMaxWidget = new SynGlyphX::DoubleMinMaxWidget(this);
	minMaxWidget->layout()->setContentsMargins(0, 0, 0, 0);
	minMaxWidget->SetKeyboardTracking(false);
	minMaxWidget->SetRange(min, max);
	
	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Numeric, m_model, modelRow, this);
	mappingFunctionWidget->SetDialogOutputMinMax(min, max);
	CreateRowOfPropertyWidgets(layout, minMaxWidget, mappingFunctionWidget, modelRow, addToPositionXYList);

	QObject::connect(minMaxWidget, &SynGlyphX::DoubleMinMaxWidget::ValueChanged, m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
}

void DataBindingWidget::CreateColorPropertyWidgets(QGridLayout* layout, int modelRow) {

	SynGlyphX::ColorMinMaxWidget* minMaxWidget = new SynGlyphX::ColorMinMaxWidget(false, this);
	minMaxWidget->layout()->setContentsMargins(0, 0, 0, 0);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Color, m_model, modelRow, this);
	CreateRowOfPropertyWidgets(layout, minMaxWidget, mappingFunctionWidget, modelRow);

	QObject::connect(minMaxWidget, &SynGlyphX::ColorMinMaxWidget::ValueChanged, m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
}

void DataBindingWidget::CreateGeometryShapePropertyWidgets(QGridLayout* layout, int modelRow) {

	SynGlyphX::GlyphShapeComboBox* comboBox = new SynGlyphX::GlyphShapeComboBox(this);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::GeometryShape, m_model, modelRow, this);
	CreateRowOfPropertyWidgets(layout, comboBox, mappingFunctionWidget, modelRow);

	QObject::connect(comboBox, &SynGlyphX::GlyphShapeComboBox::currentTextChanged, m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
}

void DataBindingWidget::CreateVirtualTopologyTypePropertyWidgets(QGridLayout* layout, int modelRow) {

	SynGlyphX::VirtualTopologyComboBox* comboBox = new SynGlyphX::VirtualTopologyComboBox(this);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::VirtualTopology, m_model, modelRow, this);
	CreateRowOfPropertyWidgets(layout, comboBox, mappingFunctionWidget, modelRow);

	QObject::connect(comboBox, &SynGlyphX::VirtualTopologyComboBox::currentTextChanged, m_dataWidgetMappers.last(), &QDataWidgetMapper::submit);
}

void DataBindingWidget::CreateGridLine(QGridLayout* layout, QFrame::Shape shape, int index, int thickness) {

	QFrame* line = new QFrame(this);
	line->setFrameStyle(shape | QFrame::Plain);
	line->setLineWidth(thickness);

	if (shape == QFrame::VLine) {

		if (index == -1) {

			index = layout->columnCount();
		}
		layout->addWidget(line, 0, index, -1, 1);
	}
	else {

		if (index == -1) {

			index = layout->rowCount();
		}
		layout->addWidget(line, index, 0, 1, -1);
	}
}

void DataBindingWidget::OnModelReset() {

	bool doesModelHaveData = !m_model->IsClear();
	setEnabled(doesModelHaveData);

	if (doesModelHaveData) {

		for (int i = 0; i < m_dataWidgetMappers.length(); ++i) {

			if (m_dataWidgetMappers[i] != nullptr) {
				m_dataWidgetMappers[i]->setCurrentIndex(i);
			}
		}

		bool areSignalsBlocked = m_nonMappableGeometryWidget->blockSignals(true);
		m_nonMappableGeometryWidget->SetWidget(static_cast<SynGlyphX::GlyphGeometryInfo::Surface>(m_model->data(m_model->index(18, 0), Qt::EditRole).toInt()), m_model->data(m_model->index(19, 0), Qt::EditRole).toDouble());
		m_nonMappableGeometryWidget->blockSignals(areSignalsBlocked);

		OnBaseObjectChanged();
	}
}

void DataBindingWidget::OnSurfaceUpdated() {

	m_model->setData(m_model->index(18, 0), m_nonMappableGeometryWidget->GetSurface());
}

void DataBindingWidget::OnTorusRatioUpdated() {

	m_model->setData(m_model->index(19, 0), m_nonMappableGeometryWidget->GetTorusRatio());
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