#include "glyphedviewerwindow.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QSettings>
#include <QtCore/QProcessEnvironment>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QDockWidget>
#include <QtCore/QDateTime>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QColorDialog>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include "glyphbuilderapplication.h"
//#include "glyphviewerantztransformer.h"
#include "changedatasourcefiledialog.h"
#include "singlewidgetdialog.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include "filesystem.h"
#include "defaultbaseimageproperties.h"
#include "downloadexception.h"
#include <SceneViewer/sceneviewer.h>
#include "baseimage.h"
#include "sharedactionlist.h"
#include <boost/uuid/uuid_io.hpp>
#include "Profiler.h"
#include <hal/hal.h>
#include <QtCore/QUuid>
#include <QtGui/QScreen>
//#include <QtNetwork>
#include "sqlitewriter.h"
#include "version.h"
#include "S3FileManager.h"
#include "selecteddatawidget.h"
#include "interactivelegendwindow.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>

GlyphEdViewerWindow::GlyphEdViewerWindow(QWidget *parent)
	: SynGlyphX::MainWindow(4, parent),
	m_showErrorFromTransform(true),
	m_showHomePage(true),
    m_viewer( nullptr )
{
	SGX_PROFILE_SCOPE
	CreateMenus();

	menuBar()->hide();

	collapsed = false;
	m_dataEngineConnection = std::make_shared<DataEngine::DataEngineConnection>();
	m_mappingModel = new SynGlyphX::DataTransformModel(this);
	m_mappingModel->SetDataEngineConnection(m_dataEngineConnection);

	CreateDockWidgets();

	centerWidgetsContainer = new QStackedWidget(this);
	centerWidgetsContainer->setContentsMargins(0, 0, 0, 0);
	centerWidgetsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setCentralWidget(centerWidgetsContainer);

	QString server_addr;
	QFile serverFile("server.txt");
	if (serverFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&serverFile);
		while (!in.atEnd())
		{
			server_addr = in.readLine();
			break;
		}
		serverFile.close();
	}
	if (!server_addr.startsWith('e') || !server_addr.endsWith('m')) {
		server_addr = "ec2-52-41-239-60.us-west-2.compute.amazonaws.com";
	}
	//QMessageBox::information(this, tr("Server message"), server_addr);

	uid = QUuid::createUuid().toString();
	uid.remove(QChar('{'));
	uid.remove(QChar('}'));
	dlg = new QWebEngineView(this);
	auto wep = dlg->page()->profile();
	wep->setHttpAcceptLanguage("en-US,en;q=0.9");
	dlg->load(QUrl("http://" + server_addr + ":5000?uid=" + uid));
	centerWidgetsContainer->addWidget(dlg);

	QScreen *screen = QGuiApplication::primaryScreen();
	QRect screenGeometry = screen->geometry();
	if (screenGeometry.width() == 3840) {
		dlg->setZoomFactor(2);
	}
	int width = screenGeometry.width() - (screenGeometry.width()*0.235);//450;
	int height = screenGeometry.height() - 83;
	m_viewer = new SynGlyphX::SceneViewer(this, SynGlyphX::ViewerMode::Full);
	m_viewer->setFilteredResultsDisplayMode(SynGlyphX::FilteredResultsDisplayMode::HideUnfiltered);
	m_viewer->setGeometry(0, 0, width, height);
	m_viewer->hide();

	QObject::connect(m_viewer, &SynGlyphX::SceneViewer::interactiveLegendToggled, this, &GlyphEdViewerWindow::ToggleInteractiveLegend);

	//CONTEXT MENU
	m_enableDisableFlyToObjectAction = new QAction(tr("Enable/Disable Fly-to-Object"), this);
	m_enableDisableFlyToObjectAction->setCheckable(true);
	m_enableDisableFlyToObjectAction->setChecked(false);
	m_enableDisableFlyToObjectAction->setEnabled(false);
	m_loadedVisualizationDependentActions.push_back(m_enableDisableFlyToObjectAction);
	QObject::connect(m_enableDisableFlyToObjectAction, &QAction::toggled, this, &GlyphEdViewerWindow::OnEnableDisableFlyToObjectAction);

	m_enableDisableFreeSelectionCameraAction = new QAction(tr("Enable/Disable Orbit Selection"), this);
	m_enableDisableFreeSelectionCameraAction->setCheckable(true);
	m_enableDisableFreeSelectionCameraAction->setChecked(true);
	m_enableDisableFreeSelectionCameraAction->setEnabled(false);
	m_loadedVisualizationDependentActions.push_back(m_enableDisableFreeSelectionCameraAction);
	QObject::connect(m_enableDisableFreeSelectionCameraAction, &QAction::toggled, this, &GlyphEdViewerWindow::OnEnableDisableFreeSelectionCamera);
	
	m_enableDisableSelEffectActionMenu = new QAction(tr("Enable/Disable Selection Effect"), this);
	m_enableDisableSelEffectActionMenu->setCheckable(true);
	m_enableDisableSelEffectActionMenu->setChecked(true);
	m_enableDisableSelEffectActionMenu->setEnabled(false);
	m_loadedVisualizationDependentActions.push_back(m_enableDisableSelEffectActionMenu);
	QObject::connect(m_enableDisableSelEffectActionMenu, &QAction::toggled, this, &GlyphEdViewerWindow::OnEnableDisableSelEffect);

	m_enableDisableSuperimposedGlyphGadgets = new QAction(tr("Enable/Disable Overlapping Group Indicators"), this);
	m_enableDisableSuperimposedGlyphGadgets->setCheckable(true);
	m_enableDisableSuperimposedGlyphGadgets->setChecked(false);
	m_enableDisableSuperimposedGlyphGadgets->setEnabled(false);
	m_loadedVisualizationDependentActions.push_back(m_enableDisableSuperimposedGlyphGadgets);
	QObject::connect(m_enableDisableSuperimposedGlyphGadgets, &QAction::toggled, this, &GlyphEdViewerWindow::OnEnableDisableSuperimposedGadgets);

	m_viewer->addAction(m_showTagsAction);
	m_viewer->addAction(m_hideTagsAction);
	m_viewer->addAction(m_hideAllTagsAction);
	m_viewer->addAction(SynGlyphX::SharedActionList::CreateSeparator(m_viewer));
	m_viewer->addAction(m_enableDisableFlyToObjectAction);
	m_viewer->addAction(m_enableDisableFreeSelectionCameraAction);
	m_viewer->addAction(m_enableDisableSelEffectActionMenu);
	m_viewer->addAction(m_enableDisableSuperimposedGlyphGadgets);
	m_viewer->addAction(m_clearSelectionAction);
	//CONTEXT MENU
	m_viewer->setContextMenuPolicy(Qt::ActionsContextMenu);

	QObject::connect(m_showTagsAction, &QAction::triggered, this, [this]{ m_viewer->showTagsOfSelectedObjects(true); });
	QObject::connect(m_hideTagsAction, &QAction::triggered, this, [this]{ m_viewer->showTagsOfSelectedObjects(false); });
	QObject::connect( m_hideAllTagsAction, &QAction::triggered, this, [this]{ m_viewer->hideAllTags(); });

	m_viewer->setOnSelectionChanged([this](bool selection_exists)
	{
		m_clearSelectionAction->setEnabled(selection_exists);
		m_showTagsAction->setEnabled(selection_exists);
		m_hideTagsAction->setEnabled(selection_exists);
	});

	try {

		if (!m_dataEngineConnection->hasJVM()) {

			m_dataEngineConnection->createJVM();
		}
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("JVM Error"), tr(e.what()));
		return;
	}
	
	try {
		
		//CreateANTzWidget();
	}

	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, tr("3D view error"), tr("3D view failed to create: ") + e.what());
		throw;
	}

	try {

		m_cacheManager.SetBaseCacheDirectory(GlyphViewerOptions::GetDefaultCacheDirectory().toStdWString());
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, tr("Cache Directory Error"), tr("Cache Directory Error: ") + e.what());
		throw;
	}

	m_linkedWidgetsManager = new LinkedWidgetsManager(m_viewer, this);

	//CreateInteractionToolbar();

	centerWidgetsContainer->setCurrentIndex(0);

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);

	EnableLoadedVisualizationDependentActions( true );

	UpdateFilenameWindowTitle(s_noFileName);

	std::vector<std::string> images;

	/*file.setFileName("qLog.txt");
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);
	out << "Initiating Socket \n" << endl;
	out << uid << endl;*/

	//EchoClient client(QUrl(QStringLiteral("ws://ec2-34-221-39-241.us-west-2.compute.amazonaws.com:5001")), false);
	//connect(&client, &EchoClient::setLaunch, this, &GlyphEdViewerWindow::OnLaunch);
	QObject::connect(&m_webSocket, &QWebSocket::connected, this, &GlyphEdViewerWindow::OnSocketConnect);
	QObject::connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &GlyphEdViewerWindow::OnSocketLaunch);
	QObject::connect(&m_webSocket, &QWebSocket::close, this, &GlyphEdViewerWindow::OnSocketClosed);
	QString ws_addr("ws://" + server_addr + ":5001");
	m_webSocket.open(QUrl(ws_addr));

}

GlyphEdViewerWindow::~GlyphEdViewerWindow()
{

}

void GlyphEdViewerWindow::CreateANTzWidget() {
	
	QStackedWidget* antzWidgetContainer = dynamic_cast<QStackedWidget*>(centralWidget());
	if (m_viewer) {
		
		antzWidgetContainer->removeWidget(m_viewer);
		delete m_viewer;
		m_viewer = nullptr;
	}

	m_viewer = new SynGlyphX::SceneViewer( this, SynGlyphX::ViewerMode::Full, true );

	m_enableDisableSelEffectActionMenu = new QAction( tr( "Enable/Disable Selection Effect" ), this );
	m_enableDisableSelEffectActionMenu->setCheckable( true );
	m_enableDisableSelEffectActionMenu->setChecked( true );
	m_enableDisableSelEffectActionMenu->setEnabled( false );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableSelEffectActionMenu );
	QObject::connect( m_enableDisableSelEffectActionMenu, &QAction::toggled, this, &GlyphEdViewerWindow::OnEnableDisableSelEffect );

	m_viewer->addAction(SynGlyphX::SharedActionList::CreateSeparator(m_viewer));
	m_viewer->addAction(m_showTagsAction);
	m_viewer->addAction(m_hideTagsAction);
	m_viewer->addAction(m_hideAllTagsAction);
	m_viewer->addAction(SynGlyphX::SharedActionList::CreateSeparator(m_viewer));
	m_viewer->addAction(m_enableDisableSelEffectActionMenu);
	m_viewer->addAction(m_clearSelectionAction);

	m_viewer->setContextMenuPolicy( Qt::ActionsContextMenu );

	antzWidgetContainer->addWidget(m_viewer);
	antzWidgetContainer->setCurrentWidget(m_viewer);
	
	//QObject::connect( m_resetCameraToDefaultPosition, &QAction::triggered, this, [this]{ m_viewer->resetCamera(); } );
	//QObject::connect(m_showAnimation, &QAction::toggled, this, [this]{ m_viewer->enableAnimation( !m_viewer->animationEnabled() ); });
	//QObject::connect(m_showTagsAction, &QAction::triggered, this, [this]{ m_viewer->showTagsOfSelectedObjects(true); });
	//QObject::connect(m_hideTagsAction, &QAction::triggered, this, [this]{ m_viewer->showTagsOfSelectedObjects(false); });
	//QObject::connect( m_hideAllTagsAction, &QAction::triggered, this, [this]{ m_viewer->hideAllTags(); });

	m_viewer->setOnSelectionChanged( [this]( bool selection_exists )
	{
		m_clearSelectionAction->setEnabled( selection_exists );
		m_showTagsAction->setEnabled( selection_exists );
		m_hideTagsAction->setEnabled( selection_exists );
	} );
}

void GlyphEdViewerWindow::closeJVM(){

	m_dataEngineConnection->destroyJVM();
}

void GlyphEdViewerWindow::CreateMenus() {

	//Create File Menu
	//m_fileMenu = menuBar()->addMenu(tr("File"));

	//QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
	//QObject::connect(exitAction, &QAction::triggered, this, &GlyphEdViewerWindow::close);

	//Create View Menu
	CreateViewMenu();

	m_stereoAction = CreateMenuAction(m_viewMenu, tr("Stereo"));
	m_stereoAction->setCheckable(true);
	QObject::connect(m_stereoAction, &QAction::triggered, this, &GlyphEdViewerWindow::ChangeStereoMode);

	m_viewMenu->addSeparator();

	//m_showAnimation = m_viewMenu->addAction(tr("Show Animation"));
	//m_showAnimation->setCheckable(true);
	//m_showAnimation->setChecked(true);

	m_viewMenu->addSeparator();

	m_showTagsAction = CreateMenuAction(m_viewMenu, tr("Show Tags For Selected Objects"), Qt::Key_I);

	m_hideTagsAction = CreateMenuAction(m_viewMenu, tr("Hide Tags For Selected Objects"), Qt::Key_O);

	m_hideAllTagsAction = CreateMenuAction(m_viewMenu, tr("Hide All Visible Tags"));
	m_loadedVisualizationDependentActions.push_back(m_hideAllTagsAction);

	m_viewMenu->addSeparator();

	m_clearSelectionAction = CreateMenuAction(m_viewMenu, tr("Clear Selection"));
	m_clearSelectionAction->setEnabled(false);
	QObject::connect( m_clearSelectionAction, &QAction::triggered, this, [&]() { m_viewer->getScene().clearSelection(); } );

	m_viewMenu->addSeparator();

	m_resetCameraToDefaultPosition = CreateMenuAction(m_viewMenu, tr("Reset Camera To Starting Position"));
	QIcon resetCamIcon;
	resetCamIcon.addFile( ":SGXGUI/Resources/Icons/icon-reset-camera.png", QSize(), QIcon::Normal, QIcon::On );
	m_resetCameraToDefaultPosition->setIcon( resetCamIcon );
	m_loadedVisualizationDependentActions.push_back(m_resetCameraToDefaultPosition);

	m_viewMenu->addSeparator();

	//CreateHelpMenu();

	//QAction* openGLSettingsAction = new QAction(tr("OpenGL Settings"), m_helpMenu);
	//QObject::connect(openGLSettingsAction, &QAction::triggered, this, &GlyphEdViewerWindow::ShowOpenGLSettings);

	//m_helpMenu->insertAction(m_aboutBoxAction, openGLSettingsAction);
	//m_helpMenu->insertSeparator(m_aboutBoxAction);

#ifdef __APPLE__
	menuBar()->addSeparator();
#endif
	SynGlyphX::MainWindow::CreateLoginMenu();

}

void GlyphEdViewerWindow::CreateDockWidgets() {

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	m_legendsWidget = new LegendsDisplayWidget(this);
	m_legendsDockWidget = new QDockWidget(tr("Legends"), this);
	m_legendsDockWidget->setAllowedAreas(Qt::NoDockWidgetArea);
	m_legendsDockWidget->setWidget(m_legendsWidget);
	m_legendsDockWidget->setFloating(true);
	QIcon legendIcon;
	legendIcon.addFile(":SGXGUI/Resources/Icons/icon-legend.png", QSize(), QIcon::Normal, QIcon::Off);
	legendIcon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
	QAction* act = m_legendsDockWidget->toggleViewAction();
	//act->setIconVisibleInMenu(false);
	act->setIcon(legendIcon);
	m_loadedVisualizationDependentActions.push_back(act);
	m_viewMenu->addAction(act);
	m_legendsDockWidget->move(100, 100);
	m_legendsDockWidget->resize(400, 280);
	m_legendsDockWidget->hide();

	m_rightDockWidget = new QDockWidget(tr("Filtering"), this);
	//m_filteringWidget = new FilteringWidget(m_columnsModel, m_filteringManager, m_rightDockWidget);
	QWebEngineView* dlg = new QWebEngineView(this);
	//dlg->setMinimumSize(width, height);
	dlg->load(QUrl("https://synglyphx.s3.amazonaws.com/Filter+Bar/filters.html"));
	m_rightDockWidget->setWidget(dlg);
	addDockWidget(Qt::RightDockWidgetArea, m_rightDockWidget);
	act = m_rightDockWidget->toggleViewAction();
	m_loadedVisualizationDependentActions.push_back(act);
	QIcon filterIcon;
	QPixmap filter_off(":SGXGUI/Resources/Icons/icon-filter.png");
	QPixmap filter_on(":SGXGUI/Resources/Icons/icon-filter-a.png");
	filterIcon.addPixmap(filter_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	filterIcon.addPixmap(filter_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	act->setIcon(filterIcon);
	//m_viewMenu->addAction(act);
	//m_showHideToolbar->addAction(act);
	m_rightDockWidget->hide();

	m_interactiveLegend = new InteractiveLegendWindow(this, m_mappingModel->GetDataMapping());
	m_interactiveLegendDock = new QDockWidget(tr("Interactive Legend"), this);
	m_interactiveLegendDock->hide();
	m_interactiveLegendDock->setFloating(true);
	m_interactiveLegendDock->setWidget(m_interactiveLegend);
	m_interactiveLegendAction = m_interactiveLegendDock->toggleViewAction();
}

void GlyphEdViewerWindow::OnSocketConnect() {

QTextStream out(&file);
out << "Socket opened" << endl;

QString toSend("uid=" + uid);
m_webSocket.sendTextMessage(toSend);
}

void GlyphEdViewerWindow::OnSocketLaunch(QString message) {
	
	//QTextStream out(&file);
	//out << "Message received: " << message << endl;

	QStringList qsl = message.split(QChar(','));
	QString type = qsl.at(0).split(QChar(':')).at(1);
	type.remove(QChar('"'));
	type.remove(QChar('}'));
	//out << "{{" + type + "}}" << endl;

	QString text;
	if (type == "LAUNCH"){
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		zero_results = false;
		QString sdt = "https" + message.split(QChar(':')).at(3).split(QChar('"')).at(0);
		QString query = message.split(QChar(':')).at(4).split(QChar(';')).at(0) + ";\"";
		QStringList legends = message.split("\"legendURLArr\":[").at(1).split(QChar(']')).at(0).split(QChar(','));
		QString inst = message.split("\"institution\":")[1].split(QChar('"')).at(1);
		//text = "Received launch response from server. \n Launch " + query;
		std::vector<std::string> bimgs = MakeDataRequest(query, sdt, legends, inst);
		if (!zero_results){
			QSize size = this->size();
			m_viewer->resize(size.width() - (size.width()*0.235), size.height() - 20);
			m_viewer->show();
			m_viewer->loadScene((cache_location + "scene/glyphs.sgc").toStdString().c_str(), (cache_location + "scene/glyphs.sgn").toStdString().c_str(), bimgs);
			m_viewer->setAxisNames(compass.at(0).c_str(), compass.at(1).c_str(), compass.at(2).c_str());
		}
		SynGlyphX::Application::restoreOverrideCursor();
		//QMessageBox::information(this, tr("Server message"), message);
	}
	else if (type == "FILTER"){
		//QMessageBox::information(this, tr("Server message"), message);
		QString ids = message.split(QChar('[')).at(1).split(QChar(']')).at(0);
		SynGlyphX::IndexSet myset = parseFilters(ids);
		filter_ids.clear();
		std::set<unsigned long>::iterator it;
		for (it = myset.begin(); it != myset.end(); ++it){
			filter_ids.append(QString::number(*it+1));
		}
		text = "Received launch response from server. \n Filter [" + filter_ids.join(", ") + "]";
		m_viewer->setFilteredResults(parseFilters(ids), true);
	}
	else if (type == "HOME" || type == "LOGOUT"){
		text = "Received launch response from server. \n Home " + uid;
		m_viewer->hide();
		m_interactiveLegend->reset();
	}
	else if (type == "OPEN_URL") {
		try {
			QString url = message.split("open_url\":")[1].split("\"")[1];
			QDesktopServices::openUrl(QUrl(url));
		}
		catch (...) {
			QMessageBox::information(this, tr("Server message"), "Failed to load the associated url.");
		}
	}
	else if (type == "VFILTERS LOAD DONE"){
		text = "Received launch response from server. \n Load Done " + uid;
	}
	else if (type == "VFILTERS CLEAR ALL"){
		text = "Received launch response from server. \n Clear All " + uid;
		m_viewer->setFilteredResults(SynGlyphX::IndexSet(), true);
		filter_ids.clear();
	}
	else if (type == "VIEW_SELECTED"){
		std::vector<int> ids = m_viewer->getScene().getSelectionIds();
		if (ids.size() != 0){
			QStringList str_ids;
			for (int id : ids){
				str_ids << QString::number(id + 1);
			}
			SelectedDataWidget* sdw = new SelectedDataWidget(this, cache_location, table_name, str_ids);
			sdw->show();
		}
		else{
			QMessageBox::information(this, tr("Server message"), "No Glyphs are currently selected.");
		}
	}
	else if (type == "VIEW_FILTERED"){
		if (filter_ids.size() != 0){
			SelectedDataWidget* sdw = new SelectedDataWidget(this, cache_location, table_name, filter_ids);
			sdw->show();
		}
		else{
			QMessageBox::information(this, tr("Server message"), "No Glyphs are currently filtered.");
		}
	}
	else if (type == "VIEW_STATS"){
		QMessageBox::information(this, tr("Server message"), "VIEW_STATS");
	}
	else if (type == "COLLAPSE_SIDENAV"){
		QSize size = this->size();
		m_viewer->resize(size.width() - (size.width()*0.026), size.height() - 20);
		collapsed = true;
	}
	else if (type == "EXPAND_SIDENAV"){
		QSize size = this->size();
		m_viewer->resize(size.width() - (size.width()*0.235), size.height() - 20);
		collapsed = false;
	}
	else if (type == "SETTINGS_MODAL"){
		//QMessageBox::information(this, tr("Server message"), "SETTINGS_MODAL");
		QDialog* settings_modal = new QDialog(this);
		settings_modal->setWindowTitle("Settings");

		QVBoxLayout* settingsLayout = new QVBoxLayout(settings_modal);
		QGroupBox* settingsGroupBox = new QGroupBox(tr("Settings"), settings_modal);

		QVBoxLayout* settingsInnerLayout = new QVBoxLayout(settings_modal);

		QCheckBox* m_enableFlyToCheckBox = new QCheckBox(tr("Enable/Disable Fly-to-Object"), settings_modal);
		m_enableFlyToCheckBox->setCheckState(m_enableDisableFlyToObjectAction->isChecked() ? Qt::Checked : Qt::Unchecked);
		settingsInnerLayout->addWidget(m_enableFlyToCheckBox);
		QObject::connect(m_enableFlyToCheckBox, &QCheckBox::toggled, m_enableDisableFlyToObjectAction, &QAction::toggle);

		QCheckBox* m_enableOrbitCheckBox = new QCheckBox(tr("Enable/Disable Orbit Selection"), settings_modal);
		m_enableOrbitCheckBox->setCheckState(m_enableDisableFreeSelectionCameraAction->isChecked() ? Qt::Checked : Qt::Unchecked);
		settingsInnerLayout->addWidget(m_enableOrbitCheckBox);
		QObject::connect(m_enableOrbitCheckBox, &QCheckBox::toggled, m_enableDisableFreeSelectionCameraAction, &QAction::toggle);

		QCheckBox* m_enableSelectionCheckBox = new QCheckBox(tr("Enable/Disable Selection Effect"), settings_modal);
		m_enableSelectionCheckBox->setCheckState(m_enableDisableSelEffectActionMenu->isChecked() ? Qt::Checked : Qt::Unchecked);
		settingsInnerLayout->addWidget(m_enableSelectionCheckBox);
		QObject::connect(m_enableSelectionCheckBox, &QCheckBox::toggled, m_enableDisableSelEffectActionMenu, &QAction::toggle);

		QCheckBox* m_enableGroupCheckBox = new QCheckBox(tr("Enable/Disable Overlapping Group Indicators"), settings_modal);
		m_enableGroupCheckBox->setCheckState(m_enableDisableSuperimposedGlyphGadgets->isChecked() ? Qt::Checked : Qt::Unchecked);
		settingsInnerLayout->addWidget(m_enableGroupCheckBox);
		QObject::connect(m_enableGroupCheckBox, &QCheckBox::toggled, m_enableDisableSuperimposedGlyphGadgets, &QAction::toggle);

		settingsGroupBox->setLayout(settingsInnerLayout);
		settingsLayout->addWidget(settingsGroupBox);
		settings_modal->setLayout(settingsLayout);
	
		settings_modal->exec();
	}

	//QMessageBox::information(this, tr("Server message"), text);

	//m_webSocket.close();
}

void GlyphEdViewerWindow::OnSocketClosed() {

	QTextStream out(&file);
	out << "Socket closed" << endl;

	QMessageBox::information(this, tr("Server message"), "Socket closed.");
}

std::vector<std::string> GlyphEdViewerWindow::MakeDataRequest(QString query, QString sdt, QStringList legends, QString inst) {

	QNetworkRequest request(QUrl("http://ec2-18-224-124-242.us-east-2.compute.amazonaws.com:8000/fetchall"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QJsonObject body;
	body.insert("query", query.mid(1, query.length() - 3).replace("\\", ""));
	body.insert("institution", inst);
	//QMessageBox::information(this, tr("Server message"), query.mid(1, query.length() - 3).replace("\\", ""));

	QNetworkAccessManager nam;
	QNetworkReply *reply = nam.post(request, QJsonDocument(body).toJson());
	while (!reply->isFinished())
	{
		qApp->processEvents();
	}
	//QMessageBox::information(this, tr("Server message"), "Received a reply.");

	QByteArray response_data = reply->readAll();
	QJsonDocument json = QJsonDocument::fromJson(response_data);
	QJsonArray data = json.array();

	//QMessageBox::information(this, tr("Server message"), QString::number(response_data.size()));

	reply->deleteLater();

	if (response_data.size() == 2){
		//QMessageBox::information(this, tr("Server message"), "No matches");
		zero_results = true;
		return std::vector<std::string>();
	}

	DataEngine::GlyphEngine ge;
	ge.initiate(m_dataEngineConnection->getEnv(), "", "", "", "", "GlyphViewer", false);

	QString uuid = QUuid::createUuid().toString().replace("{", "").replace("}", "");
	std::string dcd = GlyphViewerOptions::GetDefaultCacheDirectory().toStdString();
	std::string cacheDirectoryPath = dcd + ("/cache_" + uuid.toStdString() + "/");
	//std::string cacheDirectoryPath = dcd + ("/cache_" + boost::uuids::to_string(m_mappingModel->GetDataMapping()->GetID()));

	cache_location = QString::fromStdString(cacheDirectoryPath);//"C:/Users/bryan/OneDrive/Documents/GitHub/sgx/bin/Win64/Release/VizFiles/";
	QStringList sdt_split = sdt.split(QChar('/'));
	QString filename = cache_location + sdt_split.at(sdt_split.size() - 1);

	//QMessageBox::information(this, tr("Server message"), legends.at(0));

	table_name = query.split("FROM ").at(1).split(" ").at(0);

	QString bucket_name = sdt.split(QChar('.')).at(0).split("/").at(2);
	QString key_name = sdt.split(".com/").at(1);
	//QMessageBox::information(this, tr("Server message"), bucket_name + " | " + key_name);
	QStringList legend_list;
	ge.DownloadFiles(bucket_name, key_name, cache_location);
	for (int i = 0; i < legends.length(); i++) {
		if (!legends.at(i).contains(".png") && !legends.at(i).contains(".jpg")) {
			legends.removeAt(i);
		}
	}
	for (QString legend : legends) {
		QStringList legend_parts = legend.left(legend.length()-1).split(QChar('/'));
		int legend_length = legend_parts.size();
		QString legend_key = legend_parts.at(legend_length-2) + "/" + legend_parts.at(legend_length-1);
		legend_list << legend_parts.at(legend_length - 1);
		ge.DownloadFiles(bucket_name, legend_key, cache_location);
	}

	m_viewer->setupLegendWindow(cache_location, legend_list);
	m_mappingModel->LoadDataTransformFile(filename);
	std::string baseImageDir = SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation().toStdString();

	QMessageBox::information(this, tr("Server message"), QString::number(data.size()));

	SqliteWriter* sql_writer = new SqliteWriter();
	QString ret_query = sql_writer->WriteDatabase(cache_location, data, sdt_split.at(sdt_split.size() - 1), table_name);

	ge.initiate(m_dataEngineConnection->getEnv(), filename.toStdString(), cache_location.toStdString(), baseImageDir, "", "GlyphViewer");

	QString id = sql_writer->GetId();
	QStringList tableNames = sql_writer->GetTableNames();
	QString table_query = "SELECT * FROM " + tableNames.at(0);
	ge.SetQueryForDatasource(id, tableNames.at(0), table_query);

	if (ge.IsUpdateNeeded()){
		DownloadBaseImages(ge);
		ge.generateGlyphs(this);
	}

	compass = ge.getCompassValues();

	bool show = true;
	for (const SynGlyphX::BaseImage& baseImage : m_mappingModel->GetDataMapping().get()->GetBaseObjects()) {
		if (baseImage.GetType() == SynGlyphX::BaseImage::Type::DownloadedMap) {
			show = false;
		}
	}
	m_viewer->enableSceneAxes(show);

	return ge.getBaseImages();
}

void GlyphEdViewerWindow::DownloadBaseImages(DataEngine::GlyphEngine& ge) {

	//QTextStream out(&file);
	//out << "{{DBI Start}}" << endl;

	try {

		const std::vector<SynGlyphX::BaseImage>& bis = m_mappingModel->GetDataMapping().get()->GetBaseObjects();
		//out << "{{DBI BIS returned}}" << bis.size() << endl;
		ge.getDownloadedBaseImage(bis);
	}
	catch (const DownloadException& e) {

		SynGlyphX::Application::restoreOverrideCursor();
		QMessageBox::information(this, "Download Image Error", tr("Base image failed to download so the world map was used instead.\n\nError: ") + tr(e.what()), QMessageBox::Ok);
	}
	catch (const std::exception&) {
	
		throw;
	}
}

void GlyphEdViewerWindow::resizeEvent(QResizeEvent* event) {

	//QMessageBox::information(this, tr("Server message"), "resize");
	QSize size = this->size();

	if (collapsed){
		m_viewer->resize(size.width() - (size.width()*0.026), size.height() - 20);
	}
	else {
		m_viewer->resize(size.width() - (size.width()*0.235), size.height() - 20);
	}

	SynGlyphX::MainWindow::resizeEvent(event);

}

SynGlyphX::IndexSet GlyphEdViewerWindow::parseFilters(QString response) {
	//typedef std::set<unsigned long> IndexSet;
	QStringList rowids = response.split(QChar(','));
	//QMessageBox::information(this, tr("Server message"), rowids.join(','));
	SynGlyphX::IndexSet is;
	for (QString row : rowids){
		is.insert(row.split(QChar(':')).at(1).split(QChar('}')).at(0).toInt()-1);
	}
	return is;
}

void GlyphEdViewerWindow::ShowOpenGLSettings() {

	const auto& format = m_viewer->format();
	QString settings = tr("OpenGL Version = ") + QString::number(format.majorVersion()) + "." + QString::number(format.minorVersion()) + ( ( format.profile() == QSurfaceFormat::CoreProfile ) ? " (core profile)" : " (compatibility profile)" ) + '\n';

	settings += tr( "Stereo Support" ) + " = ";
	if ( m_viewer->stereoSupported() )
		settings += tr( "enabled\n" );
	else
		settings += tr( "disabled\n" );

	auto device_info = SynGlyphX::hal::device::get_device_info();
	char buf[2048];
	sprintf( buf, "Device reports:\n\tVendor: %s\n\tRenderer: %s\n\tVersion: %s", device_info.vendor, device_info.renderer, device_info.version );
	settings += buf;

	QMessageBox::information(this, tr("OpenGL Settings"), settings);
}

void GlyphEdViewerWindow::ChangeStereoMode() {

	if (m_viewer->stereoSupported()) {

		try {

			m_viewer->setStereoMode(!m_viewer->stereoMode());
		}
		catch (const std::exception& e) {

			QMessageBox::information(this, tr("Error"), tr("Stereo Error: ") + e.what());
		}
	}
	else {

		//m_stereoAction->setChecked(false);
		QMessageBox::information(this, tr("Stereo not supported"), tr("Stereo is not supported. Check your driver settings to see if stereo is enabled and available for your hardware.  Contact the manufacturer of your GPU for assitance."));
	}
}

void GlyphEdViewerWindow::EnableLoadedVisualizationDependentActions(bool enable) {

	for (int i = 0; i < m_loadedVisualizationDependentActions.length(); ++i) {

		m_loadedVisualizationDependentActions[i]->setEnabled(enable);
	}

	m_stereoAction->setEnabled(!enable);

	m_clearSelectionAction->setEnabled(false);
	m_showTagsAction->setEnabled(false);
	m_hideTagsAction->setEnabled(false);
}

void GlyphEdViewerWindow::ChangeBackgroundColor() {
	QColor color = QColorDialog::getColor(Qt::black, this);
	if (color.isValid())
	{
		auto r = color.redF();
		auto g = color.greenF();
		auto b = color.blueF();
		if (m_viewer) m_viewer->setBackgroundColor(glm::vec4(float(r), float(g), float(b), 1.f));
	}
}

void GlyphEdViewerWindow::closeEvent(QCloseEvent* event) {

	SynGlyphX::MainWindow::closeEvent(event);
}

void GlyphEdViewerWindow::CreateInteractionToolbar() {

	m_interactionToolbar = addToolBar(tr("Interaction"));
	m_interactionToolbar->setObjectName("main_toolbar");
	m_interactionToolbar->setFloatable(true);
	m_interactionToolbar->setMovable(true);
	m_interactionToolbar->setIconSize(QSize(42, 32));

	m_showHideHUDAxisAction = new QAction(tr("Show/Hide HUD Axis"), m_interactionToolbar);
	m_showHideHUDAxisAction->setCheckable(true);
	QIcon hudAxisIcon;
	hudAxisIcon.addFile(":SGXGUI/Resources/Icons/icon-axis-hud.png", QSize(), QIcon::Normal, QIcon::Off);
	hudAxisIcon.addFile(":SGXGUI/Resources/Icons/icon-axis-hud-a.png", QSize(), QIcon::Normal, QIcon::On);
	m_showHideHUDAxisAction->setIcon(hudAxisIcon);
	//Replace with scene viewer function when available
	QObject::connect(m_showHideHUDAxisAction, &QAction::toggled, this, &GlyphEdViewerWindow::OnShowHideHUDAxis);
	m_interactionToolbar->addAction(m_showHideHUDAxisAction);
	m_loadedVisualizationDependentActions.push_back(m_showHideHUDAxisAction);

	m_showHideSceneAxisAction = new QAction(tr("Show/Hide Scene Axis"), m_interactionToolbar);
	m_showHideSceneAxisAction->setCheckable(true);
	QIcon sceneAxisIcon;
	sceneAxisIcon.addFile(":SGXGUI/Resources/Icons/icon-axis-scene.png", QSize(), QIcon::Normal, QIcon::Off);
	sceneAxisIcon.addFile(":SGXGUI/Resources/Icons/icon-axis-scene-a.png", QSize(), QIcon::Normal, QIcon::On);
	m_showHideSceneAxisAction->setIcon(sceneAxisIcon);
	//Replace with scene viewer function when available
	QObject::connect(m_showHideSceneAxisAction, &QAction::toggled, this, &GlyphEdViewerWindow::OnShowHideSceneAxis);
	m_interactionToolbar->addAction(m_showHideSceneAxisAction);
	m_loadedVisualizationDependentActions.push_back(m_showHideSceneAxisAction);

	m_interactionToolbar->addSeparator();

	m_interactionToolbar->addAction( m_resetCameraToDefaultPosition );

	m_enableDisableFlyToObjectAction = new QAction( tr( "Enable/Disable Fly-to-Object" ), m_interactionToolbar );
	m_enableDisableFlyToObjectAction->setCheckable( true );
	QIcon flyToObjectIcon;
	flyToObjectIcon.addFile( ":SGXGUI/Resources/Icons/icon-flyby.png", QSize(), QIcon::Normal, QIcon::Off );
	flyToObjectIcon.addFile( ":SGXGUI/Resources/Icons/icon-flyby-a.png", QSize(), QIcon::Normal, QIcon::On );
	m_enableDisableFlyToObjectAction->setIcon( flyToObjectIcon );
	QObject::connect( m_enableDisableFlyToObjectAction, &QAction::toggled, this, &GlyphEdViewerWindow::OnEnableDisableFlyToObjectAction );
	m_interactionToolbar->addAction( m_enableDisableFlyToObjectAction );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableFlyToObjectAction );

	m_enableDisableFreeSelectionCameraAction = new QAction( tr( "Enable/Disable Orbit Selection" ), m_interactionToolbar );
	m_enableDisableFreeSelectionCameraAction->setCheckable( true );
	m_enableDisableFreeSelectionCameraAction->setChecked( true );
	QIcon freeSelCamIcon;
	freeSelCamIcon.addFile( ":SGXGUI/Resources/Icons/icon-orbit.png", QSize(), QIcon::Normal, QIcon::Off );
	freeSelCamIcon.addFile( ":SGXGUI/Resources/Icons/icon-orbit-a.png", QSize(), QIcon::Normal, QIcon::On );
	m_enableDisableFreeSelectionCameraAction->setIcon( freeSelCamIcon );
	QObject::connect( m_enableDisableFreeSelectionCameraAction, &QAction::toggled, this, &GlyphEdViewerWindow::OnEnableDisableFreeSelectionCamera );
	m_interactionToolbar->addAction( m_enableDisableFreeSelectionCameraAction );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableFreeSelectionCameraAction );

	m_enableDisableSelEffectAction = new QAction( tr( "Enable/Disable Selection Effect" ), m_interactionToolbar );
	m_enableDisableSelEffectAction->setCheckable( true );
	m_enableDisableSelEffectAction->setChecked( true );
	QIcon selEffIcon;
	selEffIcon.addFile( ":SGXGUI/Resources/Icons/icon-blink.png", QSize(), QIcon::Normal, QIcon::Off );
	selEffIcon.addFile( ":SGXGUI/Resources/Icons/icon-blink-a.png", QSize(), QIcon::Normal, QIcon::On );
	m_enableDisableSelEffectAction->setIcon( selEffIcon );
	QObject::connect( m_enableDisableSelEffectAction, &QAction::toggled, this, &GlyphEdViewerWindow::OnEnableDisableSelEffect );
	m_interactionToolbar->addAction( m_enableDisableSelEffectAction );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableSelEffectAction );

	m_enableDisableSuperimposedGlyphGadgets = new QAction( tr( "Enable/Disable Overlapping Group Indicators" ), m_interactionToolbar );
	m_enableDisableSuperimposedGlyphGadgets->setCheckable( true );
	m_enableDisableSuperimposedGlyphGadgets->setChecked( false );
	QIcon sggIcon;
	sggIcon.addFile( ":SGXGUI/Resources/Icons/icon-show-groups.png", QSize(), QIcon::Normal, QIcon::Off );
	sggIcon.addFile( ":SGXGUI/Resources/Icons/icon-show-groups-a.png", QSize(), QIcon::Normal, QIcon::On );
	m_enableDisableSuperimposedGlyphGadgets->setIcon( sggIcon );
	QObject::connect( m_enableDisableSuperimposedGlyphGadgets, &QAction::toggled, this, &GlyphEdViewerWindow::OnEnableDisableSuperimposedGadgets );
	m_interactionToolbar->addAction( m_enableDisableSuperimposedGlyphGadgets );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableSuperimposedGlyphGadgets );
}

void GlyphEdViewerWindow::OnStereoSetup(bool stereoEnabled) {

	//m_stereoAction->setChecked(stereoEnabled);
}

void GlyphEdViewerWindow::OnShowHideHUDAxis(bool show) {

	m_viewer->enableHUDAxes( show );
}

void GlyphEdViewerWindow::OnShowHideSceneAxis(bool show) {

	m_viewer->enableSceneAxes( show );
}

void GlyphEdViewerWindow::OnEnableDisableFlyToObjectAction( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
		m_viewer->enableFlyToObject( enable );
}

void GlyphEdViewerWindow::OnEnableDisableFreeSelectionCamera( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
		m_viewer->enableFreeSelectionCamera( !enable );
}

void GlyphEdViewerWindow::OnEnableDisableSelEffect( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
	{
		m_viewer->enableSelectionEffect( enable );
		m_enableDisableSelEffectActionMenu->setChecked( enable );
		//m_enableDisableSelEffectAction->setChecked( enable );
	}
}

void GlyphEdViewerWindow::OnEnableDisableSuperimposedGadgets( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
	{
		m_viewer->enableSuperimposedGlyphGadgets( enable );
	}
}

bool GlyphEdViewerWindow::DoesHelpExist() const {

	return SynGlyphX::GlyphBuilderApplication::IsGlyphEd();
}

void GlyphEdViewerWindow::UpdateFilenameWindowTitle(const QString& title) {

	QString prefix;
	if (title != s_noFileName) {

		prefix = title + " - ";
	}

	setWindowTitle(prefix + GetApplicationDisplayName());
}

QString GlyphEdViewerWindow::GetApplicationDisplayName() const {

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {
		QString userName = QProcessEnvironment::systemEnvironment().value("AppStream_UserName", "_");
		QString sessId = QProcessEnvironment::systemEnvironment().value("AppStream_Session_ID", "_");
		return SynGlyphX::Application::applicationName() + " (SynGlyphX Inside) " + SynGlyphX::getFullVersionString().c_str() + " | " + userName + " | " + sessId + " |";
	}
	else {

		return SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
	}
}

void GlyphEdViewerWindow::ToggleInteractiveLegend()
{
	
	if (m_interactiveLegendDock->isVisible())
	{
		m_interactiveLegendDock->hide();
	}
	else
	{
		m_interactiveLegend->setPrimaryViewer(m_viewer);
		addDockWidget(Qt::DockWidgetArea::NoDockWidgetArea, m_interactiveLegendDock);
		m_interactiveLegendDock->resize(m_interactiveLegend->size());
		m_interactiveLegendDock->show();
	}
}
