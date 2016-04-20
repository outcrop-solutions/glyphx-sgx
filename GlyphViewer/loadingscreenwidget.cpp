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
	QObject::connect(m_viewListWidget, &QListWidget::itemSelectionChanged, this, &LoadingScreenWidget::OnNewViewSelected);

	SynGlyphX::GroupBoxSingleWidget* visualizationGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("View(s)"), m_viewListWidget, this);
	vizAndFilterLayout->addWidget(visualizationGroupBox);

	for (unsigned int i = 0; i < 3; ++i) {

		AddFilterList(vizAndFilterLayout);
	}

	vizAndFilterLayout->addStretch(1);

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
		"Class Composition (Diversity)" << "Class Composition (Cohort)" << "Class Composition (All)" << "Global Dashboard";

	VisualizationData globalAdmissionsView;
	globalAdmissionsView.m_title = visualizationNames[0];
	globalAdmissionsView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Global Admissions/View 1 Datamap.sdt");

	globalAdmissionsView.m_filterTitles.push_back("Admissions Counselor");
	globalAdmissionsView.m_filterFieldNames.push_back("StaffAssigned");
	globalAdmissionsView.m_filterMultiselect.push_back(true);
	globalAdmissionsView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("GlobalAdmissions", "StaffAssigned"));
	globalAdmissionsView.m_filterTitles.push_back("Year(s)");
	globalAdmissionsView.m_filterFieldNames.push_back("Year");
	globalAdmissionsView.m_filterMultiselect.push_back(true);
	globalAdmissionsView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("GlobalAdmissions", "Year"));

	m_visualizationData.push_back(globalAdmissionsView);

	VisualizationData admissionsCounselorView;
	admissionsCounselorView.m_title = visualizationNames[1];
	admissionsCounselorView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Admissions Officer/View 2 Datamap.sdt");

	admissionsCounselorView.m_filterTitles.push_back("Admissions Counselor");
	admissionsCounselorView.m_filterFieldNames.push_back("StaffAssigned");
	admissionsCounselorView.m_filterMultiselect.push_back(false);
	admissionsCounselorView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("GlobalAdmissions", "StaffAssigned"));
	admissionsCounselorView.m_filterTitles.push_back("Decision Status");
	admissionsCounselorView.m_filterFieldNames.push_back("Last_Decision_Cluster");
	admissionsCounselorView.m_filterMultiselect.push_back(true);
	admissionsCounselorView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("GlobalAdmissions", "Last_Decision_Cluster"));
	/*admissionsCounselorView.m_filterTitles.push_back("Year(s)");
	admissionsCounselorView.m_filterMultiselect.push_back(true);
	admissionsCounselorView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("GlobalAdmissions", "Year"));*/

	m_visualizationData.push_back(admissionsCounselorView);

	VisualizationData highSchoolView;
	highSchoolView.m_title = visualizationNames[2];
	highSchoolView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/High School/View 3 Datamap.sdt");

	highSchoolView.m_filterTitles.push_back("High School");
	highSchoolView.m_filterFieldNames.push_back("HSNameUnique");
	highSchoolView.m_filterMultiselect.push_back(false);
	highSchoolView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("GlobalAdmissions", "HSNameUnique", "\"HSSize\" >= 10"));
	highSchoolView.m_filterTitles.push_back("Year(s)");
	highSchoolView.m_filterFieldNames.push_back("Year");
	highSchoolView.m_filterMultiselect.push_back(true);
	highSchoolView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("GlobalAdmissions", "Year"));

	m_visualizationData.push_back(highSchoolView);

	VisualizationData classCompositionRaceView;
	classCompositionRaceView.m_title = visualizationNames[3];
	classCompositionRaceView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/View 4 V3 in 3D All Readers.sdt");

	classCompositionRaceView.m_filterTitles.push_back("Race");
	classCompositionRaceView.m_filterFieldNames.push_back("grouping_title");
	classCompositionRaceView.m_filterMultiselect.push_back(true);
	classCompositionRaceView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "grouping_title", "\"branch_type\"='Race'"));
	classCompositionRaceView.m_filterTitles.push_back("Year(s)");
	classCompositionRaceView.m_filterFieldNames.push_back("Year");
	classCompositionRaceView.m_filterMultiselect.push_back(true);
	classCompositionRaceView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "Year"));
	classCompositionRaceView.m_filterTitles.push_back("Decision Status");
	classCompositionRaceView.m_filterFieldNames.push_back("Last_Decision_Cluster");
	classCompositionRaceView.m_filterMultiselect.push_back(true);
	classCompositionRaceView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "Last_Decision_Cluster"));

	m_visualizationData.push_back(classCompositionRaceView);

	VisualizationData classCompositionCohortView;
	classCompositionCohortView.m_title = visualizationNames[4];
	classCompositionCohortView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/View 4 V3 in 3D All Readers.sdt");

	classCompositionCohortView.m_filterTitles.push_back("Cohort");
	classCompositionCohortView.m_filterFieldNames.push_back("grouping_title");
	classCompositionCohortView.m_filterMultiselect.push_back(true);
	classCompositionCohortView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "grouping_title", "\"branch_type\"='Cohort'"));
	classCompositionCohortView.m_filterTitles.push_back("Year(s)");
	classCompositionCohortView.m_filterFieldNames.push_back("Year");
	classCompositionCohortView.m_filterMultiselect.push_back(true);
	classCompositionCohortView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "Year"));
	classCompositionCohortView.m_filterTitles.push_back("Decision Status");
	classCompositionCohortView.m_filterFieldNames.push_back("Last_Decision_Cluster");
	classCompositionCohortView.m_filterMultiselect.push_back(true);
	classCompositionCohortView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "Last_Decision_Cluster"));

	m_visualizationData.push_back(classCompositionCohortView);

	VisualizationData classCompositionTotalView;
	classCompositionTotalView.m_title = visualizationNames[5];
	classCompositionTotalView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/View 4 V3 in 3D All Readers.sdt");

	classCompositionTotalView.m_filterTitles.push_back("Total");
	classCompositionTotalView.m_filterFieldNames.push_back("grouping_title");
	classCompositionTotalView.m_filterMultiselect.push_back(true);
	classCompositionTotalView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "grouping_title", "'branch_type'='Total'"));
	classCompositionTotalView.m_filterTitles.push_back("Year(s)");
	classCompositionTotalView.m_filterFieldNames.push_back("Year");
	classCompositionTotalView.m_filterMultiselect.push_back(true);
	classCompositionTotalView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "Year"));
	classCompositionTotalView.m_filterTitles.push_back("Decision Status");
	classCompositionTotalView.m_filterFieldNames.push_back("Last_Decision_Cluster");
	classCompositionTotalView.m_filterMultiselect.push_back(true);
	classCompositionTotalView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "Last_Decision_Cluster"));

	m_visualizationData.push_back(classCompositionTotalView);

	VisualizationData globalDashboardView;
	globalDashboardView.m_title = visualizationNames[6];
	globalDashboardView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/GLobal Dashboard/View 4 V3 MASTER READER.sdt");
	globalDashboardView.m_mustHaveFilter = false;

	globalDashboardView.m_filterTitles.push_back("Admissions Counselor");
	classCompositionTotalView.m_filterFieldNames.push_back("reader_name");
	globalDashboardView.m_filterMultiselect.push_back(false);
	globalDashboardView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "reader_name"));

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

		FilteringParameters filters;

		SynGlyphX::DataTransformMapping mapping;
		mapping.ReadFromFile(m_visualizationData[m_currentView].m_sdtPath.toStdString());

		for (unsigned int i = 0; i < m_filterWidgets.size(); ++i) {

			if (IsFilterWidgetInUse(i)) {

				QSet<QString> filterData;
				for (const auto& selectedItem : m_filterWidgets[i]->selectedItems()) {

					filterData.insert(selectedItem->text());
				}

				if ((!filterData.isEmpty()) && (filterData.size() != m_filterWidgets[i]->model()->rowCount())) {

					filters.SetDistinctValueFilter(m_visualizationData[m_currentView].m_filterFieldNames[i], filterData);
				}
			}
		}

		if (m_currentView == 1) {

			QSet<QString> years;
			years.insert("2016");
			filters.SetDistinctValueFilter("Year", years);
		}

		SynGlyphX::InputTable inputTable(mapping.GetDatasources().begin()->first, mapping.GetDatasources().begin()->second->GetTableNames().at(0));

		DataMappingLoadingFilterModel::Table2LoadingFiltersMap table2FiltersMap;

		if (filters.HasFilters()) {

			table2FiltersMap[inputTable] = filters;
		}

		m_mainWindow->LoadNewVisualization(m_visualizationData[m_currentView].m_sdtPath, table2FiltersMap);
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