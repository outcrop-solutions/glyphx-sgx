#include "remapdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QDataWidgetMapper>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGroupBox>
#include "groupboxsinglewidget.h"
#include "tablesubsetproxymodel.h"
#include "doubleminmaxwidget.h"
#include "mappingfunctionwidget.h"
#include "bindinglineedit.h"
#include "baseimage.h"
#include "datatransformmodel.h"
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include "datastatsmodel.h"
#include "browselineedit.h"
#include "glyphrolestablemodel.h"
#include "verticaltabordertableview.h"
#include "SceneViewer/sceneviewer.h"
#include "Glyph3DSceneExport.h"

//RemapDialog::RemapDialog(SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping, DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent)
RemapDialog::RemapDialog(SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping, boost::uuids::uuid connectionID, QWidget *parent)
	: QDialog(parent),
	m_selectedGlyph(std::numeric_limits<unsigned int>::max())
{
	setMinimumWidth(850);

	m_dataTransformModel = new SynGlyphX::DataTransformModel(this);
	//m_dataTransformModel->SetDataEngineConnection(dataEngineConnection);
	m_dataTransformModel->SetConnectionID(connectionID);
	m_glyphRolesModel = new GlyphRolesTableModel(m_dataTransformModel, this);
	m_dataTransformModel->ChangeMapping(*dataTransformMapping);

	setWindowTitle(tr("Change X, Y, Z Position Axes"));

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* horizontalLayout = new QHBoxLayout(this);

	horizontalLayout->addWidget(CreateGlyphSwitchWidget());

	QVBoxLayout* rightLayout = new QVBoxLayout(this);

	SynGlyphX::VerticalTabOrderTableView* tableView = CreateTableView();
	rightLayout->addWidget(tableView);

	m_dataStatsView = new QTableView(this);
	m_dataStatsView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_dataStatsView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_dataStatsView->setDragEnabled(true);
	m_dataStatsView->setDragDropMode(QAbstractItemView::DragDrop);
	m_dataStatsView->setDropIndicatorShown(true);
	m_dataStatsView->setAcceptDrops(false);
	m_dataStatsView->verticalHeader()->hide();
	m_dataStatsView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_dataStatsView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	rightLayout->addWidget(m_dataStatsView);

	m_saveFilenameEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileSave, this);
	m_saveFilenameEdit->SetFilters("SynGlyphX Data Transform(*.sdt)");
	m_saveFilenameEdit->setContentsMargins(4, 4, 4, 4);
	SynGlyphX::GroupBoxSingleWidget* saveFilenameGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Filename"), m_saveFilenameEdit, this);
	rightLayout->addWidget(saveFilenameGroupBox);

	horizontalLayout->addLayout(rightLayout);

	mainLayout->addLayout(horizontalLayout);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);

	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &RemapDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &RemapDialog::reject);

	setLayout(mainLayout);

	CreateDataStatsModels();
}

RemapDialog::~RemapDialog()
{
	
}

void RemapDialog::showEvent(QShowEvent* event) {

	QDialog::showEvent(event);
	if (m_selectedGlyph == std::numeric_limits<unsigned int>::max()) {

		SwitchGlyph(0);
	}
}

void RemapDialog::CreateDataStatsModels() {

	for (const auto& glyph : m_dataTransformModel->GetDataMapping()->GetGlyphGraphs()) {

		SynGlyphX::InputTable table = m_dataTransformModel->GetDataMapping()->GetInputTable(glyph.first);
		if (m_dataStatsModels.count(table) == 0) {

			SynGlyphX::DataStatsModel* dataStatsModel = new SynGlyphX::DataStatsModel(table, m_dataTransformModel->GetTableStatsMap().at(table), this);
			int i = 0;

			for (const auto& fieldStats : m_dataTransformModel->GetTableStatsMap().at(table)) {

				SynGlyphX::HashID seed = table.GetHashID();
				SynGlyphX::CombineHashID(seed, fieldStats.at(0).toStdWString());
				std::wstring hashid = std::to_wstring(seed);
				if (m_dataTransformModel->HasFieldProperties(hashid)){
					SynGlyphX::FieldProperties fp = m_dataTransformModel->GetFieldProperties(hashid);
					fp.AddStatsToField(fieldStats);
					dataStatsModel->setFormattedData(dataStatsModel->index(i, 2), fp.transformData(2));
					dataStatsModel->setFormattedData(dataStatsModel->index(i, 3), fp.transformData(3));
				}
				i++;
			}
			m_dataStatsModels[table] = dataStatsModel;
		}
	}
}

QWidget* RemapDialog::CreateGlyphSwitchWidget() {

	QGroupBox* glyphSelectorGroupBox = new QGroupBox(tr("Glyph"), this);

	QGridLayout* glyphSelectorLayout = new QGridLayout(this);

	m_glyph3DView = new SynGlyphX::SceneViewer(this, SynGlyphX::ViewerMode::SingleGlyph);
	m_glyph3DView->setMinimumSize(256, 256);
	glyphSelectorLayout->addWidget(m_glyph3DView, 0, 0, 5, 5);

	m_previousGlyphButton = new QPushButton(this);
	m_previousGlyphButton->setToolTip(tr("Previous Glyph"));
	m_previousGlyphButton->setIcon(QIcon(":SGXGUI/Resources/left_arrow.png"));
	glyphSelectorLayout->addWidget(m_previousGlyphButton, 5, 0);
	QObject::connect(m_previousGlyphButton, &QPushButton::clicked, this, &RemapDialog::SwitchToPreviousGlyph);

	m_nextGlyphButton = new QPushButton(this);
	m_nextGlyphButton->setToolTip(tr("Next Glyph"));
	m_nextGlyphButton->setIcon(QIcon(":SGXGUI/Resources/right_arrow.png"));
	glyphSelectorLayout->addWidget(m_nextGlyphButton, 5, 4);
	QObject::connect(m_nextGlyphButton, &QPushButton::clicked, this, &RemapDialog::SwitchToNextGlyph);

	glyphSelectorGroupBox->setLayout(glyphSelectorLayout);

	return glyphSelectorGroupBox;
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

	SendMinMaxFromUIToModel();

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
	tableView->setMinimumWidth(700);
	tableView->setFixedHeight(150);

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

	m_minMaxWidgets.push_back(minMaxWidget);

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

	QObject::connect(minMaxWidget, &SynGlyphX::DoubleMinMaxWidget::ValueChanged, mappers[0], &QDataWidgetMapper::submit);
	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::FunctionChanged, mappers[1], &QDataWidgetMapper::submit);
	QObject::connect(mappingFunctionWidget, &MappingFunctionWidget::SupportedInputChanged, inputBindingLineEdit, &BindingLineEdit::SetAcceptedInputTypes);
	QObject::connect(inputBindingLineEdit, &BindingLineEdit::ValueChangedByUser, mappers[2], &QDataWidgetMapper::submit);

	SynGlyphX::TableSubsetProxyModel* proxyModel = dynamic_cast<SynGlyphX::TableSubsetProxyModel*>(tableView->model());

	tableView->setIndexWidget(proxyModel->mapFromSource(m_glyphRolesModel->index(modelRow, GlyphRolesTableModel::s_valueColumn)), minMaxWidget);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_glyphRolesModel->index(modelRow, GlyphRolesTableModel::s_mappingDataColumn)), mappingFunctionWidget);
	tableView->setIndexWidget(proxyModel->mapFromSource(m_glyphRolesModel->index(modelRow, GlyphRolesTableModel::s_mappedFieldColumn)), inputBindingLineEdit);

	minMaxWidget->setEnabled(enable);
}

void RemapDialog::SendMinMaxFromUIToModel() {

	for (unsigned int i = 0; i < 3; ++i) {

		QModelIndex indexToSet = m_glyphRolesModel->index(i, GlyphRolesTableModel::s_valueColumn);
		m_glyphRolesModel->setData(indexToSet, QVariant::fromValue<SynGlyphX::DoubleMinDiff>(m_minMaxWidgets[i]->GetValue()));
	}
}

void RemapDialog::SendMinMaxFromModelToUI() {

	for (unsigned int i = 0; i < 3; ++i) {

		QModelIndex indexToGet = m_glyphRolesModel->index(i, GlyphRolesTableModel::s_valueColumn);
		m_minMaxWidgets[i]->SetValue(m_glyphRolesModel->data(indexToGet, 2).value<SynGlyphX::DoubleMinDiff>());
	}
}

void RemapDialog::SwitchToPreviousGlyph() {

	SwitchGlyph(m_selectedGlyph - 1);
}

void RemapDialog::SwitchToNextGlyph() {

	SwitchGlyph(m_selectedGlyph + 1);
}

void RemapDialog::SwitchGlyph(unsigned int newIndex) {

	SendMinMaxFromUIToModel();

	m_selectedGlyph = newIndex;

	Rebuild3DGlyph();

	QModelIndexList modelIndexList;
	modelIndexList.push_back(m_dataTransformModel->index(m_selectedGlyph, 0));
	m_glyphRolesModel->SetSelectedGlyphTreeIndexes(modelIndexList);

	auto iT = m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().begin();
	std::advance(iT, m_selectedGlyph);

	SynGlyphX::InputTable table = m_dataTransformModel->GetDataMapping()->GetInputTable(iT->first);
	SynGlyphX::DataStatsModel* dataStatsModel = m_dataStatsModels.at(table);
	m_dataStatsView->setModel(dataStatsModel);

	SendMinMaxFromModelToUI();

	m_previousGlyphButton->setEnabled(m_selectedGlyph != 0);
	m_nextGlyphButton->setEnabled(m_selectedGlyph < m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().size() - 1);
}

void RemapDialog::Rebuild3DGlyph() {

	auto idGlyphPair = m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().begin();
	std::advance(idGlyphPair, m_selectedGlyph);

	m_glyph3DView->makeCurrent();
	m_glyph3DView->clearScene();

	SynGlyphX::GlyphScene& scene = m_glyph3DView->getScene();
	scene.beginAdding(idGlyphPair->second->size());
	SynGlyphX::Glyph3DSceneExport::ExportMaxGlyphTo3DScene(*idGlyphPair->second, scene);
	scene.finishAdding();

	m_glyph3DView->resetCamera();
}
