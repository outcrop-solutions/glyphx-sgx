#include "HomePageWidget.h"
#include "glyphbuilderapplication.h"
#include <QtCore/QDir>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QMessageBox>
#include "TitleListWidget.h"
#include "LoadingFilterWidget.h"
#include "glyphviewerwindow.h"
#include "ResizeableImageLabel.h"
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QPushButton>
#include "MultiLoadingFilterWidget.h"
#include "application.h"

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
	m_mainLayout->setHorizontalSpacing(0);
	m_mainLayout->setContentsMargins(0, 8, 16, 16);

	QLabel* logoLabel = new QLabel(this);
	logoLabel->setAlignment(Qt::AlignCenter);
	logoLabel->setPixmap(QPixmap(":SGXGUI/Resources/GlyphEd/glyphed_logo_homepage.png"));
	m_mainLayout->addWidget(logoLabel, 0, 0);

	m_optionsButtonGroup = new QButtonGroup(this);
	CreateHomePageOptionsWidget();

	QHBoxLayout* loadButtonLayout = new QHBoxLayout(this);
	loadButtonLayout->addStretch(1);
	m_loadVisualizationButton = new QPushButton(tr("Load Visualization"), this);
	m_loadVisualizationButton->setObjectName("load_viz_button");
	QObject::connect(m_loadVisualizationButton, &QPushButton::clicked, this, &HomePageWidget::OnLoadVisualization);
	loadButtonLayout->addWidget(m_loadVisualizationButton);

	m_mainLayout->addLayout(loadButtonLayout, 0, 1);

	m_homePageWidgetsLayout = new QStackedLayout(this);
	m_homePageWidgetsLayout->setContentsMargins(0, 0, 0, 0);

	CreateDashboardWidget();
	CreateAllViewsWidget();;
	CreateMyViewsWidget();
	CreateHelpWidget();

	m_mainLayout->addLayout(m_homePageWidgetsLayout, 1, 1);

	setLayout(m_mainLayout);

	QObject::connect(&GlyphViewerWindow::GetRecentFileListInstance(), &SynGlyphX::SettingsStoredFileList::FileListChanged, this, &HomePageWidget::OnRecentListUpdated);
	QObject::connect(&GlyphViewerWindow::GetSubsetFileListInstance(), &SynGlyphX::SettingsStoredFileList::FileListChanged, this, &HomePageWidget::OnSubsetListUpdated);

	QObject::connect(m_optionsButtonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &HomePageWidget::OnNewOptionSelected);
	m_optionsButtonGroup->button(0)->setChecked(true);
	OnNewOptionSelected(0);
}

HomePageWidget::~HomePageWidget()
{

}

void HomePageWidget::CreateHomePageOptionsWidget() {

	QStringList options;
	options << tr("   Dashboard") << tr("   All Views") << tr("   My Views") << tr("   Help") << tr("   Exit");

	QVBoxLayout* optionsLayout = new QVBoxLayout(this);
	optionsLayout->setSpacing(20);
	optionsLayout->addSpacing(30);

	for (unsigned int i = 0; i < options.size(); ++i) {

		QPushButton* button = new QPushButton(options[i], this);
		button->setCheckable(true);
		button->setStyleSheet("QPushButton { font-family:'Calibri', Helvetica, Arial, Sans; padding: 5px; border: 0px; font-size: 16pt; text-align: left;} QPushButton:hover{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #c7cbd2, stop: 1 #afb3ba); color: white;} QPushButton:open{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #cdcfd4, stop: 1 #e7e9ea);}");
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

	m_allViewsFilteringWidget = new MultiLoadingFilterWidget(this);
	m_allViewsFilteringWidget->Reset(m_allVisualizationData);

	vizAndFilterFrameLayout->addWidget(m_allViewsFilteringWidget);
	vizAndFilterFrame->setLayout(vizAndFilterFrameLayout);

	QWidget* allViewsWidget = new QWidget(this);
	QVBoxLayout* allViewsLayout = new QVBoxLayout(this);
	allViewsLayout->setContentsMargins(0, 0, 0, 0);
	allViewsLayout->addWidget(vizAndFilterFrame, 1);
	allViewsLayout->addStretch(1);
	allViewsWidget->setLayout(allViewsLayout);

	m_homePageWidgetsLayout->addWidget(allViewsWidget);
}

void HomePageWidget::OnRecentListUpdated() {

	m_recentViewsFilteringWidget->clear();

	for (const auto& recentFile : GlyphViewerWindow::GetRecentFileListInstance().GetFiles()) {

		QString title;
		for (const auto& visualizationData : m_allVisualizationData) {

			if (visualizationData.m_sdtPath == recentFile) {

				title = visualizationData.m_title;
				break;
			}
		}

		if (title.isEmpty()) {

			title = QFileInfo(recentFile).fileName();
		}

		m_recentViewsFilteringWidget->addItem(title);
		m_recentViewsFilteringWidget->item(m_recentViewsFilteringWidget->count() - 1)->setToolTip(recentFile);
	}
}

void HomePageWidget::OnSubsetListUpdated() {

	std::vector<MultiLoadingFilterWidget::VisualizationData> subsetVisualizationData;

	for (const auto& recentFile : GlyphViewerWindow::GetSubsetFileListInstance().GetFiles()) {

		MultiLoadingFilterWidget::VisualizationData visualizationDataToAdd;
		visualizationDataToAdd.m_sdtPath = recentFile;
		visualizationDataToAdd.m_title = QFileInfo(recentFile).fileName();

		subsetVisualizationData.push_back(visualizationDataToAdd);
	}

	m_subsetViewsFilteringWidget->Reset(subsetVisualizationData);
}

void HomePageWidget::CreateMyViewsWidget() {

	QFrame* vizAndFilterFrame = new QFrame(this);
	vizAndFilterFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);
	vizAndFilterFrame->setLineWidth(1);
	vizAndFilterFrame->setMidLineWidth(1);

	QHBoxLayout* vizAndFilterFrameLayout = new QHBoxLayout(vizAndFilterFrame);
	vizAndFilterFrameLayout->setContentsMargins(0, 0, 0, 0);
	vizAndFilterFrameLayout->setSpacing(0);

	m_subsetViewsFilteringWidget = new MultiLoadingFilterWidget(this);

	vizAndFilterFrameLayout->addWidget(m_subsetViewsFilteringWidget);
	vizAndFilterFrame->setLayout(vizAndFilterFrameLayout);

	QWidget* myViewsWidget = new QWidget(this);
	QVBoxLayout* myViewsLayout = new QVBoxLayout(this);
	myViewsLayout->setContentsMargins(0, 0, 0, 0);
	myViewsLayout->addWidget(vizAndFilterFrame, 1);
	myViewsLayout->addStretch(1);
	myViewsWidget->setLayout(myViewsLayout);

	m_homePageWidgetsLayout->addWidget(myViewsWidget);
}

void HomePageWidget::CreateHelpWidget() {

	QWidget* widget = new QWidget(this);
	m_homePageWidgetsLayout->addWidget(widget);
}

void HomePageWidget::CreateDashboardWidget() {

	QFrame* widget = new QFrame(this);
	widget->setObjectName("dashboard_frame");
	widget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	widget->setLineWidth(2);
	widget->setMidLineWidth(2);

	QGridLayout* mainLayout = new QGridLayout(widget);
	mainLayout->setContentsMargins(30, 20, 30, 10);
	mainLayout->setSpacing(15);
	mainLayout->setRowStretch(0, 1);
	mainLayout->setRowStretch(1, 1);
	mainLayout->setRowStretch(2, 2);

	QFrame* welcomeWidget = new QFrame(this);
	welcomeWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
	welcomeWidget->setLineWidth(2);
	welcomeWidget->setMidLineWidth(3);
	welcomeWidget->setStyleSheet("background-color: white;");

	QVBoxLayout* welcomeWidgetLayout = new QVBoxLayout(welcomeWidget);
	welcomeWidgetLayout->setSpacing(0);

	QLabel* welcomeLabel = new QLabel(tr("Welcome to"), welcomeWidget);
	welcomeLabel->setAlignment(Qt::AlignCenter);
	welcomeLabel->setStyleSheet("QLabel{font-size: 48pt; font-weight: bold; background-color: white;}");
	welcomeWidgetLayout->addWidget(welcomeLabel);

	QLabel* logoImage = new QLabel(widget);
	logoImage->setAlignment(Qt::AlignCenter);
	logoImage->setStyleSheet("QLabel{background-color: white;}");
	logoImage->setPixmap(QPixmap(":SGXGUI/Resources/GlyphEd/glyphed_logo_large.png"));

	welcomeWidgetLayout->addWidget(logoImage);
	welcomeWidget->setLayout(welcomeWidgetLayout);

	mainLayout->addWidget(welcomeWidget, 0, 0, 1, 3);

	QFrame* recentViewsWidget = new QFrame(this);
	recentViewsWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
	recentViewsWidget->setLineWidth(2);
	recentViewsWidget->setMidLineWidth(3);
	recentViewsWidget->setStyleSheet("background-color: white;");
	QVBoxLayout* recentViewsLayout = new QVBoxLayout(recentViewsWidget);
	recentViewsLayout->setSpacing(15);

	QLabel* recentViewsLabel = new QLabel(tr("Recent Views"), recentViewsWidget);
	recentViewsLabel->setStyleSheet("font-size: 16pt; font-weight: bold; text-align: left;");
	recentViewsLayout->addWidget(recentViewsLabel);

	m_recentViewsFilteringWidget = new QListWidget(recentViewsWidget);
	m_recentViewsFilteringWidget->setFrameStyle(QFrame::NoFrame);
	m_recentViewsFilteringWidget->setSortingEnabled(false);
	m_recentViewsFilteringWidget->setStyleSheet("font-size: 14px;");
	m_recentViewsFilteringWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	recentViewsLayout->addWidget(m_recentViewsFilteringWidget, 1);
	recentViewsWidget->setLayout(recentViewsLayout);

	mainLayout->addWidget(recentViewsWidget, 1, 0, 2, 1);

	QObject::connect(m_recentViewsFilteringWidget, &QListWidget::itemClicked, this, &HomePageWidget::OnRecentViewClicked);

	SynGlyphX::ResizeableImageLabel* upperRightDashboardImage = new SynGlyphX::ResizeableImageLabel(true, widget);
	upperRightDashboardImage->setAlignment(Qt::AlignCenter);
	upperRightDashboardImage->setFrameStyle(QFrame::Panel | QFrame::Raised);
	upperRightDashboardImage->setLineWidth(2);
	upperRightDashboardImage->setMidLineWidth(3);
	upperRightDashboardImage->setStyleSheet("QLabel{background-color: white;}");
	QString customerLogo = QDir::toNativeSeparators(GetGlyphEdDir() + "/customer.png");
	if (QFileInfo::exists(customerLogo)) {

		upperRightDashboardImage->SetPixmap(QPixmap(customerLogo));
	}
	mainLayout->addWidget(upperRightDashboardImage, 1, 1, 1, 2);

	SynGlyphX::ResizeableImageLabel* lowerRightDashboardImage = new SynGlyphX::ResizeableImageLabel(false, widget);
	lowerRightDashboardImage->setFrameStyle(QFrame::Panel | QFrame::Raised);
	lowerRightDashboardImage->setLineWidth(2);
	lowerRightDashboardImage->setMidLineWidth(3);
	QPixmap lowerRightPixmap(16, 16);
	lowerRightPixmap.fill();
	lowerRightDashboardImage->SetPixmap(lowerRightPixmap);
	mainLayout->addWidget(lowerRightDashboardImage, 2, 1, 1, 2);

	widget->setLayout(mainLayout);
	m_homePageWidgetsLayout->addWidget(widget);
}

void HomePageWidget::SetupVisualizationData() {

	QStringList visualizationNames;
	visualizationNames << "Global Admissions View" << "Admissions Counselor View (2016)" << "High School View" << 
		"Class Composition (Diversity)" << "Class Composition (Cohort)" << "Class Composition (Total)" << "Global Dashboard";

	MultiLoadingFilterWidget::VisualizationData globalAdmissionsView;
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

	m_allVisualizationData.push_back(globalAdmissionsView);

	MultiLoadingFilterWidget::VisualizationData admissionsCounselorView;
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

	m_allVisualizationData.push_back(admissionsCounselorView);

	MultiLoadingFilterWidget::VisualizationData highSchoolView;
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

	m_allVisualizationData.push_back(highSchoolView);

	QStringList statuses = m_sourceDataCache.GetSortedDistinctValuesAsStrings("Composition", "grouping_title", "\"branch_type\"='Status'");
	statuses.push_back("Cohort Aggregate");

	MultiLoadingFilterWidget::VisualizationData classCompositionRaceView;
	classCompositionRaceView.m_title = visualizationNames[3];
	classCompositionRaceView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/Class Composition Diversity.sdt");
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

	m_allVisualizationData.push_back(classCompositionRaceView);

	MultiLoadingFilterWidget::VisualizationData classCompositionCohortView;
	classCompositionCohortView.m_title = visualizationNames[4];
	classCompositionCohortView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/Class Composition Cohort.sdt");
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

	m_allVisualizationData.push_back(classCompositionCohortView);

	MultiLoadingFilterWidget::VisualizationData classCompositionTotalView;
	classCompositionTotalView.m_title = visualizationNames[5];
	classCompositionTotalView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Class Composition/Class Composition Total.sdt");
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

	m_allVisualizationData.push_back(classCompositionTotalView);

	MultiLoadingFilterWidget::VisualizationData globalDashboardView;
	globalDashboardView.m_title = visualizationNames[6];
	globalDashboardView.m_sdtPath = GetGlyphEdDir() + QDir::toNativeSeparators("/Global Dashboard/Global Dashboard.sdt");
	globalDashboardView.m_tableInGlyphEd = "Dashboard";

	globalDashboardView.m_mustHaveFilter.push_back(false);
	globalDashboardView.m_filterTitles.push_back("Admissions Counselor");
	globalDashboardView.m_filterFieldNames.push_back("reader_name");
	globalDashboardView.m_filterMultiselect.push_back(false);
	globalDashboardView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(globalDashboardView.m_tableInGlyphEd, "reader_name"));

	m_allVisualizationData.push_back(globalDashboardView);
}

QString HomePageWidget::GetGlyphEdDir() {

	if (s_glyphEdDir.isEmpty()) {

		s_glyphEdDir = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/GlyphEd");
	}

	return s_glyphEdDir;
}

void HomePageWidget::OnLoadVisualization() {

	unsigned int whichFilteringWidget = m_homePageWidgetsLayout->currentIndex();
	MultiLoadingFilterWidget* currentLoadingFilterWidget = nullptr;
	if (whichFilteringWidget == 1) {

		currentLoadingFilterWidget = m_allViewsFilteringWidget;
	}
	else if (whichFilteringWidget == 2) {

		currentLoadingFilterWidget = m_subsetViewsFilteringWidget;
	}

	if (!currentLoadingFilterWidget->DoCurrentNecessaryFiltersHaveSelection()) {

		QMessageBox::information(this, tr("Did not load visualization"), tr("Visualization can not be loaded until at least one value has been selected from all necessary filters."));
		return;
	}

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	LoadVisualization(currentLoadingFilterWidget->GetCurrentFilename(), currentLoadingFilterWidget->GetCurrentFilterValues());
}

bool HomePageWidget::LoadVisualization(const QString& sdtToLoad, const DistinctValueFilteringParameters& userSelectedFilters) {
	
	FilteringParameters filters;
	filters.DistinctValueFilteringParameters::operator=(userSelectedFilters);

	SynGlyphX::DataTransformMapping mapping;
	mapping.ReadFromFile(sdtToLoad.toStdString());

	int currentDataVisualization = -1;
	for (unsigned int k = 0; k < m_allVisualizationData.size(); ++k) {

		if (m_allVisualizationData[k].m_sdtPath == sdtToLoad) {

			currentDataVisualization = k;
			break;
		}
	}

	if (currentDataVisualization != -1) {

		if (currentDataVisualization == 1) {

			QSet<QString> years;
			years.insert("2016");
			filters.SetDistinctValueFilter("Year", years);
		}
		else if (currentDataVisualization == 3) {

			QSet<QString> branch;
			branch.insert("Race");
			filters.SetDistinctValueFilter("branch_type", branch);
		}
		else if (currentDataVisualization == 4) {

			QSet<QString> branch;
			branch.insert("Cohort");
			filters.SetDistinctValueFilter("branch_type", branch);
		}
		else if (currentDataVisualization == 5) {

			QSet<QString> groupingTitle;
			groupingTitle.insert("Cohort Aggregate");
			filters.SetDistinctValueFilter("grouping_title_lv2", groupingTitle);
		}
		else if (currentDataVisualization == 6) {

			QSet<QString> readers;
			if (filters.HasFilters()) {

				readers = filters.GetDistinctValueFilters().at(m_allVisualizationData[6].m_filterFieldNames[0]);

			}
			readers.insert("TOTAL");
			filters.SetDistinctValueFilter(m_allVisualizationData[6].m_filterFieldNames[0], readers);
		}

		QString dataFilename = QDir::toNativeSeparators(QString::fromStdWString(mapping.GetDatasources().begin()->second->GetDBName()));
		if (QFile::exists(dataFilename)) {

			QFile::remove(dataFilename);
		}

		bool didFilterHaveResult = m_sourceDataCache.ExportFilteredDataToCSV(dataFilename, m_allVisualizationData[currentDataVisualization].m_tableInGlyphEd, filters);

		SynGlyphX::Application::restoreOverrideCursor();

		if (!didFilterHaveResult) {

			QMessageBox::warning(this, tr("Load Visualization"), tr("The selected combination of filters had no results.  Please try a different combination of filters to load a visualization."));
			return false;
		}
	}
	else {

		SynGlyphX::Application::restoreOverrideCursor();
	}

	return m_mainWindow->LoadNewVisualization(sdtToLoad, userSelectedFilters);
}

void HomePageWidget::OnNewOptionSelected(int index) {

	if (index == (m_optionsButtonGroup->buttons().count() - 1)) {

		SynGlyphX::Application::instance()->exit();
	}

	m_homePageWidgetsLayout->setCurrentIndex(index);
	m_loadVisualizationButton->setVisible((index == 1) || (index == 2));
}

void HomePageWidget::OnRecentViewClicked(QListWidgetItem *item) {

	m_mainWindow->LoadRecentFile(item->toolTip());
}
