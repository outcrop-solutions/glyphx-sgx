#include "databindingtableswidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtCore/QModelIndex>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDataWidgetMapper>
#include "colorminmaxwidget.h"
#include "doubleminmaxwidget.h"
#include "intminmaxwidget.h"
#include "datamappingfunction.h"
#include "mappingfunctionwidget.h"
#include "glyphenumcombobox.h"
#include "stretchsurroundedwidget.h"
#include "baseimage.h"
#include "datatransformmapping.h"

const QMargins DataBindingTablesWidget::s_cellMargins(8, 5, 8, 5);

DataBindingTablesWidget::DataBindingTablesWidget(GlyphRolesTableModel* model, QWidget *parent)
	: QTabWidget(parent),
	m_model(model),
	m_torusRatioMapper(nullptr)
{
	CreateBasePropertiesTable();
	CreateTagAndDescriptionWidget();
	CreateAnimationTable();
	CreateGeometryTopologyTab();
	CreateURLTab();

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	for (auto mapperAndRow : m_dataWidgetMappersAndRows) {

		if (mapperAndRow.first != nullptr) {

			mapperAndRow.first->setCurrentIndex(mapperAndRow.second);
		}
	}

	QObject::connect(model, &GlyphRolesTableModel::dataChanged, this, &DataBindingTablesWidget::OnModelDataChanged);
	OnModelDataChanged();
}

DataBindingTablesWidget::~DataBindingTablesWidget()
{

}

void DataBindingTablesWidget::CreateGeometryTopologyTab() {

	SynGlyphX::VerticalTabOrderTableView* tableView = CreateSubsetTableView({ SynGlyphX::DataMappingGlyph::MappableField::VirtualTopology,
													SynGlyphX::DataMappingGlyph::MappableField::GeometryShape, 
													SynGlyphX::DataMappingGlyph::MappableField::GeometryShape + 1, 
													SynGlyphX::DataMappingGlyph::MappableField::GeometryShape + 2});
	tableView->setSpan(2, 1, 1, 3);
	tableView->setSpan(3, 1, 1, 3);

	CreateVirtualTopologyTypePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::VirtualTopology);
	CreateGeometryShapePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::GeometryShape);

	tableView->resizeColumnToContents(1);

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::Fixed);
	horizontalHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(3, QHeaderView::Stretch);

	m_surfaceRadioButtonWidget = new SynGlyphX::SurfaceRadioButtonWidget(Qt::Horizontal, tableView);
	QWidget* surfaceBoundingWidget = new QWidget(this);
	QHBoxLayout* surfaceBoundingWidgetLayout = new QHBoxLayout(surfaceBoundingWidget);
	surfaceBoundingWidgetLayout->setContentsMargins(s_cellMargins);
	surfaceBoundingWidgetLayout->addWidget(m_surfaceRadioButtonWidget);
	surfaceBoundingWidgetLayout->addStretch(1);
	surfaceBoundingWidget->setLayout(surfaceBoundingWidgetLayout);

	m_torusRatioSpinBox = new QDoubleSpinBox(this);
	m_torusRatioSpinBox->setSingleStep(0.05);
	m_torusRatioSpinBox->setDecimals(2);
	QWidget* torusRatioBoundingWidget = new QWidget(this);
	QHBoxLayout* torusRatioBoundingWidgetLayout = new QHBoxLayout(torusRatioBoundingWidget);
	torusRatioBoundingWidgetLayout->setContentsMargins(s_cellMargins);
	torusRatioBoundingWidgetLayout->addWidget(m_torusRatioSpinBox);
	torusRatioBoundingWidgetLayout->addStretch(1);
	torusRatioBoundingWidget->setLayout(torusRatioBoundingWidgetLayout);

	SynGlyphX::TableSubsetProxyModel* proxyModel = dynamic_cast<SynGlyphX::TableSubsetProxyModel*>(tableView->model());
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(SynGlyphX::DataMappingGlyph::MappableField::GeometryShape + 1, GlyphRolesTableModel::s_valueColumn)), surfaceBoundingWidget);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(SynGlyphX::DataMappingGlyph::MappableField::GeometryShape + 2, GlyphRolesTableModel::s_valueColumn)), torusRatioBoundingWidget);

	std::array<QDataWidgetMapper*, 2> mappers;
	for (int i = 0; i < 2; ++i) {

		mappers[i] = new QDataWidgetMapper(this);
		mappers[i]->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
		mappers[i]->setModel(m_model);
	}

	mappers[0]->addMapping(m_surfaceRadioButtonWidget, GlyphRolesTableModel::s_valueColumn);
	mappers[1]->addMapping(m_torusRatioSpinBox, GlyphRolesTableModel::s_valueColumn);

	m_dataWidgetMappersAndRows[mappers[0]] = SynGlyphX::DataMappingGlyph::MappableField::GeometryShape + 1;
	m_dataWidgetMappersAndRows[mappers[1]] = SynGlyphX::DataMappingGlyph::MappableField::GeometryShape + 2;

	m_torusRatioMapper = mappers[1];

	QObject::connect(m_surfaceRadioButtonWidget, &SynGlyphX::SurfaceRadioButtonWidget::ButtonClicked, mappers[0], &QDataWidgetMapper::submit);
	QObject::connect(m_torusRatioSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &DataBindingTablesWidget::OnTorusRatioChanged);

	addTab(tableView, tr("Geometry && Topology"));
}

void DataBindingTablesWidget::CreateAnimationTable() {

	SynGlyphX::VerticalTabOrderTableView* tableView = CreateSubsetTableView({ SynGlyphX::DataMappingGlyph::MappableField::RotationRateX,
													SynGlyphX::DataMappingGlyph::MappableField::RotationRateY, 
													SynGlyphX::DataMappingGlyph::MappableField::RotationRateZ });

	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::RotationRateX, -1000.0, 1000.0);
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::RotationRateY, -1000.0, 1000.0);
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::RotationRateZ, -1000.0, 1000.0);

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(3, QHeaderView::Stretch);

	addTab(tableView, tr("Animation"));
}

void DataBindingTablesWidget::CreateURLTab() {

	SynGlyphX::VerticalTabOrderTableView* tableView = CreateSubsetTableView({ SynGlyphX::DataMappingGlyph::MappableField::URL }, { GlyphRolesTableModel::s_propertyNameColumn, GlyphRolesTableModel::s_mappedFieldColumn });
	SynGlyphX::TableSubsetProxyModel* proxyModel = dynamic_cast<SynGlyphX::TableSubsetProxyModel*>(tableView->model());

	BindingLineEdit* urlLineEdit = new BindingLineEdit(m_model, tableView);
	urlLineEdit->layout()->setContentsMargins(s_cellMargins);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(SynGlyphX::DataMappingGlyph::MappableField::URL, GlyphRolesTableModel::s_mappedFieldColumn)), urlLineEdit);

	QDataWidgetMapper* urlMapper = new QDataWidgetMapper(this);
	urlMapper->setModel(m_model);
	urlMapper->addMapping(urlLineEdit, GlyphRolesTableModel::s_mappedFieldColumn);

	QObject::connect(urlLineEdit, &BindingLineEdit::ValueChangedByUser, urlMapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappersAndRows[urlMapper] = SynGlyphX::DataMappingGlyph::MappableField::URL;

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::Stretch);

	addTab(tableView, tr("URL"));
}

void DataBindingTablesWidget::CreateTagAndDescriptionWidget() {

	SynGlyphX::VerticalTabOrderTableView* tableView = CreateSubsetTableView({ SynGlyphX::DataMappingGlyph::MappableField::Tag, SynGlyphX::DataMappingGlyph::MappableField::Description },
												  { GlyphRolesTableModel::s_propertyNameColumn, GlyphRolesTableModel::s_mappedFieldColumn });
	SynGlyphX::TableSubsetProxyModel* proxyModel = dynamic_cast<SynGlyphX::TableSubsetProxyModel*>(tableView->model());

	BindingLineEdit* tagLineEdit = new BindingLineEdit(m_model, tableView);
	tagLineEdit->layout()->setContentsMargins(s_cellMargins);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(SynGlyphX::DataMappingGlyph::MappableField::Tag, GlyphRolesTableModel::s_mappedFieldColumn)), tagLineEdit);

	QDataWidgetMapper* tagMapper = new QDataWidgetMapper(this);
	tagMapper->setModel(m_model);
	tagMapper->addMapping(tagLineEdit, GlyphRolesTableModel::s_mappedFieldColumn);

	QObject::connect(tagLineEdit, &BindingLineEdit::ValueChangedByUser, tagMapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappersAndRows[tagMapper] = SynGlyphX::DataMappingGlyph::MappableField::Tag;

	BindingLineEdit* descriptionBindingLineEdit = new BindingLineEdit(m_model, tableView);
	descriptionBindingLineEdit->layout()->setContentsMargins(s_cellMargins);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(SynGlyphX::DataMappingGlyph::MappableField::Description, GlyphRolesTableModel::s_mappedFieldColumn)), descriptionBindingLineEdit);

	QDataWidgetMapper* descriptionMapper = new QDataWidgetMapper(this);
	descriptionMapper->setModel(m_model);
	descriptionMapper->addMapping(descriptionBindingLineEdit, GlyphRolesTableModel::s_mappedFieldColumn);

	QObject::connect(descriptionBindingLineEdit, &BindingLineEdit::ValueChangedByUser, descriptionMapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappersAndRows[descriptionMapper] = SynGlyphX::DataMappingGlyph::MappableField::Description;

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::Stretch);

	addTab(tableView, tr("Tag && Description"));
}

void DataBindingTablesWidget::CreateBasePropertiesTable() {

	SynGlyphX::VerticalTabOrderTableView* tableView = CreateSubsetTableView({ SynGlyphX::DataMappingGlyph::MappableField::PositionX,
													SynGlyphX::DataMappingGlyph::MappableField::PositionY, 
													SynGlyphX::DataMappingGlyph::MappableField::PositionZ,
													SynGlyphX::DataMappingGlyph::MappableField::RotationX,
													SynGlyphX::DataMappingGlyph::MappableField::RotationY,
													SynGlyphX::DataMappingGlyph::MappableField::RotationZ,
													SynGlyphX::DataMappingGlyph::MappableField::ScaleX,
													SynGlyphX::DataMappingGlyph::MappableField::ScaleY,
													SynGlyphX::DataMappingGlyph::MappableField::ScaleZ,
													SynGlyphX::DataMappingGlyph::MappableField::Color,
													SynGlyphX::DataMappingGlyph::MappableField::Transparency });

	//Position
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::PositionX, -10000.0, 10000.0, true);
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::PositionY, -10000.0, 10000.0, true);
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::PositionZ, 0.0, 10000.0);

	//Rotation
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::RotationX, -360.0, 360.0);
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::RotationY, -360.0, 360.0);
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::RotationZ, -360.0, 360.0);

	//Scale
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::ScaleX, 0.0, 1000.0);
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::ScaleY, 0.0, 1000.0);
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::ScaleZ, 0.0, 1000.0);

	//Color & Transparency
	CreateColorPropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::Color);
	CreateDoublePropertyWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::Transparency, 0.0, 255.0);
	//CreateIntegerPropertyWidgets(gridLayout, 10);

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(3, QHeaderView::Stretch);

	addTab(tableView, tr("Base Properties"));
}

SynGlyphX::VerticalTabOrderTableView* DataBindingTablesWidget::CreateSubsetTableView(const SynGlyphX::TableSubsetProxyModel::Subset& rowSubset, const SynGlyphX::TableSubsetProxyModel::Subset& columnSubset) {

	SynGlyphX::VerticalTabOrderTableView* tableView = new SynGlyphX::VerticalTabOrderTableView(this);

	SynGlyphX::TableSubsetProxyModel* proxyModel = new SynGlyphX::TableSubsetProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->SetRowSubset(rowSubset);
	proxyModel->SetColumnSubset(columnSubset);

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionsClickable(false);
	horizontalHeader->setStretchLastSection(true);

	QHeaderView* verticalHeader = tableView->verticalHeader();
	verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
	verticalHeader->hide();
	
	tableView->setSortingEnabled(false);
	tableView->setSelectionMode(QAbstractItemView::NoSelection);
	tableView->setModel(proxyModel);

	return tableView;
}

QDataWidgetMapper* DataBindingTablesWidget::AddRowOfWidgetsToTable(SynGlyphX::VerticalTabOrderTableView* tableView, QWidget* valueWidget, QWidget* valueWidgetToMap, MappingFunctionWidget* mappingFunctionWidget, int modelRow, bool addToPositionXYList) {

	std::array<QDataWidgetMapper*, 3> mappers;
	for (int i = 0; i < 3; ++i) {

		mappers[i] = new QDataWidgetMapper(this);
		mappers[i]->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
		mappers[i]->setModel(m_model);
	}

	BindingLineEdit* inputBindingLineEdit = new BindingLineEdit(m_model, tableView, SynGlyphX::MappingFunctionData::Input::Numeric);

	mappers[0]->addMapping(valueWidgetToMap, GlyphRolesTableModel::s_valueColumn);
	mappers[1]->addMapping(mappingFunctionWidget, GlyphRolesTableModel::s_mappingDataColumn);
	mappers[2]->addMapping(inputBindingLineEdit, GlyphRolesTableModel::s_mappedFieldColumn);

	for (int i = 0; i < 3; ++i) {

		m_dataWidgetMappersAndRows[mappers[i]] = modelRow;
	}

	valueWidget->layout()->setContentsMargins(s_cellMargins);
	mappingFunctionWidget->layout()->setContentsMargins(s_cellMargins);
	inputBindingLineEdit->layout()->setContentsMargins(s_cellMargins);

	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::FunctionChanged, mappers[1], &QDataWidgetMapper::submit);
	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::SupportedInputChanged, inputBindingLineEdit, &BindingLineEdit::SetAcceptedInputTypes);
	QObject::connect(inputBindingLineEdit, &BindingLineEdit::ValueChangedByUser, mappers[2], &QDataWidgetMapper::submit);

	SynGlyphX::TableSubsetProxyModel* proxyModel = dynamic_cast<SynGlyphX::TableSubsetProxyModel*>(tableView->model());

	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(modelRow, GlyphRolesTableModel::s_valueColumn)), valueWidget);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(modelRow, GlyphRolesTableModel::s_mappingDataColumn)), mappingFunctionWidget);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(modelRow, GlyphRolesTableModel::s_mappedFieldColumn)), inputBindingLineEdit);

	if (addToPositionXYList) {

		m_positionXYMinMaxWidgets.push_back(valueWidget);
		m_positionXYMinMaxWidgets.push_back(mappingFunctionWidget);
	}

	return mappers[0];
}

void DataBindingTablesWidget::CreateIntegerPropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow, int min, int max) {

	SynGlyphX::IntMinMaxWidget* minMaxWidget = new SynGlyphX::IntMinMaxWidget(tableView);
	minMaxWidget->SetKeyboardTracking(false);
	minMaxWidget->SetRange(min, max);
	
	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Numeric, m_model, modelRow, tableView);
	mappingFunctionWidget->SetDialogOutputMinMax(min, max);

	QDataWidgetMapper* mapper = AddRowOfWidgetsToTable(tableView, minMaxWidget, minMaxWidget, mappingFunctionWidget, modelRow);

	QObject::connect(minMaxWidget, &SynGlyphX::IntMinMaxWidget::ValueChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::CreateDoublePropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow, double min, double max, bool addToPositionXYList) {

	SynGlyphX::DoubleMinMaxWidget* minMaxWidget = new SynGlyphX::DoubleMinMaxWidget(tableView);
	minMaxWidget->SetKeyboardTracking(false);
	minMaxWidget->SetRange(min, max);
	minMaxWidget->SetDecimals(4);
	minMaxWidget->SetStep(0.1);
	
	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Numeric, m_model, modelRow, tableView);
	mappingFunctionWidget->SetDialogOutputMinMax(min, max);
	
	QDataWidgetMapper* mapper = AddRowOfWidgetsToTable(tableView, minMaxWidget, minMaxWidget, mappingFunctionWidget, modelRow, addToPositionXYList);

	QObject::connect(minMaxWidget, &SynGlyphX::DoubleMinMaxWidget::ValueChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::CreateColorPropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow) {

	SynGlyphX::ColorMinMaxWidget* minMaxWidget = new SynGlyphX::ColorMinMaxWidget(false, tableView);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Color, m_model, modelRow, this);
	QDataWidgetMapper* mapper = AddRowOfWidgetsToTable(tableView, minMaxWidget, minMaxWidget, mappingFunctionWidget, modelRow);

	QObject::connect(minMaxWidget, &SynGlyphX::ColorMinMaxWidget::ValueChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::CreateGeometryShapePropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow) {

	SynGlyphX::GlyphShapeComboBox* comboBox = new SynGlyphX::GlyphShapeComboBox(tableView);
	SynGlyphX::StretchSurroundedWidget* stretchSurroundedWidget = new SynGlyphX::StretchSurroundedWidget(SynGlyphX::StretchSurroundedWidget::All, comboBox, this);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::GeometryShape, m_model, modelRow, tableView);
	QDataWidgetMapper* mapper = AddRowOfWidgetsToTable(tableView, stretchSurroundedWidget, comboBox, mappingFunctionWidget, modelRow);

	QObject::connect(comboBox, &SynGlyphX::GlyphShapeComboBox::currentTextChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::CreateVirtualTopologyTypePropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow) {

	SynGlyphX::VirtualTopologyComboBox* comboBox = new SynGlyphX::VirtualTopologyComboBox(tableView);
	SynGlyphX::StretchSurroundedWidget* stretchSurroundedWidget = new SynGlyphX::StretchSurroundedWidget(SynGlyphX::StretchSurroundedWidget::Vertical, comboBox, this);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::VirtualTopology, m_model, modelRow, tableView);
	QDataWidgetMapper* mapper = AddRowOfWidgetsToTable(tableView, stretchSurroundedWidget, comboBox, mappingFunctionWidget, modelRow);

	QObject::connect(comboBox, &SynGlyphX::VirtualTopologyComboBox::currentTextChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::OnModelDataChanged() {

	bool doesModelHaveData = !m_model->IsClear();
	for (int i = 0; i < count(); ++i) {

		widget(i)->setEnabled(doesModelHaveData);
	}

	if (doesModelHaveData) {

		OnBaseObjectChanged();
	}
}

void DataBindingTablesWidget::CommitChanges() {

	for (auto mapperAndRow : m_dataWidgetMappersAndRows) {

		if (mapperAndRow.first != nullptr) {

			mapperAndRow.first->submit();
		}
	}
}

void DataBindingTablesWidget::OnBaseObjectChanged() {

	EnablePositionXYMixMaxWidgets(!(m_model->IsAnyCurrentGlyphRoot() && (m_model->GetDataTransformMapping()->GetBaseObjects()[0].GetType() == SynGlyphX::BaseImage::Type::DownloadedMap)));
}

void DataBindingTablesWidget::EnablePositionXYMixMaxWidgets(bool enable) {

	for (QWidget* widget : m_positionXYMinMaxWidgets) {

		if (widget != nullptr) {
			widget->setEnabled(enable);
		}
	}
}

void DataBindingTablesWidget::OnTorusRatioChanged() {

	if (m_torusRatioSpinBox->hasFocus()) {

		m_torusRatioMapper->submit();
	}
}