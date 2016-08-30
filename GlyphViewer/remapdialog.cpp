#include "remapdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QDataWidgetMapper>
#include "groupboxsinglewidget.h"
#include "tablesubsetproxymodel.h"
#include "doubleminmaxwidget.h"
#include "mappingfunctionwidget.h"
#include "bindinglineedit.h"
#include "baseimage.h"
#include "datatransformmodel.h"
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>

RemapDialog::RemapDialog(SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping, DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent)
	: QDialog(parent)
{
	setMinimumWidth(850);

	m_dataTransformModel = new SynGlyphX::DataTransformModel(this);
	m_dataTransformModel->SetDataEngineConnection(dataEngineConnection);
	m_glyphRolesModel = new GlyphRolesTableModel(m_dataTransformModel, this);
	m_dataTransformModel->ChangeMapping(*dataTransformMapping);

	setWindowTitle(tr("Change X, Y, Z Position Axes"));

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	SynGlyphX::VerticalTabOrderTableView* tableView = CreateTableView();
	mainLayout->addWidget(tableView);

	SynGlyphX::InputTable tableInDataStats =
		const_cast<SynGlyphX::DataTransformMapping*>(dataTransformMapping.get())->GetInputFieldManager()->GetFieldMap().begin()->second;
		
		/*dataTransformMapping->GetGlyphGraphs().begin()->second->GetInputFields().begin()->second;*/
	m_dataStatsModel = new SynGlyphX::DataStatsModel(tableInDataStats, m_dataTransformModel->GetTableStatsMap().at(tableInDataStats), this);
	QTableView* dataStatsView = new QTableView(this);
	dataStatsView->setModel(m_dataStatsModel);
	dataStatsView->setSelectionMode(QAbstractItemView::SingleSelection);
	dataStatsView->setSelectionBehavior(QAbstractItemView::SelectRows);
	dataStatsView->setDragEnabled(true);
	dataStatsView->setDragDropMode(QAbstractItemView::DragDrop);
	dataStatsView->setDropIndicatorShown(true);
	dataStatsView->setAcceptDrops(false);
	dataStatsView->verticalHeader()->hide();
	dataStatsView->resizeColumnsToContents();
	dataStatsView->resizeRowsToContents();

	mainLayout->addWidget(dataStatsView);

	m_saveFilenameEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileSave, this);
	m_saveFilenameEdit->SetFilters("SynGlyphX Data Transform(*.sdt)");
	m_saveFilenameEdit->setContentsMargins(4, 4, 4, 4);
	SynGlyphX::GroupBoxSingleWidget* saveFilenameGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Filename"), m_saveFilenameEdit, this);
	mainLayout->addWidget(saveFilenameGroupBox);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);

	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &RemapDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &RemapDialog::reject);

	setLayout(mainLayout);

	QModelIndexList modelIndexList;
	modelIndexList.push_back(m_dataTransformModel->index(0, 0));
	m_glyphRolesModel->SetSelectedGlyphTreeIndexes(modelIndexList);
}

RemapDialog::~RemapDialog()
{

}

void RemapDialog::SetSaveFilename(const QString& saveFilename) {

	QSettings settings;
	settings.beginGroup("Remap");
	QString saveDirectory = settings.value("saveDirectory", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
	settings.endGroup();

	QFileInfo saveFilenameInfo(saveFilename);

	m_saveFilenameEdit->SetText(QDir::toNativeSeparators(saveDirectory + "/" + saveFilenameInfo.completeBaseName() + "_remap.sdt"));
}

QString RemapDialog::GetSaveFilename() const {

	return QDir::toNativeSeparators(m_saveFilenameEdit->GetText());
}

void RemapDialog::accept() {

	QString saveFilename = m_saveFilenameEdit->GetText();
	if (saveFilename.isEmpty()) {

		QMessageBox::warning(this, tr("Position Remapping"), tr("The remapped visualization must be given a filename that isn't empty so that it can be saved."));
		return;
	}

	if (QFileInfo::exists(saveFilename)) {

		if (QMessageBox::question(this, tr("Position Remapping"), tr("The filename already exists.  Do you wish to overwrite it?")) == QMessageBox::No) {

			return;
		}
	}

	QFileInfo saveFilenameInfo(saveFilename);
	QSettings settings;
	settings.beginGroup("Remap");
	settings.setValue("saveDirectory", saveFilenameInfo.absolutePath());
	settings.endGroup();

	QDialog::accept();
}

SynGlyphX::DataTransformMapping::ConstSharedPtr RemapDialog::GetNewMapping() const {

	return m_dataTransformModel->GetDataMapping();
}

SynGlyphX::VerticalTabOrderTableView* RemapDialog::CreateTableView() {

	SynGlyphX::VerticalTabOrderTableView* tableView = new SynGlyphX::VerticalTabOrderTableView(this);

	SynGlyphX::TableSubsetProxyModel* proxyModel = new SynGlyphX::TableSubsetProxyModel(this);
	proxyModel->setSourceModel(m_glyphRolesModel);
	proxyModel->SetRowSubset({ SynGlyphX::DataMappingGlyph::MappableField::PositionX,
		SynGlyphX::DataMappingGlyph::MappableField::PositionY,
		SynGlyphX::DataMappingGlyph::MappableField::PositionZ });
	proxyModel->SetColumnSubset(SynGlyphX::TableSubsetProxyModel::Subset());

	tableView->setSortingEnabled(false);
	tableView->setSelectionMode(QAbstractItemView::NoSelection);
	tableView->setModel(proxyModel);

	QHeaderView* horizontalHeader = tableView->horizontalHeader();
	horizontalHeader->setSectionsClickable(false);
	horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
	horizontalHeader->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);
	horizontalHeader->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);
	horizontalHeader->setSectionResizeMode(3, QHeaderView::ResizeMode::Stretch);

	QHeaderView* verticalHeader = tableView->verticalHeader();
	verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
	verticalHeader->hide();

	bool enablePositionXY = (m_dataTransformModel->GetDataMapping()->GetBaseObjects()[0].GetType() != SynGlyphX::BaseImage::Type::DownloadedMap);
	AddRowOfWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::PositionX, -1000.0, 1000.0, enablePositionXY);
	AddRowOfWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::PositionY, -1000.0, 1000.0, enablePositionXY);
	AddRowOfWidgets(tableView, SynGlyphX::DataMappingGlyph::MappableField::PositionZ, 0.0, 1000.0);

	return tableView;
}

void RemapDialog::AddRowOfWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow, double min, double max, bool enable) {

	SynGlyphX::DoubleMinMaxWidget* minMaxWidget = new SynGlyphX::DoubleMinMaxWidget(tableView);
	minMaxWidget->SetKeyboardTracking(false);
	minMaxWidget->SetRange(min, max);
	minMaxWidget->SetDecimals(4);
	minMaxWidget->SetStep(0.1);

	MappingFunctionWidget* mappingFunctionWidget = new MappingFunctionWidget(MappingFunctionWidget::KeyType::Numeric, m_glyphRolesModel, modelRow, tableView);
	mappingFunctionWidget->SetDialogOutputMinMax(min, max);

	std::array<QDataWidgetMapper*, 3> mappers;
	for (int i = 0; i < 3; ++i) {

		mappers[i] = new QDataWidgetMapper(this);
		mappers[i]->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
		mappers[i]->setModel(m_glyphRolesModel);
	}

	BindingLineEdit* inputBindingLineEdit = new BindingLineEdit(m_glyphRolesModel, tableView, SynGlyphX::MappingFunctionData::Input::Numeric);

	mappers[0]->addMapping(minMaxWidget, GlyphRolesTableModel::s_valueColumn);
	mappers[1]->addMapping(mappingFunctionWidget, GlyphRolesTableModel::s_mappingDataColumn);
	mappers[2]->addMapping(inputBindingLineEdit, GlyphRolesTableModel::s_mappedFieldColumn);

	for (int i = 0; i < 3; ++i) {

		mappers[i]->setCurrentIndex(modelRow);
	}

	QMargins cellMargins(8, 5, 8, 5);
	minMaxWidget->layout()->setContentsMargins(cellMargins);
	mappingFunctionWidget->layout()->setContentsMargins(cellMargins);
	inputBindingLineEdit->layout()->setContentsMargins(cellMargins);

	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::FunctionChanged, mappers[1], &QDataWidgetMapper::submit);
	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::SupportedInputChanged, inputBindingLineEdit, &BindingLineEdit::SetAcceptedInputTypes);
	QObject::connect(inputBindingLineEdit, &BindingLineEdit::ValueChangedByUser, mappers[2], &QDataWidgetMapper::submit);

	SynGlyphX::TableSubsetProxyModel* proxyModel = dynamic_cast<SynGlyphX::TableSubsetProxyModel*>(tableView->model());

	tableView->setIndexWidget(proxyModel->mapFromSource(m_glyphRolesModel->index(modelRow, GlyphRolesTableModel::s_valueColumn)), minMaxWidget);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_glyphRolesModel->index(modelRow, GlyphRolesTableModel::s_mappingDataColumn)), mappingFunctionWidget);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_glyphRolesModel->index(modelRow, GlyphRolesTableModel::s_mappedFieldColumn)), inputBindingLineEdit);

	QObject::connect(minMaxWidget, &SynGlyphX::DoubleMinMaxWidget::ValueChanged, mappers[0], &QDataWidgetMapper::submit);

	minMaxWidget->setEnabled(enable);
}