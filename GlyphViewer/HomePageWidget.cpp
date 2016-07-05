#include "HomePageWidget.h"
#include "glyphbuilderapplication.h"
#include <QtCore/QDir>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QMessageBox>
#include "TitleListWidget.h"
#include "LoadingFilterWidget.h"
#include "glyphviewerwindow.h"

QString HomePageWidget::s_glyphEdDir;

HomePageWidget::HomePageWidget(GlyphViewerWindow* mainWindow, QWidget *parent)
	: QFrame(parent),
	m_mainWindow(mainWindow)
{
	m_sourceDataCache.Setup(GetGlyphEdDir() + QDir::toNativeSeparators("/glyphed.db"));

	SetupVisualizationData();

	setFrameStyle(QFrame::Box | QFrame::Sunken);
	
	m_mainLayout = new QGridLayout(this);
	m_mainLayout->setRowStretch(1, 1);
	m_mainLayout->setColumnStretch(1, 1);

	QLabel* logoLabel = new QLabel(this);
	logoLabel->setPixmap(QPixmap(SynGlyphX::GlyphBuilderApplication::GetLogoLocation(SynGlyphX::GlyphBuilderApplication::NoBorder)));
	m_mainLayout->addWidget(logoLabel, 0, 0);

	m_optionsButtonGroup = new QButtonGroup(this);
	CreateHomePageOptionsWidget();

	QHBoxLayout* loadButtonLayout = new QHBoxLayout(this);
	loadButtonLayout->addStretch(1);
	m_loadVisualizationButton = new QPushButton(tr("Load Visualization"), this);
	QObject::connect(m_loadVisualizationButton, &QPushButton::clicked, this, &HomePageWidget::OnLoadVisualization);
	loadButtonLayout->addWidget(m_loadVisualizationButton);

	m_mainLayout->addLayout(loadButtonLayout, 0, 1);

	m_homePageWidgetsLayout = new QStackedLayout(this);
	m_homePageWidgetsLayout->setContentsMargins(0, 0, 0, 0);

	CreateDashboardWidget();
	CreateAllViewsWidget();
	CreateRecentViewsWidget();
	CreateMyViewsWidget();
	CreateHelpWidget();

	m_mainLayout->addLayout(m_homePageWidgetsLayout, 1, 1);

	setLayout(m_mainLayout);

	QObject::connect(m_viewListWidget, &SynGlyphX::TitleListWidget::CurrentRowChanged, m_loadingFilterStackedWidget, &QStackedWidget::setCurrentIndex);
	m_viewListWidget->SelectItem(0);

	QObject::connect(m_optionsButtonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &HomePageWidget::OnNewOptionSelected);
	m_optionsButtonGroup->button(0)->setChecked(true);
	OnNewOptionSelected(0);
}

HomePageWidget::~HomePageWidget()
{

}

void HomePageWidget::CreateHomePageOptionsWidget() {

	QStringList options;
	options << tr("Dashboard") << tr("All Views") << tr("Recent Views") << tr("My Views") << tr("Help");

	QVBoxLayout* optionsLayout = new QVBoxLayout(this);
	optionsLayout->setSpacing(20);
	optionsLayout->addSpacing(30);

	for (unsigned int i = 0; i < options.size(); ++i) {

		QPushButton* button = new QPushButton(options[i], this);
		button->setCheckable(true);
		button->setStyleSheet("QPushButton { border: 0px;  font-size: 14pt; font-weight: bold; } QPushButton:checked{ text-decoration: underline; }");
		optionsLayout->addWidget(button);
		m_optionsButtonGroup->addButton(button, i);
	}

	optionsLayout->addStretch(1);

	m_mainLayout->addLayout(optionsLayout, 1, 0);
}

void HomePageWidget::CreateAllViewsWidget() {

	QFrame* vizAndFilterFrame = new QFrame(this);
	vizAndFilterFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);
	vizAndFilterFrame->setLineWidth(1);
	vizAndFilterFrame->setMidLineWidth(1);

	QHBoxLayout* vizAndFilterFrameLayout = new QHBoxLayout(vizAndFilterFrame);
	vizAndFilterFrameLayout->setContentsMargins(0, 0, 0, 0);
	vizAndFilterFrameLayout->setSpacing(0);

	QSplitter* vizAndFilterSplitter = new QSplitter(Qt::Horizontal, this);
	vizAndFilterSplitter->setChildrenCollapsible(false);
	vizAndFilterSplitter->setHandleWidth(2);
	vizAndFilterSplitter->setStyleSheet("QSplitter::handle:horizontal { background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 1, stop : 0 #eee, stop:1 #ccc);"
		"border: 1px solid #777; width: 0px; margin - top: 0px; margin - bottom: 0px; border - radius: 2px; }");

	m_viewListWidget = new SynGlyphX::TitleListWidget(this);
	m_viewListWidget->SetAllowMultiselect(false);
	m_viewListWidget->ShowSelectAllButton(false);
	m_viewListWidget->SetTitle(tr("View(s)"));
	m_viewListWidget->layout()->setContentsMargins(0, 0, 0, 0);

	QStringList visualizations;
	for (const auto& visualizationData : m_visualizationData) {

		visualizations << visualizationData.m_title;
	}
	m_viewListWidget->SetItems(visualizations);
	vizAndFilterSplitter->addWidget(m_viewListWidget);

	m_loadingFilterStackedWidget = new QStackedWidget(this);
	for (unsigned int i = 0; i < m_visualizationData.size(); ++i) {

		LoadingFilterWidget* loadingFilterWidget = new LoadingFilterWidget(this);
		for (unsigned int j = 0; j < m_visualizationData[i].m_filterTitles.size(); ++j) {

			loadingFilterWidget->AddFilter(m_visualizationData[i].m_filterTitles[j],
				m_visualizationData[i].m_filterMultiselect[j],
				m_visualizationData[i].m_filterValues[j]);
		}

		m_loadingFilterStackedWidget->addWidget(loadingFilterWidget);
		m_loadingFilterWidgets.append(loadingFilterWidget);
	}
	vizAndFilterSplitter->addWidget(m_loadingFilterStackedWidget);

	vizAndFilterFrameLayout->addWidget(vizAndFilterSplitter);
	vizAndFilterFrame->setLayout(vizAndFilterFrameLayout);

	m_homePageWidgetsLayout->addWidget(vizAndFilterFrame);
}

void HomePageWidget::CreateRecentViewsWidget() {

	QWidget* widget = new QWidget(this);
	m_homePageWidgetsLayout->addWidget(widget);
}

void HomePageWidget::CreateMyViewsWidget() {

	QWidget* widget = new QWidget(this);
	m_homePageWidgetsLayout->addWidget(widget);
}

void HomePageWidget::CreateHelpWidget() {

	QWidget* widget = new QWidget(this);
	m_homePageWidgetsLayout->addWidget(widget);
}

void HomePageWidget::CreateDashboardWidget() {

	QWidget* widget = new QWidget(this);
	m_homePageWidgetsLayout->addWidget(widget);
}

void HomePageWidget::SetupVisualizationData() {

	QStringList visualizationNames;
	visualizationNames << "Global Admissions View" << "Admissions Counselor View (2016)" << "High School View" << 
		"Class Composition (Diversity)" << "Class Composition (Cohort)" << "Class Composition (Total)" << "Global Dashboard";

	VisualizationData globalAdmissionsView;
	globalAdmissionsView.m_title = visualizationNames[0];
	globalAdmissionsView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Global Admissions/Global Admissions.sdt");
	globalAdmissionsView.m_tableInGlyphEd = "GlobalAdmissions";

	globalAdmissionsView.m_filterTitles.push_back("Admissions Counselor");
	globalAdmissionsView.m_mustHaveFilter.push_back(true);
	globalAdmissionsView.m_filterFieldNames.push_back("StaffAssigned");
	globalAdmissionsView.m_filterMultiselect.push_back(true);
	globalAdmissionsView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(globalAdmissionsView.m_tableInGlyphEd, "StaffAssigned"));
	globalAdmissionsView.m_filterTitles.push_back("Year(s)");
	globalAdmissionsView.m_mustHaveFilter.push_back(true);
	globalAdmissionsView.m_filterFieldNames.push_back("Year");
	globalAdmissionsView.m_filterMultiselect.push_back(true);
	globalAdmissionsView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(globalAdmissionsView.m_tableInGlyphEd, "Year"));

	m_visualizationData.push_back(globalAdmissionsView);

	VisualizationData admissionsCounselorView;
	admissionsCounselorView.m_title = visualizationNames[1];
	admissionsCounselorView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Admissions Officer/Admissions Counselor.sdt");
	admissionsCounselorView.m_tableInGlyphEd = "GlobalAdmissions";

	admissionsCounselorView.m_filterTitles.push_back("Admissions Counselor");
	admissionsCounselorView.m_mustHaveFilter.push_back(true);
	admissionsCounselorView.m_filterFieldNames.push_back("StaffAssigned");
	admissionsCounselorView.m_filterMultiselect.push_back(false);
	admissionsCounselorView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(admissionsCounselorView.m_tableInGlyphEd, "StaffAssigned", "\"Year\"=2016"));
	admissionsCounselorView.m_filterTitles.push_back("Decision Status");
	admissionsCounselorView.m_mustHaveFilter.push_back(true);
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
	highSchoolView.m_mustHaveFilter.push_back(true);
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
	highSchoolView.m_mustHaveFilter.push_back(true);
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
	classCompositionRaceView.m_mustHaveFilter.push_back(true);
	classCompositionRaceView.m_filterFieldNames.push_back("grouping_title");
	classCompositionRaceView.m_filterMultiselect.push_back(true);
	classCompositionRaceView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionRaceView.m_tableInGlyphEd, "grouping_title", "\"branch_type\"='Race'"));
	classCompositionRaceView.m_filterTitles.push_back("Year(s)");
	classCompositionRaceView.m_mustHaveFilter.push_back(true);
	classCompositionRaceView.m_filterFieldNames.push_back("Year");
	classCompositionRaceView.m_filterMultiselect.push_back(true);
	classCompositionRaceView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionRaceView.m_tableInGlyphEd, "Year"));
	classCompositionRaceView.m_filterTitles.push_back("Decision Status");
	classCompositionRaceView.m_mustHaveFilter.push_back(true);
	classCompositionRaceView.m_filterFieldNames.push_back("grouping_title_lv2");
	classCompositionRaceView.m_filterMultiselect.push_back(true);
	classCompositionRaceView.m_filterValues.push_back(statuses);

	m_visualizationData.push_back(classCompositionRaceView);

	VisualizationData classCompositionCohortView;
	classCompositionCohortView.m_title = visualizationNames[4];
	classCompositionCohortView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/Class Composition.sdt");
	classCompositionCohortView.m_tableInGlyphEd = "Composition";

	classCompositionCohortView.m_filterTitles.push_back("Cohort");
	classCompositionCohortView.m_mustHaveFilter.push_back(true);
	classCompositionCohortView.m_filterFieldNames.push_back("grouping_title");
	classCompositionCohortView.m_filterMultiselect.push_back(true);
	classCompositionCohortView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionCohortView.m_tableInGlyphEd, "grouping_title", "\"branch_type\"='Cohort'"));
	classCompositionCohortView.m_filterTitles.push_back("Year(s)");
	classCompositionCohortView.m_mustHaveFilter.push_back(true);
	classCompositionCohortView.m_filterFieldNames.push_back("Year");
	classCompositionCohortView.m_filterMultiselect.push_back(true);
	classCompositionCohortView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionCohortView.m_tableInGlyphEd, "Year"));
	classCompositionCohortView.m_filterTitles.push_back("Decision Status");
	classCompositionCohortView.m_mustHaveFilter.push_back(true);
	classCompositionCohortView.m_filterFieldNames.push_back("grouping_title_lv2");
	classCompositionCohortView.m_filterMultiselect.push_back(true);
	classCompositionCohortView.m_filterValues.push_back(statuses);

	m_visualizationData.push_back(classCompositionCohortView);

	VisualizationData classCompositionTotalView;
	classCompositionTotalView.m_title = visualizationNames[5];
	classCompositionTotalView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/Class Composition.sdt");
	classCompositionTotalView.m_tableInGlyphEd = "Composition";

	classCompositionTotalView.m_filterTitles.push_back("Year(s)");
	classCompositionTotalView.m_mustHaveFilter.push_back(true);
	classCompositionTotalView.m_filterFieldNames.push_back("Year");
	classCompositionTotalView.m_filterMultiselect.push_back(true);
	classCompositionTotalView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(classCompositionTotalView.m_tableInGlyphEd, "Year"));
	classCompositionTotalView.m_filterTitles.push_back("Decision Status");
	classCompositionTotalView.m_mustHaveFilter.push_back(true);
	classCompositionTotalView.m_filterFieldNames.push_back("grouping_title");
	classCompositionTotalView.m_filterMultiselect.push_back(true);
	classCompositionTotalView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "grouping_title", "\"branch_type\"='Status'"));

	m_visualizationData.push_back(classCompositionTotalView);

	VisualizationData globalDashboardView;
	globalDashboardView.m_title = visualizationNames[6];
	globalDashboardView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Global Dashboard/Global Dashboard.sdt");
	globalDashboardView.m_tableInGlyphEd = "Dashboard";

	globalDashboardView.m_mustHaveFilter.push_back(false);
	globalDashboardView.m_filterTitles.push_back("Admissions Counselor");
	globalDashboardView.m_filterFieldNames.push_back("reader_name");
	globalDashboardView.m_filterMultiselect.push_back(false);
	globalDashboardView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(globalDashboardView.m_tableInGlyphEd, "reader_name"));

	m_visualizationData.push_back(globalDashboardView);
}

QString HomePageWidget::GetGlyphEdDir() {

	if (s_glyphEdDir.isEmpty()) {

		s_glyphEdDir = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/GlyphEd");
	}

	return s_glyphEdDir;
}

void HomePageWidget::OnLoadVisualization() {

	unsigned int currentView = m_loadingFilterStackedWidget->currentIndex();
	for (int j = 0; j < m_visualizationData[currentView].m_mustHaveFilter.size(); ++j) {

		if (m_visualizationData[currentView].m_mustHaveFilter[j] && (!m_loadingFilterWidgets[currentView]->AreAnyValuesSelected(j))) {

			QMessageBox::information(this, tr("Did not load visualization"), tr("Visualization can not be loaded until at least one value has been selected from ") + m_visualizationData[currentView].m_filterTitles[j] + ".");
			return;
		}
	}

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	FilteringParameters filters;

	SynGlyphX::DataTransformMapping mapping;
	mapping.ReadFromFile(m_visualizationData[currentView].m_sdtPath.toStdString());

	for (unsigned int i = 0; i < m_visualizationData[currentView].m_filterFieldNames.size(); ++i) {

		QSet<QString> filterData = m_loadingFilterWidgets[currentView]->GetFilterData(i);

		if ((!filterData.isEmpty()) && (!m_loadingFilterWidgets[currentView]->AreAllValuesSelected(i))) {

			filters.SetDistinctValueFilter(m_visualizationData[currentView].m_filterFieldNames[i], filterData);
		}
	}

	if (currentView == 1) {

		QSet<QString> years;
		years.insert("2016");
		filters.SetDistinctValueFilter("Year", years);
	}
	else if (currentView == 3) {

		QSet<QString> branch;
		branch.insert("Race");
		filters.SetDistinctValueFilter("branch_type", branch);
	}
	else if (currentView == 4) {

		QSet<QString> branch;
		branch.insert("Cohort");
		filters.SetDistinctValueFilter("branch_type", branch);
	}
	else if (currentView == 5) {

		QSet<QString> groupingTitle;
		groupingTitle.insert("Cohort Aggregate");
		filters.SetDistinctValueFilter("grouping_title_lv2", groupingTitle);
	}
	else if (currentView == 6) {

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

	bool didFilterHaveResult = m_sourceDataCache.ExportFilteredDataToCSV(dataFilename, m_visualizationData[currentView].m_tableInGlyphEd, table2FiltersMap[inputTable]);

	SynGlyphX::Application::restoreOverrideCursor();

	if (didFilterHaveResult) {

		m_mainWindow->LoadNewVisualization(m_visualizationData[currentView].m_sdtPath);
	}
	else {

		QMessageBox::warning(this, tr("Load Visualization"), tr("The selected combination of filters had no results.  Please try a different combination of filters to load a visualization."));
	}
}

void HomePageWidget::OnNewOptionSelected(int index) {

	m_homePageWidgetsLayout->setCurrentIndex(index);
	m_loadVisualizationButton->setVisible(index == 1);
}