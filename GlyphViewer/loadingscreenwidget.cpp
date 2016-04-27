#include "loadingscreenwidget.h"
#include "glyphbuilderapplication.h"
#include <QtCore/QDir>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>

QString LoadingScreenWidget::s_glyphEdDir;

LoadingScreenWidget::LoadingScreenWidget(GlyphViewerWindow* mainWindow, QWidget *parent)
	: QFrame(parent),
	m_mainWindow(mainWindow),
	m_currentView(0)
{
	m_sourceDataCache.Setup(GetGlyphEdDir() + QDir::toNativeSeparators("/glyphed.db"));

	SetupVisualizationData();

	setFrameShape(QFrame::Shape::Box);
	setFrameShadow(QFrame::Shadow::Sunken);
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* vizAndFilterLayout = new QHBoxLayout(this);

	m_viewListWidget = new QListWidget(this);
	m_viewListWidget->setSortingEnabled(false);
	for (const auto& visualizationData : m_visualizationData) {

		m_viewListWidget->addItem(visualizationData.m_title);
	}
	m_viewListWidget->setMinimumWidth(m_viewListWidget->sizeHintForColumn(0) + 5);
	QObject::connect(m_viewListWidget, &QListWidget::itemSelectionChanged, this, &LoadingScreenWidget::OnNewViewSelected);

	SynGlyphX::GroupBoxSingleWidget* visualizationGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("View(s)"), m_viewListWidget, this);
	vizAndFilterLayout->addWidget(visualizationGroupBox);

	for (unsigned int i = 0; i < 3; ++i) {

		AddFilterList(vizAndFilterLayout);
	}

	vizAndFilterLayout->addStretch(1);

	QVBoxLayout* logoLayout = new QVBoxLayout(this);
	logoLayout->setContentsMargins(0, 0, 0, 0);

	QLabel* logoLabel = new QLabel(this);
	logoLabel->setPixmap(QPixmap(SynGlyphX::GlyphBuilderApplication::GetLogoLocation(SynGlyphX::GlyphBuilderApplication::NoBorder)));

	logoLayout->addWidget(logoLabel);
	logoLayout->addStretch(1);

	vizAndFilterLayout->addLayout(logoLayout);

	mainLayout->addLayout(vizAndFilterLayout, 1);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);

	QPushButton* loadButton = new QPushButton(tr("Load Visualization"), this);
	buttonLayout->addWidget(loadButton, 1);
	QObject::connect(loadButton, &QPushButton::clicked, this, &LoadingScreenWidget::OnLoadVisualization);

	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);

	m_viewListWidget->selectionModel()->select(m_viewListWidget->model()->index(0, 0), QItemSelectionModel::SelectionFlag::ClearAndSelect);
}

LoadingScreenWidget::~LoadingScreenWidget()
{

}

void LoadingScreenWidget::SetupVisualizationData() {

	QStringList visualizationNames;
	visualizationNames << "Global Admissions View" << "Admissions Counselor View (2016)" << "High School View" << 
		"Class Composition (Diversity)" << "Class Composition (Cohort)" << "Class Composition (Total)" << "Global Dashboard";

	VisualizationData globalAdmissionsView;
	globalAdmissionsView.m_title = visualizationNames[0];
	globalAdmissionsView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Global Admissions/Global Admissions.sdt");
	globalAdmissionsView.m_tableInGlyphEd = "GlobalAdmissions";

	globalAdmissionsView.m_filterTitles.push_back("Admissions Counselor");
	globalAdmissionsView.m_filterFieldNames.push_back("StaffAssigned");
	globalAdmissionsView.m_filterMultiselect.push_back(true);
	globalAdmissionsView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(globalAdmissionsView.m_tableInGlyphEd, "StaffAssigned"));
	globalAdmissionsView.m_filterTitles.push_back("Year(s)");
	globalAdmissionsView.m_filterFieldNames.push_back("Year");
	globalAdmissionsView.m_filterMultiselect.push_back(true);
	globalAdmissionsView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(globalAdmissionsView.m_tableInGlyphEd, "Year"));

	m_visualizationData.push_back(globalAdmissionsView);

	VisualizationData admissionsCounselorView;
	admissionsCounselorView.m_title = visualizationNames[1];
	admissionsCounselorView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Admissions Officer/Admissions Counselor.sdt");
	admissionsCounselorView.m_tableInGlyphEd = "GlobalAdmissions";

	admissionsCounselorView.m_filterTitles.push_back("Admissions Counselor");
	admissionsCounselorView.m_filterFieldNames.push_back("StaffAssigned");
	admissionsCounselorView.m_filterMultiselect.push_back(false);
	admissionsCounselorView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(admissionsCounselorView.m_tableInGlyphEd, "StaffAssigned", "\"Year\"=2016"));
	admissionsCounselorView.m_filterTitles.push_back("Decision Status");
	admissionsCounselorView.m_filterFieldNames.push_back("Last_Decision_Cluster");
	admissionsCounselorView.m_filterMultiselect.push_back(true);
	admissionsCounselorView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(admissionsCounselorView.m_tableInGlyphEd, "Last_Decision_Cluster", "\"Year\"=2016"));
	/*admissionsCounselorView.m_filterTitles.push_back("Year(s)");
	admissionsCounselorView.m_filterMultiselect.push_back(true);
	admissionsCounselorView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("GlobalAdmissions", "Year"));*/

	m_visualizationData.push_back(admissionsCounselorView);

	VisualizationData highSchoolView;
	highSchoolView.m_title = visualizationNames[2];
	highSchoolView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/High School/High School.sdt");
	highSchoolView.m_tableInGlyphEd = "GlobalAdmissions";

	highSchoolView.m_filterTitles.push_back("High School");
	highSchoolView.m_filterFieldNames.push_back("HSNameUnique");
	highSchoolView.m_filterMultiselect.push_back(false);

	QStringList highSchools;
	SourceDataCache::SharedSQLQuery distinctValuesQuery = m_sourceDataCache.CreateDistinctValueAndCountQuery(highSchoolView.m_tableInGlyphEd, "HSNameUnique", "\"Year\"=2016");
	distinctValuesQuery->exec();
	while (distinctValuesQuery->next()) {

		if (distinctValuesQuery->value(1).toULongLong() >= 10) {

			highSchools.push_back(distinctValuesQuery->value(0).toString());
		}
	}
	highSchools.sort(Qt::CaseInsensitive);
	highSchoolView.m_filterValues.push_back(highSchools);
	highSchoolView.m_filterTitles.push_back("Year(s)");
	highSchoolView.m_filterFieldNames.push_back("Year");
	highSchoolView.m_filterMultiselect.push_back(true);
	highSchoolView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(highSchoolView.m_tableInGlyphEd, "Year"));

	m_visualizationData.push_back(highSchoolView);

	QStringList statuses = m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "grouping_title", "\"branch_type\"='Status'");
	statuses.push_back("Cohort Aggregate");

	VisualizationData classCompositionRaceView;
	classCompositionRaceView.m_title = visualizationNames[3];
	classCompositionRaceView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/Class Composition.sdt");
	classCompositionRaceView.m_tableInGlyphEd = "Composition";

	classCompositionRaceView.m_filterTitles.push_back("Race");
	classCompositionRaceView.m_filterFieldNames.push_back("grouping_title");
	classCompositionRaceView.m_filterMultiselect.push_back(true);
	classCompositionRaceView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionRaceView.m_tableInGlyphEd, "grouping_title", "\"branch_type\"='Race'"));
	classCompositionRaceView.m_filterTitles.push_back("Year(s)");
	classCompositionRaceView.m_filterFieldNames.push_back("Year");
	classCompositionRaceView.m_filterMultiselect.push_back(true);
	classCompositionRaceView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionRaceView.m_tableInGlyphEd, "Year"));
	classCompositionRaceView.m_filterTitles.push_back("Decision Status");
	classCompositionRaceView.m_filterFieldNames.push_back("grouping_title_lv2");
	classCompositionRaceView.m_filterMultiselect.push_back(true);
	classCompositionRaceView.m_filterValues.push_back(statuses);

	m_visualizationData.push_back(classCompositionRaceView);

	VisualizationData classCompositionCohortView;
	classCompositionCohortView.m_title = visualizationNames[4];
	classCompositionCohortView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/Class Composition.sdt");
	classCompositionCohortView.m_tableInGlyphEd = "Composition";

	classCompositionCohortView.m_filterTitles.push_back("Cohort");
	classCompositionCohortView.m_filterFieldNames.push_back("grouping_title");
	classCompositionCohortView.m_filterMultiselect.push_back(true);
	classCompositionCohortView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionCohortView.m_tableInGlyphEd, "grouping_title", "\"branch_type\"='Cohort'"));
	classCompositionCohortView.m_filterTitles.push_back("Year(s)");
	classCompositionCohortView.m_filterFieldNames.push_back("Year");
	classCompositionCohortView.m_filterMultiselect.push_back(true);
	classCompositionCohortView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionCohortView.m_tableInGlyphEd, "Year"));
	classCompositionCohortView.m_filterTitles.push_back("Decision Status");
	classCompositionCohortView.m_filterFieldNames.push_back("grouping_title_lv2");
	classCompositionCohortView.m_filterMultiselect.push_back(true);
	classCompositionCohortView.m_filterValues.push_back(statuses);

	m_visualizationData.push_back(classCompositionCohortView);

	VisualizationData classCompositionTotalView;
	classCompositionTotalView.m_title = visualizationNames[5];
	classCompositionTotalView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/Class Composition.sdt");
	classCompositionTotalView.m_tableInGlyphEd = "Composition";

	classCompositionTotalView.m_filterTitles.push_back("Year(s)");
	classCompositionTotalView.m_filterFieldNames.push_back("Year");
	classCompositionTotalView.m_filterMultiselect.push_back(true);
	classCompositionTotalView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionTotalView.m_tableInGlyphEd, "Year"));
	classCompositionTotalView.m_filterTitles.push_back("Decision Status");
	classCompositionTotalView.m_filterFieldNames.push_back("grouping_title");
	classCompositionTotalView.m_filterMultiselect.push_back(true);
	classCompositionTotalView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "grouping_title", "\"branch_type\"='Status'"));

	m_visualizationData.push_back(classCompositionTotalView);

	VisualizationData globalDashboardView;
	globalDashboardView.m_title = visualizationNames[6];
	globalDashboardView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Global Dashboard/Global Dashboard.sdt");
	globalDashboardView.m_mustHaveFilter = false;
	globalDashboardView.m_tableInGlyphEd = "Dashboard";

	globalDashboardView.m_filterTitles.push_back("Admissions Counselor");
	globalDashboardView.m_filterFieldNames.push_back("reader_name");
	globalDashboardView.m_filterMultiselect.push_back(false);
	globalDashboardView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(globalDashboardView.m_tableInGlyphEd, "reader_name"));

	m_visualizationData.push_back(globalDashboardView);
}

bool LoadingScreenWidget::DoesGlyphEdDirExist() {

	return QDir(GetGlyphEdDir()).exists();
}

QString LoadingScreenWidget::GetGlyphEdDir() {

	if (s_glyphEdDir.isEmpty()) {

		s_glyphEdDir = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/GlyphEd");
	}

	return s_glyphEdDir;
}

void LoadingScreenWidget::OnLoadVisualization() {

	if (m_visualizationData[m_currentView].m_mustHaveFilter && AreAnyFiltersMissingSelection()) {

		QMessageBox::information(this, tr("Did not load visualization"), tr("Visualization can not be loaded until at least one value has been selected from each filter."));
	}
	else {

		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		FilteringParameters filters;

		SynGlyphX::DataTransformMapping mapping;
		mapping.ReadFromFile(m_visualizationData[m_currentView].m_sdtPath.toStdString());

		for (unsigned int i = 0; i < m_filterWidgets.size(); ++i) {

			if (IsFilterWidgetInUse(i)) {

				QSet<QString> filterData;
				for (const auto& selectedItem : m_filterWidgets[i]->selectedItems()) {

					filterData.insert(selectedItem->text());
				}

				if ((!filterData.isEmpty()) && 
					((filterData.size() != m_filterWidgets[i]->model()->rowCount()) ||
					(i == 2) || ((i == 1) && (m_currentView == 5)))) {

					filters.SetDistinctValueFilter(m_visualizationData[m_currentView].m_filterFieldNames[i], filterData);
				}
			}
		}

		if (m_currentView == 1) {

			QSet<QString> years;
			years.insert("2016");
			filters.SetDistinctValueFilter("Year", years);
		}
		else if (m_currentView == 3) {

			QSet<QString> branch;
			branch.insert("Race");
			filters.SetDistinctValueFilter("branch_type", branch);
		}
		else if (m_currentView == 4) {

			QSet<QString> branch;
			branch.insert("Cohort");
			filters.SetDistinctValueFilter("branch_type", branch);
		}
		/*else if (m_currentView == 5) {

			QSet<QString> branch;
			branch.insert("Total");
			filters.SetDistinctValueFilter("branch_type", branch);
		}*/
		else if (m_currentView == 6) {

			QSet<QString> readers;
			if (filters.HasFilters()) {

				readers = filters.GetDistinctValueFilters().at(m_visualizationData[6].m_filterFieldNames[0]);
				
			}
			readers.insert("TOTAL");
			filters.SetDistinctValueFilter(m_visualizationData[6].m_filterFieldNames[0], readers);
		}

		SynGlyphX::InputTable inputTable(mapping.GetDatasources().begin()->first, mapping.GetDatasources().begin()->second->GetTableNames().at(0));

		DataMappingLoadingFilterModel::Table2LoadingFiltersMap table2FiltersMap;

		if (filters.HasFilters()) {

			table2FiltersMap[inputTable] = filters;
		}

		QString dataFilename = QDir::toNativeSeparators(QString::fromStdWString(mapping.GetDatasources().begin()->second->GetDBName()));
		if (QFile::exists(dataFilename)) {

			QFile::remove(dataFilename);
		}

		bool didFilterHaveResult = m_sourceDataCache.ExportFilteredDataToCSV(dataFilename, m_visualizationData[m_currentView].m_tableInGlyphEd, table2FiltersMap[inputTable]);

		SynGlyphX::Application::restoreOverrideCursor();

		if (didFilterHaveResult) {

			m_mainWindow->LoadNewVisualization(m_visualizationData[m_currentView].m_sdtPath);
		}
		else {

			QMessageBox::warning(this, tr("Load Visualization"), tr("The selected combination of filters had no results.  Please try a different combination of filters to load a visualization."));
		}
	}
}

void LoadingScreenWidget::OnNewViewSelected() {

	m_currentView = m_viewListWidget->selectionModel()->selectedIndexes().front().row();

	ClearFilters();

	for (unsigned int i = 0; i < m_filterWidgets.size(); ++i) {

		if (i < m_visualizationData[m_currentView].m_filterTitles.size()) {

			m_filterGroupBoxWidgets[i]->setVisible(true);
			m_filterGroupBoxWidgets[i]->setTitle(m_visualizationData[m_currentView].m_filterTitles[i]);
			SetMultiSelectionEnabled(i, m_visualizationData[m_currentView].m_filterMultiselect[i]);
			m_filterWidgets[i]->addItems(m_visualizationData[m_currentView].m_filterValues[i]);
			m_filterWidgets[i]->setMinimumWidth(m_filterWidgets[i]->sizeHintForColumn(0) + 30);
		}
		else {

			m_filterGroupBoxWidgets[i]->setVisible(false);
		}
	}
}

void LoadingScreenWidget::AddFilterList(QHBoxLayout* filterListLayout) {

	QListWidget* filterListWidget = new QListWidget(this);
	m_filterWidgets.push_back(filterListWidget);

	QPushButton* selectAllButton = new QPushButton(tr("Select All"), this);
	m_selectAllButtonWidgets.push_back(selectAllButton);
	QObject::connect(selectAllButton, &QPushButton::clicked, this, &LoadingScreenWidget::OnSelectAll);

	QVBoxLayout* groupBoxLayout = new QVBoxLayout(this);
	groupBoxLayout->addWidget(selectAllButton);
	groupBoxLayout->addWidget(filterListWidget);

	QGroupBox* groupBox = new QGroupBox(this);
	groupBox->setLayout(groupBoxLayout);
	m_filterGroupBoxWidgets.push_back(groupBox);
	filterListLayout->addWidget(groupBox);
}

void LoadingScreenWidget::OnSelectAll() {

	unsigned int i = 0;
	for (; i < m_selectAllButtonWidgets.size(); ++i) {

		if (sender() == m_selectAllButtonWidgets[i]) {

			break;
		}
	}

	QItemSelection selection(m_filterWidgets[i]->model()->index(0, 0), m_filterWidgets[i]->model()->index(m_filterWidgets[i]->model()->rowCount()-1, 0));
	m_filterWidgets[i]->selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
}

void LoadingScreenWidget::SetMultiSelectionEnabled(unsigned int filterWidgetIndex, bool multiSelectionEnabled) {

	m_filterWidgets[filterWidgetIndex]->setSelectionMode(multiSelectionEnabled ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);
	m_selectAllButtonWidgets[filterWidgetIndex]->setEnabled(multiSelectionEnabled);
}

bool LoadingScreenWidget::AreAnyFiltersMissingSelection() const {

	for (unsigned int i = 0; i < m_filterWidgets.size(); ++i) {

		if (IsFilterWidgetInUse(i) && m_filterWidgets[i]->selectionModel()->selectedIndexes().empty()) {

			return true;
		}
	}

	return false;
}

bool LoadingScreenWidget::IsFilterWidgetInUse(unsigned int index) const {

	return m_filterGroupBoxWidgets[index]->isVisible();
}

void LoadingScreenWidget::ClearFilters() {

	for (auto& filterWidget : m_filterWidgets) {

		filterWidget->selectionModel()->clearSelection();
		filterWidget->clear();
	}
}