#include "databindingtableswidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtCore/QModelIndex>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include "colorminmaxwidget.h"
#include "doubleminmaxwidget.h"
#include "intminmaxwidget.h"
#include "datamappingfunction.h"
#include "mappingfunctionwidget.h"
#include "glyphenumcombobox.h"
#include "stretchsurroundedwidget.h"

DataBindingTablesWidget::DataBindingTablesWidget(GlyphRolesTableModel* model, QWidget *parent)
	: QTabWidget(parent),
	m_model(model)
{
	CreateBasePropertiesTable();
	CreateTagAndDescriptionWidget();
	CreateAnimationTable();
	CreateGeometryTopologyTab();

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

	QTableView* tableView = CreateSubsetTableView({ 16, 17, 18 });
	tableView->setSpan(2, 1, 1, 3);

	CreateVirtualTopologyTypePropertyWidgets(tableView, 16);
	CreateGeometryShapePropertyWidgets(tableView, 17);

	tableView->resizeColumnToContents(1);

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::Fixed);
	horizontalHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(3, QHeaderView::Stretch);

	m_nonMappableGeometryWidget = new SynGlyphX::NonMappableGeometryWidget(tableView);
	QWidget* widget = new QWidget(this);
	QHBoxLayout* widgetLayout = new QHBoxLayout(widget);
	widgetLayout->addWidget(m_nonMappableGeometryWidget);
	widgetLayout->addStretch(1);
	widget->setLayout(widgetLayout);

	SynGlyphX::TableSubsetProxyModel* proxyModel = dynamic_cast<SynGlyphX::TableSubsetProxyModel*>(tableView->model());
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(18, 1)), widget);

	addTab(tableView, tr("Geometry && Topology"));

	QObject::connect(m_nonMappableGeometryWidget, &SynGlyphX::NonMappableGeometryWidget::PropertiesChanged, this, &DataBindingTablesWidget::OnNonMappablePropertiesUpdated);
}

void DataBindingTablesWidget::CreateAnimationTable() {

	QTableView* tableView = CreateSubsetTableView({ 13, 14, 15 });

	CreateDoublePropertyWidgets(tableView, 13, -1000.0, 1000.0);
	CreateDoublePropertyWidgets(tableView, 14, -1000.0, 1000.0);
	CreateDoublePropertyWidgets(tableView, 15, -1000.0, 1000.0);

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(3, QHeaderView::Stretch);

	addTab(tableView, tr("Animation"));
}

void DataBindingTablesWidget::CreateTagAndDescriptionWidget() {

	QTableView* tableView = CreateSubsetTableView({ 11, 12 }, { 0, 3 });
	SynGlyphX::TableSubsetProxyModel* proxyModel = dynamic_cast<SynGlyphX::TableSubsetProxyModel*>(tableView->model());

	m_tagLineEdit = new BindingLineEdit(m_model, tableView);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(11, 3)), m_tagLineEdit);

	QDataWidgetMapper* tagMapper = new QDataWidgetMapper(this);
	tagMapper->setModel(m_model);
	tagMapper->addMapping(m_tagLineEdit, 3);  

	QObject::connect(m_tagLineEdit, &BindingLineEdit::ValueChangedByUser, tagMapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappersAndRows[tagMapper] = 11;

	m_descriptionEdit = new SynGlyphX::RichTextEditor("", tableView);
	m_descriptionEdit->setEnabled(false);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(12, 3)), m_descriptionEdit);

	QDataWidgetMapper* descriptionMapper = new QDataWidgetMapper(this);
	descriptionMapper->setModel(m_model);
	descriptionMapper->addMapping(m_descriptionEdit, 3);

	//QObject::connect(m_descriptionEdit, &BindingLineEdit::ValueChangedByUser, mapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappersAndRows[descriptionMapper] = 12;

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::Stretch);

	addTab(tableView, tr("Tag && Description"));
}

void DataBindingTablesWidget::CreateBasePropertiesTable() {

	QTableView* tableView = CreateSubsetTableView({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });

	//Position
	CreateDoublePropertyWidgets(tableView, 0, -1000.0, 1000.0, true);
	CreateDoublePropertyWidgets(tableView, 1, -1000.0, 1000.0, true);
	CreateDoublePropertyWidgets(tableView, 2, 0.0, 1000.0);

	//Rotation
	CreateDoublePropertyWidgets(tableView, 3, -360.0, 360.0);
	CreateDoublePropertyWidgets(tableView, 4, -360.0, 360.0);
	CreateDoublePropertyWidgets(tableView, 5, -360.0, 360.0);

	//Scale
	CreateDoublePropertyWidgets(tableView, 6, 0.0, 1000.0);
	CreateDoublePropertyWidgets(tableView, 7, 0.0, 1000.0);
	CreateDoublePropertyWidgets(tableView, 8, 0.0, 1000.0);

	//Color & Transparency
	CreateColorPropertyWidgets(tableView, 9);
	CreateDoublePropertyWidgets(tableView, 10, 0.0, 255.0);
	//CreateIntegerPropertyWidgets(gridLayout, 10);

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	horizontalHeader->setSectionResizeMode(3, QHeaderView::Stretch);

	addTab(tableView, tr("Base Properties"));
}

QTableView* DataBindingTablesWidget::CreateSubsetTableView(const SynGlyphX::TableSubsetProxyModel::Subset& rowSubset, const SynGlyphX::TableSubsetProxyModel::Subset& columnSubset) {

	QTableView* tableView = new QTableView(this);

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

void DataBindingTablesWidget::AddRowOfWidgetsToTable(QTableView* tableView, const QList<QWidget*> widgets, int modelRow, bool addToPositionXYList) {

	SynGlyphX::TableSubsetProxyModel* proxyModel = dynamic_cast<SynGlyphX::TableSubsetProxyModel*>(tableView->model());

	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(modelRow, 1)), widgets[0]);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(modelRow, 2)), widgets[1]);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_model->index(modelRow, 3)), widgets[2]);

	if (addToPositionXYList) {

		m_positionXYMinMaxWidgets.push_back(widgets[0]);
		m_positionXYMinMaxWidgets.push_back(widgets[1]);
	}
}

QDataWidgetMapper* DataBindingTablesWidget::CreateMapper(QWidget* parent, QWidget* valueWidget, MappingFunctionWidget* mappingFunctionWidget, int modelRow) {

	QDataWidgetMapper* mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

	BindingLineEdit* inputBindingLineEdit = new BindingLineEdit(m_model, parent, SynGlyphX::MappingFunctionData::Input::Numeric);

	mapper->setModel(m_model);

	mapper->addMapping(valueWidget, 1);
	mapper->addMapping(mappingFunctionWidget, 2);
	mapper->addMapping(inputBindingLineEdit, 3);

	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::FunctionChanged, mapper, &QDataWidgetMapper::submit);
	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::SupportedInputChanged, inputBindingLineEdit, &BindingLineEdit::SetAcceptedInputTypes);
	QObject::connect(inputBindingLineEdit, &BindingLineEdit::ValueChangedByUser, mapper, &QDataWidgetMapper::submit);

	m_dataWidgetMappersAndRows[mapper] = modelRow;

	return mapper;
}

void DataBindingTablesWidget::CreateIntegerPropertyWidgets(QTableView* tableView, int modelRow, int min, int max) {

	SynGlyphX::IntMinMaxWidget* minMaxWidget = new SynGlyphX::IntMinMaxWidget(tableView);
	minMaxWidget->SetKeyboardTracking(false);
	minMaxWidget->SetRange(min, max);
	
	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Numeric, m_model, modelRow, tableView);
	mappingFunctionWidget->SetDialogOutputMinMax(min, max);
	QDataWidgetMapper* mapper = CreateMapper(tableView, minMaxWidget, mappingFunctionWidget, modelRow);
	QList<QWidget*> widgetList;
	widgetList.push_back(mapper->mappedWidgetAt(1));
	widgetList.push_back(mapper->mappedWidgetAt(2));
	widgetList.push_back(mapper->mappedWidgetAt(3));
	AddRowOfWidgetsToTable(tableView, widgetList, modelRow);

	QObject::connect(minMaxWidget, &SynGlyphX::IntMinMaxWidget::ValueChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::CreateDoublePropertyWidgets(QTableView* tableView, int modelRow, double min, double max, bool addToPositionXYList) {

	SynGlyphX::DoubleMinMaxWidget* minMaxWidget = new SynGlyphX::DoubleMinMaxWidget(tableView);
	minMaxWidget->SetKeyboardTracking(false);
	minMaxWidget->SetRange(min, max);
	minMaxWidget->SetDecimals(4);
	minMaxWidget->SetStep(0.1);
	
	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Numeric, m_model, modelRow, tableView);
	mappingFunctionWidget->SetDialogOutputMinMax(min, max);
	QDataWidgetMapper* mapper = CreateMapper(tableView, minMaxWidget, mappingFunctionWidget, modelRow);
	QList<QWidget*> widgetList;
	widgetList.push_back(mapper->mappedWidgetAt(1));
	widgetList.push_back(mapper->mappedWidgetAt(2));
	widgetList.push_back(mapper->mappedWidgetAt(3));
	AddRowOfWidgetsToTable(tableView, widgetList, modelRow, addToPositionXYList);

	QObject::connect(minMaxWidget, &SynGlyphX::DoubleMinMaxWidget::ValueChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::CreateColorPropertyWidgets(QTableView* tableView, int modelRow) {

	SynGlyphX::ColorMinMaxWidget* minMaxWidget = new SynGlyphX::ColorMinMaxWidget(false, tableView);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Color, m_model, modelRow, this);
	QDataWidgetMapper* mapper = CreateMapper(tableView, minMaxWidget, mappingFunctionWidget, modelRow);
	QList<QWidget*> widgetList;
	widgetList.push_back(mapper->mappedWidgetAt(1));
	widgetList.push_back(mapper->mappedWidgetAt(2));
	widgetList.push_back(mapper->mappedWidgetAt(3));
	AddRowOfWidgetsToTable(tableView, widgetList, modelRow);

	QObject::connect(minMaxWidget, &SynGlyphX::ColorMinMaxWidget::ValueChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::CreateGeometryShapePropertyWidgets(QTableView* tableView, int modelRow) {

	SynGlyphX::GlyphShapeComboBox* comboBox = new SynGlyphX::GlyphShapeComboBox(tableView);
	SynGlyphX::StretchSurroundedWidget* stretchSurroundedWidget = new SynGlyphX::StretchSurroundedWidget(SynGlyphX::StretchSurroundedWidget::All, comboBox, this);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::GeometryShape, m_model, modelRow, tableView);
	QDataWidgetMapper* mapper = CreateMapper(tableView, comboBox, mappingFunctionWidget, modelRow);
	QList<QWidget*> widgetList;
	widgetList.push_back(stretchSurroundedWidget);
	widgetList.push_back(mapper->mappedWidgetAt(2));
	widgetList.push_back(mapper->mappedWidgetAt(3));
	AddRowOfWidgetsToTable(tableView, widgetList, modelRow);

	QObject::connect(comboBox, &SynGlyphX::GlyphShapeComboBox::currentTextChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::CreateVirtualTopologyTypePropertyWidgets(QTableView* tableView, int modelRow) {

	SynGlyphX::VirtualTopologyComboBox* comboBox = new SynGlyphX::VirtualTopologyComboBox(tableView);
	SynGlyphX::StretchSurroundedWidget* stretchSurroundedWidget = new SynGlyphX::StretchSurroundedWidget(SynGlyphX::StretchSurroundedWidget::Vertical, comboBox, this);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::VirtualTopology, m_model, modelRow, tableView);
	QDataWidgetMapper* mapper = CreateMapper(tableView, comboBox, mappingFunctionWidget, modelRow);
	QList<QWidget*> widgetList;
	widgetList.push_back(stretchSurroundedWidget);
	widgetList.push_back(mapper->mappedWidgetAt(2));
	widgetList.push_back(mapper->mappedWidgetAt(3));
	AddRowOfWidgetsToTable(tableView, widgetList, modelRow);

	QObject::connect(comboBox, &SynGlyphX::VirtualTopologyComboBox::currentTextChanged, mapper, &QDataWidgetMapper::submit);
}

void DataBindingTablesWidget::OnModelDataChanged() {

	bool doesModelHaveData = !m_model->IsClear();
	for (int i = 0; i < count(); ++i) {

		widget(i)->setEnabled(doesModelHaveData);
	}

	if (doesModelHaveData) {

		bool areSignalsBlocked = m_nonMappableGeometryWidget->blockSignals(true);
		m_nonMappableGeometryWidget->SetProperties(m_model->data(m_model->index(18, 1), Qt::EditRole).value<SynGlyphX::NonMappableGeometryProperties>());
		m_nonMappableGeometryWidget->blockSignals(areSignalsBlocked);

		OnBaseObjectChanged();
	}
}

void DataBindingTablesWidget::OnNonMappablePropertiesUpdated() {

	m_model->setData(m_model->index(18, 1), QVariant::fromValue<SynGlyphX::NonMappableGeometryProperties>(m_nonMappableGeometryWidget->GetProperties()));
}

void DataBindingTablesWidget::CommitChanges() {

	for (auto mapperAndRow : m_dataWidgetMappersAndRows) {

		if (mapperAndRow.first != nullptr) {

			mapperAndRow.first->submit();
		}
	}
}

void DataBindingTablesWidget::OnBaseObjectChanged() {

	EnablePositionXYMixMaxWidgets(!(m_model->IsCurrentGlyphRoot() && (m_model->GetDataTransformMapping()->GetBaseObjects()[0].GetType() == SynGlyphX::BaseImage::Type::DownloadedMap)));
}

void DataBindingTablesWidget::EnablePositionXYMixMaxWidgets(bool enable) {

	for (QWidget* widget : m_positionXYMinMaxWidgets) {

		if (widget != nullptr) {
			widget->setEnabled(enable);
		}
	}
}