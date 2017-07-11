#include "HomePageWidget.h"
#include "glyphbuilderapplication.h"
#include <QtCore/QDir>
#include <QtCore/QSettings>
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
#include <QtWidgets/QListWidget>
#include "SharedVisualizationsWidget.h"
#include "glyphbuilderapplication.h"
#include "helpdialog.h"
#include <boost/uuid/uuid_io.hpp>
#include <QtWidgets/QFormLayout>
#include <QtWebEngineWidgets/QWebEngineView>
#include "syncprogressdialog.h"
#include <QtWidgets/QDesktopWidget>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "AnnouncementDialog.h"
#include "S3FileManager.h"
#include "GroupSelectionDialog.h"
#include "version.h"

HomePageWidget::HomePageWidget(GlyphViewerWindow* mainWindow, DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent)
	: QFrame(parent),
	m_mainWindow(mainWindow),
	loggedOn(false),
    m_dataEngineConnection(dataEngineConnection)
{
	setFrameStyle(QFrame::Box | QFrame::Sunken);
	
	m_mainLayout = new QGridLayout(this);
	m_mainLayout->setRowStretch(1, 1);
	m_mainLayout->setColumnStretch(1, 1);
	m_mainLayout->setHorizontalSpacing(0);
	m_mainLayout->setContentsMargins(0, 8, 16, 16);
	/*
	QSettings settings;
	QStringList keys = settings.allKeys();
	for (auto key : keys){
		settings.remove(key);
	}
	settings.clear();
	*/
	QLabel* logoLabel = new QLabel(this);
	logoLabel->setAlignment(Qt::AlignCenter);
	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

		logoLabel->setPixmap(QPixmap(":SGXGUI/Resources/GlyphEd/glyphed_logo_homepage.png").scaled(SynGlyphX::Application::DynamicQSize(135, 25)));
	}
	else {

		logoLabel->setPixmap(QPixmap(":SGXGUI/Resources/synglyphx_logo_white_border.png").scaled(SynGlyphX::Application::DynamicQSize(178,40)));
	}
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
	CreateAllViewsWidget();
	CreateMyViewsWidget();
	CreateHelpWidget();

	m_mainLayout->addLayout(m_homePageWidgetsLayout, 1, 1);

	setLayout(m_mainLayout);

	QObject::connect(&GlyphViewerWindow::GetRecentFileListInstance(), &SynGlyphX::SettingsStoredFileList::FileListChanged, this, &HomePageWidget::OnRecentListUpdated);
	QObject::connect(&GlyphViewerWindow::GetSubsetFileListInstance(), &SynGlyphX::SettingsStoredFileList::FileListChanged, this, &HomePageWidget::OnSubsetListUpdated);

	QObject::connect(m_optionsButtonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &HomePageWidget::OnNewOptionSelected);
	m_optionsButtonGroup->button(0)->setChecked(true);
	OnNewOptionSelected(0);

	s3Manager = new DataEngine::S3FileManager();
	CheckForNewRelease();

	if (loggedOn){
		//QTimer::singleShot(0, this, SLOT(Login()));
		Login();
	}
	else if(releaseDialog){
		releaseDialog->show();
	}
}

HomePageWidget::~HomePageWidget()
{

}

void HomePageWidget::LoadProject(const QString& project) {
	//m_allViewsFilteringWidget->Reset(m_dataEngineConnection, project);
	
}

void HomePageWidget::CreateHomePageOptionsWidget() {

	QStringList options;
	options << tr("   Home") << tr("   All Views") << tr("   My Views") << tr("   User Guide") << tr("   Exit");

	QVBoxLayout* optionsLayout = new QVBoxLayout(this);
	optionsLayout->setSpacing(20);
	optionsLayout->addSpacing(30);

	for (int i = 0; i < options.size(); ++i) {

		QPushButton* button = new QPushButton(options[i], this);
		button->setCheckable(true);
		button->setStyleSheet("QPushButton { font-family:'Calibri', Helvetica, Arial, Sans; padding: 5px; border: 0px; font-size: 16pt; text-align: left;color:black;} QPushButton:hover{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #c7cbd2, stop: 1 #afb3ba); color: white;} QPushButton:open{ background-color: #4acaf8;color: white;}");//qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #cdcfd4, stop: 1 #e7e9ea)
		optionsLayout->addWidget(button);
		m_optionsButtonGroup->addButton(button, i);
	}

	optionsLayout->addStretch(1);

	m_mainLayout->addLayout(optionsLayout, 1, 0);
}

void HomePageWidget::CreateAllViewsWidget() {

	QFrame* vizAndFilterFrame = new QFrame(this);
	vizAndFilterFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
	vizAndFilterFrame->setLineWidth(1);
	vizAndFilterFrame->setMidLineWidth(1);

	QHBoxLayout* vizAndFilterFrameLayout = new QHBoxLayout(vizAndFilterFrame);
	vizAndFilterFrameLayout->setContentsMargins(0, 0, 0, 0);
	vizAndFilterFrameLayout->setSpacing(0);

	m_allViewsFilteringWidget = new SharedVisualizationsWidget(this);
	//m_allViewsFilteringWidget->Reset(m_dataEngineConnection);

	vizAndFilterFrameLayout->addWidget(m_allViewsFilteringWidget);
	vizAndFilterFrame->setLayout(vizAndFilterFrameLayout);

	QWidget* allViewsWidget = new QWidget(this);
	QVBoxLayout* allViewsLayout = new QVBoxLayout(this);
	allViewsLayout->setContentsMargins(0, 0, 0, 0);
	allViewsLayout->addWidget(vizAndFilterFrame, 5);
	allViewsLayout->addStretch(1);
	allViewsWidget->setLayout(allViewsLayout);

	m_homePageWidgetsLayout->addWidget(allViewsWidget);
}

void HomePageWidget::OnRecentListUpdated() {

	m_recentViewsFilteringWidget->clear();

	for (const auto& recentFile : GlyphViewerWindow::GetRecentFileListInstance().GetFiles()) {

		QString title;
		const QMap<QString, QString>& visualizationInfo = m_allViewsFilteringWidget->GetSharedVisualizationsInfo();
		if (visualizationInfo.contains(recentFile)) {

			title = visualizationInfo[recentFile];
		}
		else {

			title = QFileInfo(recentFile).completeBaseName();
		}

		m_recentViewsFilteringWidget->addItem(title);
		m_recentViewsFilteringWidget->item(m_recentViewsFilteringWidget->count() - 1)->setToolTip(recentFile);
	}
}

void HomePageWidget::OnSubsetListUpdated() {

	QStringList titles;
	QStringList filenames;

	for (const auto& recentFile : GlyphViewerWindow::GetSubsetFileListInstance().GetFiles()) {

		filenames.push_back(recentFile);
		titles.push_back(QFileInfo(recentFile).completeBaseName());
	}

	m_subsetViewsFilteringWidget->SetItems(titles, filenames);
}

void HomePageWidget::CreateMyViewsWidget() {

	QFrame* vizAndFilterFrame = new QFrame(this);
	vizAndFilterFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
	vizAndFilterFrame->setLineWidth(1);
	vizAndFilterFrame->setMidLineWidth(1);

	QHBoxLayout* vizAndFilterFrameLayout = new QHBoxLayout(vizAndFilterFrame);
	vizAndFilterFrameLayout->setContentsMargins(0, 0, 0, 0);
	vizAndFilterFrameLayout->setSpacing(0);

	m_subsetViewsFilteringWidget = new SynGlyphX::TitleListWidget(this);
	m_subsetViewsFilteringWidget->SetAllowMultiselect(false);
	m_subsetViewsFilteringWidget->ShowSelectAllButton(false);
	m_subsetViewsFilteringWidget->SetTitle(tr("View(s)"));
	m_subsetViewsFilteringWidget->layout()->setContentsMargins(0, 0, 0, 0);

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

	if (m_homePageWidgetsLayout->count() < m_optionsButtonGroup->buttons().size()-1){

		QFrame* widget = new QFrame(this);
		widget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		widget->setLineWidth(2);
		widget->setMidLineWidth(2);
		widget->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #cdcfd4, stop: 1 #e7e9ea)");

		QHBoxLayout* mainLayout = new QHBoxLayout(widget);
		mainLayout->setContentsMargins(0, 0, 0, 0);

		QFrame* helpWidget = new QFrame(this);
		helpWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
		helpWidget->setLineWidth(2);
		helpWidget->setMidLineWidth(3);
		helpWidget->setStyleSheet("background-color: white;");

		QHBoxLayout* helpLayout = new QHBoxLayout(this);
		helpLayout->setContentsMargins(0, 0, 0, 0);
		helpLayout->addStretch();
		QString path("https://s3.amazonaws.com/glyphit/userguide/index.htm");
		if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {
			path = "https://s3.amazonaws.com/glyphit/userguide/index.htm";
		}
		w_guide = widget->frameWidth() / 2;
		h_guide = widget->frameRect().height();
		helpLayout->addWidget(SynGlyphX::createHelpDialog(path, w_guide, h_guide, helpWidget));
		helpLayout->addStretch();
		helpWidget->setLayout(helpLayout);
		mainLayout->addWidget(helpWidget);

		widget->setLayout(mainLayout);
		m_homePageWidgetsLayout->addWidget(widget);
	}
	else{
		
		int index = 0;
		for (int i = 0; i < m_optionsButtonGroup->buttons().size(); i++){
			if (m_optionsButtonGroup->button(i)->text() == "   User Guide"){
				index = i;
				break;
			}
		}

		QFrame* widget = static_cast<QFrame*>(m_homePageWidgetsLayout->widget(index));
		QHBoxLayout* mainLayout = static_cast<QHBoxLayout*>(widget->layout());

		if (mainLayout->count() == 2){

			QFrame* lhw = static_cast<QFrame*>(mainLayout->itemAt(1)->widget());
			if (lhw->layout() != NULL)
			{
				QLayoutItem* item;
				while ((item = lhw->layout()->takeAt(0)) != NULL)
				{
					delete item->widget();
					delete item;
				}
				delete lhw->layout();
			}
			QLayoutItem* item = mainLayout->takeAt(1);
			delete item->widget();
			delete item;
		}
		else{

			m_mainWindow->SetCurrentUserDirectory(m_dataEngineConnection->UserAccessControls()->GlyphEdPath());
			QDir dir(m_dataEngineConnection->UserAccessControls()->GlyphEdPath() + "/guide");
			if (dir.exists()){

				QFrame* l_helpWidget = new QFrame(this);
				l_helpWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
				l_helpWidget->setLineWidth(2);
				l_helpWidget->setMidLineWidth(3);
				l_helpWidget->setStyleSheet("background-color: white;");

				QHBoxLayout* l_helpLayout = new QHBoxLayout(this);
				l_helpLayout->setContentsMargins(0, 0, 0, 0);
				l_helpLayout->addStretch();
				QString l_path(dir.path() + "/index.htm");
				l_helpLayout->addWidget(SynGlyphX::createHelpDialog(l_path, w_guide, h_guide, l_helpWidget));
				l_helpLayout->addStretch();
				l_helpWidget->setLayout(l_helpLayout);
				mainLayout->addWidget(l_helpWidget);
			}
		}
	}
}

void HomePageWidget::CreateDashboardWidget() {

	m_dashboardWidget = new QFrame(this);
	m_dashboardWidget->setObjectName("dashboard_frame");
	m_dashboardWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	m_dashboardWidget->setLineWidth(2);
	m_dashboardWidget->setMidLineWidth(2);

	m_mainDashboadLayout = new QVBoxLayout(m_dashboardWidget);
	m_mainDashboadLayout->setContentsMargins(30, 20, 30, 10);
	m_mainDashboadLayout->setSpacing(15);
	//mainDashboardLayout->setRowStretch(0, 1);
	//mainDashboardLayout->setRowStretch(1, 1);
	//mainDashboardLayout->setRowStretch(2, 2);

	QFrame* welcomeWidget = new QFrame(this);
	welcomeWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
	welcomeWidget->setLineWidth(2);
	welcomeWidget->setMidLineWidth(3);
	welcomeWidget->setStyleSheet("background-color: white;");

	QVBoxLayout* welcomeWidgetLayout = new QVBoxLayout(welcomeWidget);
	welcomeWidgetLayout->setSpacing(0);

	QLabel* welcomeLabel = new QLabel(tr("Welcome to"), welcomeWidget);
	welcomeLabel->setAlignment(Qt::AlignCenter);
	welcomeLabel->setStyleSheet("QLabel{font-size: 36pt; font-weight: bold; background-color: white;}");
	welcomeWidgetLayout->addWidget(welcomeLabel);

	QLabel* logoImage = new QLabel(m_dashboardWidget);
	logoImage->setAlignment(Qt::AlignCenter);
	logoImage->setStyleSheet("QLabel{background-color: white;}");
	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

		logoImage->setPixmap(QPixmap(":SGXGUI/Resources/GlyphEd/glyphed_logo_large.png").scaled(SynGlyphX::Application::DynamicQSize(584,110)));//584,110
	}
	else {

		QString topLogo = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/toplogo.png");
		if (QFileInfo::exists(topLogo)) {

			logoImage->setPixmap(QPixmap(topLogo));
		}
		else
		{
			logoImage->setPixmap(QPixmap(":SGXGUI/Resources/synglyphx_logo_large.png").scaled(SynGlyphX::Application::DynamicQSize(670, 130)));
		}
	}

	welcomeWidgetLayout->addWidget(logoImage);
	welcomeWidget->setLayout(welcomeWidgetLayout);

	m_mainDashboadLayout->addWidget(welcomeWidget);

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd() || SynGlyphX::GlyphBuilderApplication::AWSEnabled()) {
		loggedOn = m_mainWindow->IsUserLoggedIn();
	}
	CreateLowerHalfDashboardWidget();

	m_homePageWidgetsLayout->addWidget(m_dashboardWidget);

	m_stackedDashboardLayout->setCurrentIndex(1);
}

void HomePageWidget::SwitchDashboardLayout() {

	if (m_mainWindow->IsUserLoggedIn()) {

		loggedOn = true;
		SetCustomerLogo();
		m_stackedDashboardLayout->setCurrentIndex(0);
		if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd() || SynGlyphX::GlyphBuilderApplication::AWSEnabled()) {
			m_mainWindow->setMinimumWidth(1200);
			m_mainWindow->setMinimumHeight(700);
		}
	}
	else {

		loggedOn = false;
		loginWidget->Clear();
		m_stackedDashboardLayout->setCurrentIndex(1);
	}
}

void HomePageWidget::CreateLowerHalfDashboardWidget() {

	m_stackedDashboardLayout = new QStackedLayout(m_dashboardWidget);
	m_stackedDashboardLayout->setContentsMargins(0, 0, 0, 0);
	m_mainDashboadLayout->addLayout(m_stackedDashboardLayout);

	QWidget* loggedInWidget = new QWidget(m_dashboardWidget);
	QGridLayout* loggedInGridLayout = new QGridLayout(loggedInWidget);
	loggedInGridLayout->setContentsMargins(0, 0, 0, 0);
	loggedInGridLayout->setSpacing(15);
	loggedInGridLayout->setRowStretch(0, 1);
	loggedInGridLayout->setRowStretch(1, 2);

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
	m_recentViewsFilteringWidget->setStyleSheet("font-size: 11pt;");
	m_recentViewsFilteringWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	recentViewsLayout->addWidget(m_recentViewsFilteringWidget, 1);
	recentViewsWidget->setLayout(recentViewsLayout);

	loggedInGridLayout->addWidget(recentViewsWidget, 0, 0, 2, 1);

	QObject::connect(m_recentViewsFilteringWidget, &QListWidget::itemClicked, this, &HomePageWidget::OnRecentViewClicked);

	upperRightDashboardImage = new SynGlyphX::ResizeableImageLabel(true, m_dashboardWidget);
	upperRightDashboardImage->setAlignment(Qt::AlignCenter);
	upperRightDashboardImage->setFrameStyle(QFrame::Panel | QFrame::Raised);
	upperRightDashboardImage->setLineWidth(2);
	upperRightDashboardImage->setMidLineWidth(3);
	upperRightDashboardImage->setContentsMargins(10,10,10,10);
	upperRightDashboardImage->setStyleSheet("QLabel{background-color: white;}");

	//QString customerLogo = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/customer.png");
	//QString upperRightLogo = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/rightupper.png");

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd() || SynGlyphX::GlyphBuilderApplication::AWSEnabled()) {
		SetCustomerLogo();
	}

	loggedInGridLayout->addWidget(upperRightDashboardImage, 0, 1, 1, 2);
	loggedInGridLayout->addWidget(CreateLowerDashboardWidget(), 1, 1, 1, 2);

	loggedInWidget->setLayout(loggedInGridLayout);
	m_stackedDashboardLayout->addWidget(loggedInWidget);

	//m_allViewsFilteringWidget->Reset(m_dataEngineConnection);

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd() || SynGlyphX::GlyphBuilderApplication::AWSEnabled()) {
		QWidget* loggedOutWidget = new QWidget(m_dashboardWidget);
		QGridLayout* loggedOutLayout = new QGridLayout(loggedOutWidget);
		loggedOutLayout->setContentsMargins(0, 0, 0, 0);
		//loggedOutLayout->setColumnMinimumWidth(1, 550);
		loggedOutLayout->setRowStretch(0, 1);
		loggedOutLayout->setRowStretch(1, 1);
		//loggedOutLayout->setRowStretch(2, 1);
		loggedOutLayout->setColumnStretch(0, 1);
		loggedOutLayout->setColumnStretch(1, 1);
		loggedOutLayout->setColumnStretch(2, 1);
		loggedOutLayout->setSpacing(15);

		loginWidget = new DataEngine::UserLoginDialog(m_dataEngineConnection, this);
		loginWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
		loginWidget->setLineWidth(2);
		loginWidget->setMidLineWidth(3);
		//loginWidget->setMinimumHeight(250);
		loginWidget->setStyleSheet("background-color: white;");
		QObject::connect(loginWidget, &DataEngine::UserLoginDialog::LoginActivated, this, &HomePageWidget::Login);

		loginWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
		loggedOutLayout->addWidget(loginWidget, 0, 1, 1, 1);//, Qt::AlignCenter | Qt::AlignTop); //1,1
		loggedOutLayout->addWidget(CreateLowerDashboardWidget(), 1, 1, 1, 1);//, Qt::AlignCenter | Qt::AlignTop);//2,1

		loggedOutWidget->setLayout(loggedOutLayout);
		m_stackedDashboardLayout->addWidget(loggedOutWidget);
	}
}

void HomePageWidget::SetCustomerLogo() {


	QSettings st1;
	st1.beginGroup("LogoSettings");

	bool onStartupChecked = false;
	bool reqOnStartupChecked = false;
	if (m_dataEngineConnection->UserAccessControls()->GetUserID() != 0){
		QSettings settings;
		settings.beginGroup(QString::number(m_dataEngineConnection->UserAccessControls()->GetUserID()));
		onStartupChecked = settings.value("OnStartupChecked", false).toBool();
		reqOnStartupChecked = settings.value("ReqOnStartupChecked", false).toBool();
		settings.endGroup();
		if (!onStartupChecked && !reqOnStartupChecked){
			st1.setValue("LastLogo", SynGlyphX::GlyphBuilderApplication::IsGlyphEd() ? ":SGXGUI/Resources/GlyphEd/synglyphx_x_ED.ico" : ":SGXGUI/Resources/sgx_x.png");
		}
	}

	QString upperRightLogo = QDir::toNativeSeparators(QDir::cleanPath(m_dataEngineConnection->UserAccessControls()->GlyphEdPath()) + "/customer.png");
	if (!QFileInfo::exists(upperRightLogo)) {
		upperRightLogo = st1.value("LastLogo", ":SGXGUI/Resources/sgx_x.png").toString();
		if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()){
			upperRightLogo = st1.value("LastLogo", ":SGXGUI/Resources/GlyphEd/ucollege.png").toString();
		}
	}
	else{
		st1.setValue("LastLogo", upperRightLogo);
	}
	st1.endGroup();

	QPixmap p(upperRightLogo);
	upperRightDashboardImage->SetPixmap(QPixmap(upperRightLogo));
	upperRightDashboardImage->AddPadding(25, 0, 25, 0);
}

QWidget* HomePageWidget::CreateLowerDashboardWidget() {

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd() || SynGlyphX::GlyphBuilderApplication::AWSEnabled()) {

		QLabel* lowerRightLabel = new QLabel(m_dashboardWidget);
		lowerRightLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
		lowerRightLabel->setLineWidth(2);
		lowerRightLabel->setMidLineWidth(3);
		lowerRightLabel->setWordWrap(true);
		lowerRightLabel->setTextFormat(Qt::RichText);
		lowerRightLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
		lowerRightLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
		lowerRightLabel->setOpenExternalLinks(true);
		lowerRightLabel->setStyleSheet("font-size: 10pt; background-color: white; padding: 10px;");

		if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()){

			lowerRightLabel->setText(QString::fromStdWString(L"<p>Welcome to GlyphEd\u2122. GlyphEd's Enrollment Management GlyphKIT\u2122 is data visualization software designed "
				L"to help the Enrollment Management Office benefit from the breadth and depth of available Enrollment Management data. "
				L"We welcome your interest and input on product direction.</p>"
				L"<p>For Help & Support, please contact us at:  <a href=\"mailto:support@GlyphEd.co\">Support@GlyphEd.co</a></p>"
				L"<p>For Sales and Product Direction, please contact us at:  <a href=\"mailto:mark@GlyphEd.co\">Mark@GlyphEd.co</a></p>"
				L"<p>If you prefer to call, we can be reached at:  571.733.9469 during normal business hours, Eastern Time.</p>"));
		}
		else{
			lowerRightLabel->setText(QString::fromStdWString(L"<p>Welcome to GlyphViewer. "
				L"We welcome your interest and input on product direction.</p>"
				//L"<p>For Help & Support, please contact us at:  <a href=\"mailto:info@synglyphx.com\">Info@SynGlyphX.com</a></p>"
				L"<p>For Sales and Product Direction, please contact us at:  <a href=\"mailto:mark@synglyphx.com\">Mark@SynGlyphX.com</a></p>"
				L"<p>If you prefer to call, we can be reached at:  571.733.9469 during normal business hours, Eastern Time.</p>"));
		}
		return lowerRightLabel;
	}
	else {

		SynGlyphX::ResizeableImageLabel* lowerRightDashboardImage = new SynGlyphX::ResizeableImageLabel(true, m_dashboardWidget);
		lowerRightDashboardImage->setAlignment(Qt::AlignCenter);
		lowerRightDashboardImage->setFrameStyle(QFrame::Panel | QFrame::Raised);
		lowerRightDashboardImage->setLineWidth(2);
		lowerRightDashboardImage->setMidLineWidth(3);
		lowerRightDashboardImage->setStyleSheet("QLabel{background-color: white;}");

		QString lowerRightLogo = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/rightlower.png");
		if (QFileInfo::exists(lowerRightLogo)) {

			lowerRightDashboardImage->SetPixmap(QPixmap(lowerRightLogo));
		}

		return lowerRightDashboardImage;
	}
}

void HomePageWidget::Login(){

	if (loginWidget->Login()){
		m_mainWindow->MainWindow::UpdateUserMenu(m_dataEngineConnection->UserAccessControls()->NameOfUser());
		m_mainWindow->UpdateUserMenu();
		SwitchDashboardLayout();
		QTimer::singleShot(0, this, SLOT(ContinueWithLogin()));
	}
	else{
		QMessageBox critical_error(QMessageBox::Critical, tr("Failed To Login"), tr("Invalid username or password, please try again"), QMessageBox::Ok, this);
		critical_error.setDetailedText(m_dataEngineConnection->JavaErrors());
		critical_error.setStyleSheet("QLabel{margin-right:75px;},QTextEdit{min-width:500px;}");
		critical_error.setStandardButtons(QMessageBox::Ok);
		critical_error.setDefaultButton(QMessageBox::Ok);
		critical_error.setEscapeButton(QMessageBox::Ok);
		critical_error.exec();
	}
}

void HomePageWidget::ContinueWithLogin(){

	if (m_mainWindow->MainWindow::HasValidLicense()){

		QSettings settings;
		settings.beginGroup(QString::number(m_dataEngineConnection->UserAccessControls()->GetUserID()));
		QString groupName = settings.value("GroupName", "Default").toString();
		bool onStartupChecked = settings.value("OnStartupChecked", false).toBool();
		bool reqOnStartupChecked = settings.value("ReqOnStartupChecked", true).toBool();
		settings.endGroup();

		int ag = m_dataEngineConnection->UserAccessControls()->CheckAvailableGroups();
		QStringList fgns = m_dataEngineConnection->UserAccessControls()->GetFormattedGroupNames();
		m_mainWindow->CreateUserSettingsDialog(fgns);

		if (ag > 1 && (reqOnStartupChecked || onStartupChecked)){

			if (fgns.size() == 1){
				m_dataEngineConnection->UserAccessControls()->SetChosenGroup(fgns.at(0));
			}
			else if (onStartupChecked && fgns.contains(groupName)){
				m_dataEngineConnection->UserAccessControls()->SetChosenGroup(groupName);
				m_mainWindow->SetSelectedGroup(groupName);
			}
			else{
				GroupSelectionDialog* gsd = new GroupSelectionDialog(fgns, this);
				if (gsd->exec() == QDialog::Accepted) {
					groupName = gsd->GetSelectedGroup();
					m_dataEngineConnection->UserAccessControls()->SetChosenGroup(groupName);
					m_mainWindow->SetSelectedGroup(groupName);
				}
				else{
					ag = 0;
				}
			}
		}

		if (ag == 0 || (!reqOnStartupChecked && !onStartupChecked)){
			SwitchDashboardLayout();
		}
		else{
			SyncFilesAndLoadViews();

			QSettings groupSettings;
			groupSettings.beginGroup(groupName);
			groupSettings.setValue("LastUserToAccess", m_dataEngineConnection->UserAccessControls()->GetUserID());
			groupSettings.setValue("DirectoryPath", m_dataEngineConnection->UserAccessControls()->GlyphEdPath());
			groupSettings.setValue("VisualizationNames", m_dataEngineConnection->UserAccessControls()->VizualizationNames());
			groupSettings.endGroup();
		}
	}
	else{
		m_mainWindow->Logout();
	}
}

void HomePageWidget::SyncFilesAndLoadViews(){

	bool showReleaseNow = loggedOn;
	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	SynGlyphX::SyncProgressDialog *d = new SynGlyphX::SyncProgressDialog(m_dataEngineConnection, m_allViewsFilteringWidget, this);
	d->exec();
	LoggedOut();
	SynGlyphX::Application::restoreOverrideCursor();
	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd() && m_dataEngineConnection->UserAccessControls()->VisualizationsToSync() > 1){
		SynGlyphX::AnnouncementDialog* notesDialog = new SynGlyphX::AnnouncementDialog("Patch Notes", this);
		notesDialog->AddWebView("https://s3.amazonaws.com/glyphed/changes/patchnotes.html");
		notesDialog->show();
	}
	if (showReleaseNow && releaseDialog){
		releaseDialog->show();
	}
}

void HomePageWidget::ReSyncFilesAndLoadViews(){

	SynGlyphX::SyncProgressDialog *d = new SynGlyphX::SyncProgressDialog(m_dataEngineConnection, m_allViewsFilteringWidget, this);
	d->exec();
	LoggedOut();
}

void HomePageWidget::ResetViews(){
	m_allViewsFilteringWidget->ClearAll();
	m_optionsButtonGroup->button(0)->click();
}

void HomePageWidget::LoggedOut(){
	
	CreateHelpWidget();
	SwitchDashboardLayout();
}

void HomePageWidget::SetupGlyphEdViz() {

	//QStringList visualizationNames;
	//visualizationNames << "Global Admissions View" << "Reader View (2016)" << "High School View" << 
	//	"Class Composition (Diversity)" << "Class Composition (Cohort)" << "Class Composition (Total)" << "Global Dashboard";

	m_sourceDataCache.Setup(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + QDir::toNativeSeparators("/GlyphEd/glyphed.db"));

	m_glyphEdSDTFiles << QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + QDir::toNativeSeparators("/GlyphEd/Global Admissions/Global Admissions.sdt");
	m_glyphEdSDTFiles << QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + QDir::toNativeSeparators("/GlyphEd/Admissions Officer/Reader View.sdt");
	m_glyphEdSDTFiles << QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + QDir::toNativeSeparators("/GlyphEd/High School/High School.sdt");
	m_glyphEdSDTFiles << QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + QDir::toNativeSeparators("/GlyphEd/Class Composition/Class Composition Diversity.sdt");
	m_glyphEdSDTFiles << QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + QDir::toNativeSeparators("/GlyphEd/Class Composition/Class Composition Cohort.sdt");
	m_glyphEdSDTFiles << QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + QDir::toNativeSeparators("/GlyphEd/Class Composition/Class Composition Total.sdt");
	m_glyphEdSDTFiles << QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + QDir::toNativeSeparators("/GlyphEd/Global Dashboard/Global Dashboard.sdt");

	m_glyphEdTablesInDB << "GlobalAdmissions";
	m_glyphEdTablesInDB << "GlobalAdmissions";
	m_glyphEdTablesInDB << "GlobalAdmissions";
	m_glyphEdTablesInDB << "Composition";
	m_glyphEdTablesInDB << "Composition";
	m_glyphEdTablesInDB << "Composition";
	m_glyphEdTablesInDB << "Dashboard";

	for (auto i = 0; i < m_glyphEdTablesInDB.size(); ++i) {

		ProduceGlyphEdCSV(m_glyphEdSDTFiles[i], m_glyphEdTablesInDB[i], i);
	}

	/*MultiLoadingFilterWidget::VisualizationData globalAdmissionsView;
	globalAdmissionsView.m_title = visualizationNames[0];
	globalAdmissionsView.m_sdtPath = 
	globalAdmissionsView.m_tableInGlyphEd = ;

	globalAdmissionsView.m_filterTitles.push_back("Reader");
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
	admissionsCounselorView.m_tableInGlyphEd = "";

	admissionsCounselorView.m_filterTitles.push_back("Reader");
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
	globalDashboardView.m_filterTitles.push_back("Reader");
	globalDashboardView.m_filterFieldNames.push_back("reader_name");
	globalDashboardView.m_filterMultiselect.push_back(false);
	globalDashboardView.m_filterValues.push_back(m_sourceDataCache.GetSortedDistinctValuesAsStrings(globalDashboardView.m_tableInGlyphEd, "reader_name"));

	m_allVisualizationData.push_back(globalDashboardView);*/
}

void HomePageWidget::OnLoadVisualization() {

	QString fileToLoad;
	MultiTableDistinctValueFilteringParameters filteringParameters;

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	unsigned int whichFilteringWidget = m_homePageWidgetsLayout->currentIndex();
	if (whichFilteringWidget == 1) {

		if (!m_allViewsFilteringWidget->DoCurrentNecessaryFiltersHaveSelection()) {

			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::information(this, tr("Did not load visualization"), tr("Visualization can not be loaded until at least one value has been selected from all necessary filters."));
			return;
		}
		fileToLoad = m_allViewsFilteringWidget->GetCurrentFilename();
		filteringParameters = m_allViewsFilteringWidget->GetCurrentFilterValues();

		if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

			if (fileToLoad.endsWith("Global Dashboard.sdt")) {

				auto singleTableFilters = filteringParameters.begin();
				QSet<QString> readers;
				if (!filteringParameters.empty()) {

					if (singleTableFilters->second.HasFilters()) {

						readers = singleTableFilters->second.GetDistinctValueFilter("reader_name");
					}

				}
				readers.insert("TOTAL");
				singleTableFilters->second.SetDistinctValueFilter("reader_name", readers);
			}
		}

		DataEngine::GlyphEngine ge;
		ge.initiate(m_dataEngineConnection->getEnv(), fileToLoad.toStdString(), "", "", "", "GlyphViewer");
		for (const auto& filtersForTable : filteringParameters) {

			QString id = QString::fromStdWString(boost::uuids::to_wstring(filtersForTable.first.GetDatasourceID()));
			QString tableName = QString::fromStdWString(filtersForTable.first.GetTable());
			if (ge.SizeOfQuery(id, tableName, filtersForTable.second.GenerateQuery(filtersForTable.first)) == 0) {

				SynGlyphX::Application::restoreOverrideCursor();
				QMessageBox::information(this, tr("Did not load visualization"), tr("The selected combination of filters generate zero results.  Please try a different combination of filters."));
				return;
			}
		}
	}
	else if (whichFilteringWidget == 2) {

		if ( m_subsetViewsFilteringWidget->GetSelectedTooltips().size() > 0 )
		{
			fileToLoad = m_subsetViewsFilteringWidget->GetSelectedTooltips().front();
		}
		else
		{
			SynGlyphX::Application::restoreOverrideCursor();
			return;
		}
	}

	SynGlyphX::Application::restoreOverrideCursor();

	emit LoadVisualization(fileToLoad, filteringParameters, !filteringParameters.empty());
}

void HomePageWidget::ProduceGlyphEdCSV(const QString& sdtToLoad, const QString& tableInDB, unsigned int currentDataVisualization) {

	SynGlyphX::DataTransformMapping mapping;
	mapping.ReadFromFile(sdtToLoad.toStdString());
	FilteringParameters filters;

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
		/*else if (currentDataVisualization == 6) {

			QSet<QString> readers;
			if (filters.HasFilters()) {

				readers = filters.GetDistinctValueFilters().at(m_allVisualizationData[6].m_filterFieldNames[0]);

			}
			readers.insert("TOTAL");
			filters.SetDistinctValueFilter(m_allVisualizationData[6].m_filterFieldNames[0], readers);
		}*/

		QString dataFilename = QDir::toNativeSeparators(QString::fromStdWString(mapping.GetDatasources().begin()->second->GetDBName()));
		if (QFile::exists(dataFilename)) {

			QFile::remove(dataFilename);
		}

		bool didFilterHaveResult = m_sourceDataCache.ExportFilteredDataToCSV(dataFilename, tableInDB, filters);

		if (!didFilterHaveResult) {

			QMessageBox::warning(this, tr("GlyphEd Generate Visualization"), tr("The selected combination of filters had no results.  Please try a different combination of filters to load a visualization."));
		}
	}
}

void HomePageWidget::OnNewOptionSelected(int index) {

	if (index == (m_optionsButtonGroup->buttons().count() - 1)) {

		SynGlyphX::Application::instance()->exit();
	}

	m_homePageWidgetsLayout->setCurrentIndex(index);
	m_loadVisualizationButton->setVisible((index == 1) || (index == 2));
	m_mainWindow->update();
}

void HomePageWidget::OnRecentViewClicked(QListWidgetItem *item) {

	emit LoadRecentFile(item->toolTip());
}

void HomePageWidget::CheckForNewRelease() {

	QString os_path =
#ifdef WIN32
		"releases/windows";
#elif __APPLE__
		"releases/mac";
#endif

	bool isGlyphIt = QFile("DataMapper.exe").exists();
	QString appName = (QFileInfo(QCoreApplication::applicationFilePath()).fileName().toLower().replace(".exe","") == "glyphed") ? "glyphed" : "glyphit";
	std::vector<DataEngine::S3File*> files = s3Manager->GetFilesFromDirectory(appName.toStdString(), os_path.toStdString().c_str());
	std::string releaseName;
	for (const auto& file : files){ if (file->GetName().find("newrelease") != std::string::npos){ releaseName = file->GetName(); } }
	if (!releaseName.empty())
	{
		std::vector<std::string> x;
		boost::split(x, releaseName, boost::is_any_of("_"));
		std::string installName = appName.toStdString();
		if (appName != "glyphed" && !isGlyphIt) { installName = "glyphviewer"; }

		if (x.at(1) != SynGlyphX::getAppVersionString() && std::string(SynGlyphX::getAppVersionString()).find("t") == std::string::npos) {
			for (const auto& file : files) {
				if (file->GetName().find(installName + "_" + x.at(1) + ".") != std::string::npos) {
					releaseDialog = std::make_shared<SynGlyphX::AnnouncementDialog>("New Release Available", this);
					releaseDialog->AddLabel("https://s3.amazonaws.com/" + appName + "/" + os_path + "/" + releaseName.c_str());
					releaseDialog->ReplaceLabelText("***", QString::fromStdString(file->GetUrl()));
					releaseDialog->ReplaceLabelText("_._.__", x.at(1).c_str());
					releaseDialog->AddWebView("https://s3.amazonaws.com/" + appName + "/" + os_path + "/changes/changes_" + x.at(1).c_str() + "_.html");
					releaseDialog->resize(400, 250);
				}
			}
		}
	}
}
