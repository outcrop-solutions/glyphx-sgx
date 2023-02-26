#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QStackedWidget>
//#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QGroupBox>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QSettings>
#include <QtCore/QDirIterator>
#include <QtWidgets/QDockWidget>
#include <QtCore/QDateTime>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QColorDialog>
#include "glyphbuilderapplication.h"
#include "frontendfilterlistwidget.h"
#include "datatransformmapping.h"
#include "downloadoptionsdialog.h"
#include "data/npmapfile.h"
//#include "glyphviewerantztransformer.h"
#include "changedatasourcefiledialog.h"
#include "singlewidgetdialog.h"
#include "optionswidget.h"
#include "userdefinedbaseimageproperties.h"
#include "changeimagefiledialog.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include "filesystem.h"
#include "defaultbaseimageproperties.h"
#include "downloadexception.h"
#include "HomePageWidget.h"
#include "remapdialog.h"
#include <SceneViewer/sceneviewer.h>
#include "baseimage.h"
#include "sharedactionlist.h"
#include "GVGlobal.h"
#include "application.h"
#include <boost/uuid/uuid_io.hpp>
#include "LoadingFilterDialog.h"
#include "GlyphForestInfoModel.h"
#include "Profiler.h"
#include "networkdownloader.h"
#include "InteractiveLegend.h"
#include <hal/hal.h>
#include <QtWidgets/QDialogButtonBox>
#include "FieldProperties.h"
#include "version.h"
#include "filesystem.h"
#include <QtWebEngineCore/QWebEngineProfile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QUuid>
#include "DownloadManager.h"
#include "baseimage.h"
#include <chrono>
#include <vector>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include <QtWidgets/QTableWidget>
#include "waitingspinnerwidget.h"
#include <QTimer>
#include "AwsLogger.h"
#include <QHeaderView>
#include <QLibraryInfo>

SynGlyphX::SettingsStoredFileList GlyphViewerWindow::s_subsetFileList("subsetFileList");
QMap<QString, MultiTableDistinctValueFilteringParameters> GlyphViewerWindow::s_recentFilters;

GlyphViewerWindow::GlyphViewerWindow(QString address, QString model, QWidget *parent)
	: SynGlyphX::MainWindow(4, parent),
	m_showErrorFromTransform(true),
	m_showHomePage(true),
    m_viewer( nullptr )
{
	SGX_PROFILE_SCOPE
	//m_dataEngineConnection = std::make_shared<DataEngine::DataEngineConnection>();
	m_mappingModel = new SynGlyphX::DataTransformModel(this);
	//m_mappingModel->SetDataEngineConnection(m_dataEngineConnection);
	m_sourceDataCache = std::make_shared<SourceDataCache>();
	m_glyphForestModel = new SynGlyphX::GlyphForestInfoModel(this);
	lastModified = 0;

	m_glyphForestSelectionModel = new SynGlyphX::ItemFocusSelectionModel(m_glyphForestModel, this);
	m_filteringManager = new FilteringManager(m_mappingModel, m_sourceDataCache, m_glyphForestSelectionModel, this);

	m_columnsModel = new SourceDataInfoModel(m_mappingModel->GetDataMapping(), m_sourceDataCache, this);
	m_columnsModel->SetSelectable(false, false, true);

	m_fileToolbar = addToolBar(tr("File"));
	m_fileToolbar->setFloatable(true);
	m_fileToolbar->setMovable(true);
	m_fileToolbar->setIconSize(SynGlyphX::Application::DynamicQSize(42, 32));

	m_showHideToolbar = addToolBar(tr("Show/Hide Widgets"));
	m_showHideToolbar->setObjectName("main_toolbar");
	m_showHideToolbar->setFloatable(true);
	m_showHideToolbar->setMovable(true);

	CreateMenus();
	CreateDockWidgets();

	QStackedWidget* centerWidgetsContainer = new QStackedWidget(this);
	centerWidgetsContainer->setContentsMargins(0, 0, 0, 0);
	centerWidgetsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setCentralWidget(centerWidgetsContainer);

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd())
		NetworkDownloader::Instance().SetKeySettingName("MapQuestOpenSettings_GlyphEd");

	/*try {

		if (!m_dataEngineConnection->hasJVM()) {

			m_dataEngineConnection->createJVM();
			de_version = m_dataEngineConnection->VersionNumber();
			m_dataEngineConnection->UserAccessControls()->SetAppVersionNumber(SynGlyphX::getAppVersionString());
			m_dataEngineConnection->SetGlyphEdPath(QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/Content/"));
		}
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("JVM Error"), tr(e.what()));
		return;
	}*/

	//"C:/Users/bryan/OneDrive/Documents/GitHub/sgx/bin/Win64/Debug";
	//QLibraryInfo::location(QLibraryInfo::LibraryExecutablesPath)
	CreateLoadingScreen();
	
	try {
		
		CreateSceneViewer();
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

	QObject::connect(m_viewer, &SynGlyphX::SceneViewer::closeVisualization, this, &GlyphViewerWindow::CloseVisualization);
	QObject::connect(m_viewer, &SynGlyphX::SceneViewer::interactiveLegendToggled, this, &GlyphViewerWindow::ToggleInteractiveLegend);
	QObject::connect(m_viewer, &SynGlyphX::SceneViewer::saveSnapshot, this, &GlyphViewerWindow::SaveSnapshot);

	m_linkedWidgetsManager = new LinkedWidgetsManager(m_viewer, this);
	//m_filteringWidget->SetupLinkedWidgets(*m_linkedWidgetsManager);
	m_pseudoTimeFilterWidget->SetupLinkedWidgets(*m_linkedWidgetsManager);

	CreateInteractionToolbar();

	centerWidgetsContainer->setCurrentIndex(1);

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);

	EnableLoadedVisualizationDependentActions( false );

	UpdateFilenameWindowTitle(s_noFileName);

	m_fileToolbar->hide();
	m_showHideToolbar->hide();
	m_showHideToolbar->setVisible(false);
	m_interactionToolbar->hide();
	m_interactionToolbar->setVisible(false);
	menuBar()->hide();

	int ws_port = 12345;
	l_server = new LocalServer(this);
	l_server->startServer("WebChannel Filtering Server", ws_port);
	Core* core = l_server->WebChannelCore();
	CreateGlyphDrawer();
	core->SetDrawerWidget(glyphDrawer);
	core->SetViewerWidget(m_viewer);
	drawerDock = new QDockWidget(glyphDrawer);
	glyphDrawer->addDockWidget(Qt::BottomDockWidgetArea, drawerDock);
	drawerDock->hide();
	drawerDockHeightSet = false;
	core->SetDockWidget(drawerDock);

	QObject::connect(core, &Core::OP, this, &GlyphViewerWindow::LoadProjectIntoGlyphDrawer);
	QObject::connect(core, &Core::UF, this, &GlyphViewerWindow::UpdateGlyphDrawerFilter);
	QObject::connect(core, &Core::CS, this, &GlyphViewerWindow::ChangeModelState);
	QObject::connect(core, &Core::RD, this, &GlyphViewerWindow::ReloadGlyphDrawer);
	QObject::connect(core, &Core::SN, this, &GlyphViewerWindow::SendSdtName);
	QObject::connect(core, &Core::CM, this, &GlyphViewerWindow::CloseModel);
	QObject::connect(core, &Core::Settings, this, &GlyphViewerWindow::LoadSettings);


	dlg->load(QUrl(address));

	/*QObject::connect(&m_webSocket, &QWebSocket::connected, this, &GlyphViewerWindow::OnSocketConnect);
	QObject::connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [=](QAbstractSocket::SocketError error) {
		QMessageBox::information(this, tr("Server message"), QString(error));
	});
	m_webSocket.open(QUrl("wss://ggi3cm7i62.execute-api.us-east-1.amazonaws.com/production"));*/

	counter = 1;

	//GlyphEngine::Engine* glyphEngine = new GlyphEngine::Engine();
	//glyphEngine->initiate();

	/*if (model != nullptr) {
		QMessageBox::information(this, tr("Model Url"), model);
	}*/

	QSettings qsettings;
	qsettings.beginGroup("GlyphX_User");
	QString userID = qsettings.value("userid", "123456").toString();
	summation = true;
	explosion = false;
	AwsLogger::getInstance()->setCommonDataPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation());
	AwsLogger::getInstance()->logger(userID, "New session.");
	AwsLogger::getInstance()->localLogger("New session.");
	AwsLogger::getInstance()->localLogger(QLibraryInfo::location(QLibraryInfo::LibraryExecutablesPath));

}

GlyphViewerWindow::~GlyphViewerWindow()
{

}

void GlyphViewerWindow::closeJVM(){
	
	//m_dataEngineConnection->destroyJVM();
}
/*
void GlyphViewerWindow::OnSocketConnect() {

	QObject::connect(&m_webSocket, &QWebSocket::textMessageReceived,
		this, &GlyphViewerWindow::OnSocketLaunch);
	m_webSocket.sendTextMessage(QStringLiteral("{\"action\" : \"OnMessage\" , \"connectionId\": \"\", \"message\" : \"\"}"));
}

void GlyphViewerWindow::OnSocketSslErrors(const QList<QSslError> &errors) {
	for (int i = 0; i < errors.size(); i++) {
		QMessageBox::information(this, tr("Server message"), errors[i].errorString());
	}
}

void GlyphViewerWindow::OnSocketLaunch(QString message) {

	QMap<QString, QString> jsonMap;
	QString json = message;
	QString type = message.split(',')[0].split(":")[1].remove('"');
	jsonMap.insert("type", type);
	QString m = message.split("\"message\":\"")[1];
	m.chop(2);
	jsonMap.insert("message", m);

	if (jsonMap["type"] == "id") {
		//QString address = "http://sgxprototype.s3-website-us-east-1.amazonaws.com/?server=" + jsonMap["message"];

		QString address = "http://localhost:3000/";
		dlg->load(QUrl(address));
	}
	else if (jsonMap["type"] == "code") {

		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		QMap<QString, QString> jMap;
		QString j = jsonMap["message"];
		j.remove(QChar('{'));
		j.remove(QChar('}'));
		j.remove(QChar('"'));
		QStringList jsonList = j.split(QChar(','));
		for (int i = 0; i < jsonList.size(); i++) {
			int index = jsonList.at(i).indexOf(':');
			QString key = jsonList.at(i).mid(0, index);
			QString value = jsonList.at(i).mid(index + 1);
			jMap.insert(key, value);
		}

		QString filename = jsonMap["message"].split('/')[5].split('?')[0];
		QString location = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/Content/");
		QDir loc(QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation())));
		loc.mkdir("Content");
		
		QFile dir(location + filename.split(".zip")[0]);
		lastModified = 0;

		if (!dir.exists()) {
			DownloadManager downloadManager(m_dataEngineConnection);
			downloadManager.DownloadFile(jMap["URL"], location + filename);
		}
		else {
			QFileInfo info(dir);

			qint64 lastMod = jMap["LastModified"].toLongLong();
			lastModified = info.lastModified().toSecsSinceEpoch();

			if ((lastMod - lastModified) > 0) {
				QDir d(location + filename.split(".zip")[0]);
				d.removeRecursively();
				DownloadManager downloadManager(m_dataEngineConnection);
				downloadManager.DownloadFile(jMap["URL"], location + filename);
			}
				
		}

		QString m_currentFilename = findSdtInDirectory(location + filename.split(".zip")[0]);

		if (m_currentFilename != "None") {
			LoadNewVisualization(m_currentFilename);

		}

		SynGlyphX::Application::restoreOverrideCursor();

	}
	else {
		//QMessageBox::information(this, tr("Server message"), jsonMap["type"] + ", " + jsonMap["message"]);
	}

}

void GlyphViewerWindow::OnSocketClosed() {

	QMessageBox::information(this, tr("Server message"), "Socket closed.");
}
*/
void GlyphViewerWindow::CreateGlyphDrawer() {

	glyphDrawer = new QMainWindow(this);

	//QRect rec = QApplication::desktop()->screenGeometry();

	//glyphDrawer->setMinimumSize(QSize(1793, rec.height()-200));
	//glyphDrawer->move(511, 200);

	glyphDrawer->setWindowFlags(Qt::FramelessWindowHint);

	//QVBoxLayout* layout = new QVBoxLayout(this);
	QVBoxLayout* layout = new QVBoxLayout(this);

	/*QLabel* syncLabel = new QLabel(this);
	syncLabel->setAlignment(Qt::AlignCenter);
	syncLabel->setWordWrap(true);
	syncLabel->setTextFormat(Qt::RichText);
	syncLabel->setStyleSheet("QLabel{font-size: 12.5pt; font-weight: bold;}");
	syncLabel->setText(tr("GlyphX.sdt"));*/

	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(0);
	layout->addWidget(m_viewer);
	glyphDrawer->setCentralWidget(m_viewer);

	glyphDrawer->setStyleSheet("QWidget{background-color: #0f172a;}");

	glyphDrawer->setLayout(layout);

	glyphDrawer->hide();

	/*int x = 512;
	int y = 512;
	int w = 1785;
	int h = 825;

	glyphDrawer->setMinimumSize(QSize(w, h));
	glyphDrawer->move(x, y);*/

	QDir loc(QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation())));
	loc.mkdir("Content");

}

void GlyphViewerWindow::LoadProjectIntoGlyphDrawer(QString text, bool load_from_cache) {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	drawerDock->hide();

	try {

		AwsLogger::getInstance()->localLogger(text);

		QString location = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/Content/");
			
		//text = "{\"user_id\":\"1234-1234-1234\", \"model_id\":\"e8d3c870-d2c3-4ebb-bd92-22f61f560413\"}";
		QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
		QJsonObject obj = doc.object();
		QString userId = obj.value("user_id").toString();
		QString modelId = obj.value("model_id").toString();

		AwsLogger::getInstance()->logger(userID, text);

		athenaTableName = modelId;

		if (!QDir(location + "/" + modelId).exists()) {
			QDir().mkdir(location + "/" + modelId);
		}

		DownloadManager downloadManager;
	
		QString sdt = "https://glyphx-model-output-bucket.s3.amazonaws.com/" + userId + "/" + modelId + "/model.sdt";

		//***Get SDT file, download it into Content, use it to configure the model***
		downloadManager.DownloadFile(QUrl(sdt), location + "/" + modelId + "/model.sdt");

		glyphDrawer->show();
		m_viewer->show();

		m_mappingModel->LoadDataTransformFile(location + "/" + modelId + "/model.sdt");
		AwsLogger::getInstance()->localLogger("Data Transform file loaded");
		boost::uuids::uuid uuid = m_mappingModel->GetDataMapping()->GetID();

		std::string dcd = GlyphViewerOptions::GetDefaultCacheDirectory().toStdString();
		std::string cacheDirectoryPath = dcd + ("/cache_" + boost::uuids::to_string(uuid) + "/");

		cache_location = QString::fromStdString(cacheDirectoryPath);

		QStringList legend_list;
		m_viewer->setupLegendWindow(cache_location, legend_list);

		if (!QFile(cache_location + "scene/bytes.sgc").exists()) {
			QDir().mkdir(cache_location);
			QDir().mkdir(cache_location + "/scene");
		}

		QString sgc = "https://glyphx-model-output-bucket.s3.amazonaws.com/" + userId + "/" + modelId + "/bytes.sgc";
		QString sgn = "https://glyphx-model-output-bucket.s3.amazonaws.com/" + userId + "/" + modelId + "/bytes.sgn";
			
		//***Download bytes.sgc and bytes.sgn into cache directory***
		downloadManager.DownloadFile(QUrl(sgc), cache_location + "scene/bytes.sgc");
		downloadManager.DownloadFile(QUrl(sgn), cache_location + "scene/bytes.sgn");

		std::vector<std::string> base_images;
		QDir scene(cache_location + "scene/");

		QFileInfoList files = scene.entryInfoList();

		for (QFileInfo file : files) {
			QString base = file.baseName();
			if (base.contains("base_image")) {
				base_images.push_back(base.toStdString());
			}
		}
		QString summ = summation ? "true" : "false";
		QString expl = explosion ? "true" : "false";
		//out << "summation: " << summ << endl;
		//out << "explosion: " << expl << endl;
		m_viewer->setGroupSettings(summation, explosion);
		AwsLogger::getInstance()->localLogger(cache_location);
		m_viewer->loadScene((cache_location + "scene/bytes.sgc").toStdString().c_str(), (cache_location + "scene/bytes.sgn").toStdString().c_str(), base_images);
		m_viewer->setFilteredGlyphOpacity(0.0f);
		AwsLogger::getInstance()->localLogger("Finshed loading from cache.");

		AwsLogger::getInstance()->localLogger("About to load files into model");
		LoadFilesIntoModel();
		AwsLogger::getInstance()->localLogger("Loaded files into model");

	}
	catch (std::exception& e) {
		QMessageBox::information(this, tr("Error"), e.what());
		//QMessageBox::information(this, tr("Error message"), "Failed to load model.\n" + text);
	}

	SynGlyphX::Application::restoreOverrideCursor();

	l_server->WebChannelCore()->GetDrawerPosition();

	HitAthenaAPI(QList<int>{6}, true);
}

void GlyphViewerWindow::UpdateGlyphDrawerFilter(QString text) {
	
	try {
		QString url = "https://q8gv8df1a2.execute-api.us-east-2.amazonaws.com/prod/etl/get-filtering-ids";
		QNetworkRequest request(url);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		QJsonDocument filter_doc = QJsonDocument::fromJson(text.toUtf8());
		QJsonObject filter_obj = filter_doc.object();
		QString query = filter_obj.value("filter").toString();

		QJsonObject obj;
		obj["tableName"] = athenaTableName; //"01388c0b_5f5f_49ff_8c15_b6e902604b14";
		obj["query"] = query;
		QJsonDocument doc(obj);
		QByteArray data = doc.toJson();

		QNetworkAccessManager networkManager;
		QNetworkReply *reply = networkManager.post(request, data);

		QEventLoop loop;
		QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

		if (reply->isRunning()) {
			loop.exec();
		}
		reply->deleteLater();

		if (reply->error() != QNetworkReply::NoError) {
			AwsLogger::getInstance()->localLogger(reply->errorString());
			throw DownloadException(("Network Error: " + reply->errorString() + "\n_" + url + "_").toStdString().c_str());
		}

		QJsonDocument reply_doc = QJsonDocument::fromJson(reply->readAll());
		QJsonArray ids = reply_doc.array();
		AwsLogger::getInstance()->localLogger("Filter returned " + QString::number(ids.size()) + " ids");

		SynGlyphX::IndexSet myset;
		for (int i = 0; i < ids.size(); i++) {
			myset.insert(ids.at(i).toInt());
		}
		m_viewer->setFilteredResults(myset, true);

	}
	catch (const std::exception& e) {

		QMessageBox::information(this, tr("Error message"), "Failed to update filter. \n" + QString::fromStdString(e.what()));
	}
	
}

void GlyphViewerWindow::ChangeModelState(QString text) {

	try {
		QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
		QJsonObject obj = doc.object();
		QString camera = obj.value("camera").toString();
		QString query = obj.value("query").toString();

		QStringList nums = camera.remove("[").remove("]").split(",");
		std::vector<double> pos;
		for (QString num : nums) {
			pos.push_back(num.toDouble());
		}
		m_viewer->setCameraPosition(pos);

		UpdateGlyphDrawerFilter(query);
	}
	catch (...) {
		QMessageBox::information(this, tr("Error message"), "Failed to change model state.");
	}
}

void GlyphViewerWindow::SendSdtName(QString text) {

	l_server->WebChannelCore()->SendSdtName(m_currentSdtName);
}

void GlyphViewerWindow::CloseModel() {

	glyphDrawer->hide();
	ClearAllData();
	ClearCurrentFile();
	m_viewer->clearScene();
}

void GlyphViewerWindow::ReloadGlyphDrawer(QString text) {

}

QString GlyphViewerWindow::findSdtInDirectory(const QString& directory) {

	QDirIterator it(directory, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QFile f(it.next());
		if (f.fileName().endsWith(".sdt")) {
			return f.fileName();
		}
	}
	return "None";
}

void GlyphViewerWindow::SaveSnapshot() {

	std::vector<float> pos = m_viewer->getCameraPosition();
	QString cameraJson = jsonFromCamera(pos);

	//QMessageBox::information(this, tr("Server message"), cameraJson);
	
	QList<QString> tables = m_filteringManager->GetTable2FiltersMap().keys();
	QString json = "\"tables\": {";
	for (int i = 0; i < tables.size(); i++) {
		json += jsonFromFilterTable(tables[i]);
		if (i != tables.size() - 1)
			json += ",";
	}
	json += "}";

	QString snapshot = "{" + cameraJson + "," + json + "}";
	QFile file("snapshot.json");
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		out << snapshot;
		file.close();
	}
	//QMessageBox::information(this, tr("Server message"), snapshot);
	//std::vector<float> position{-289.296, -263.176, 285.157, 0.623141, 0.566878, -0.538836};
}

QString GlyphViewerWindow::jsonFromCamera(std::vector<float> pos) {

	QString json = "\"camera\": {\"position\": [" + QString::number(pos[0]) + "," + QString::number(pos[1]) + "," + QString::number(pos[2]) + "]," +
		"\"direction\": [" + QString::number(pos[3]) + "," + QString::number(pos[4]) + "," + QString::number(pos[5]) + "]}";
	return json;
}

QString GlyphViewerWindow::jsonFromFilterTable(QString table) {

	QString json = "\"" + table + "\": {";
	bool hf = m_filteringManager->GetTable2FiltersMap()[table].HasFilters();
	if (hf) {
		std::vector<std::pair<QString, QSet<QString>>> dvf = m_filteringManager->GetTable2FiltersMap()[table].GetDistinctValueFilters();
		for (int i = 0; i < dvf.size(); i++) {
			QList<QString> values = dvf[i].second.values();
			json += jsonFromFilterField(dvf[i].first, values);
			if (i != dvf.size() - 1)
				json += ",";
		}
	}
	json += "}";
	return json;
}

QString GlyphViewerWindow::jsonFromFilterField(QString field, QList<QString> values) {

	QString json = "\"" + field + "\": [";
	for (int i = 0; i < values.size(); i++) {
		json += "\"" + values[i] + "\"";
		if (i != values.size() - 1)
			json += ",";
	}
	json += "]";
	return json;
}

void GlyphViewerWindow::CreateLoadingScreen() {

	QStackedWidget* centerWidgetsContainer = dynamic_cast<QStackedWidget*>(centralWidget());

	dlg = new QWebEngineView(this);
	auto wep = dlg->page()->profile();
	wep->setHttpAcceptLanguage("en-US,en;q=0.9");
	centerWidgetsContainer->addWidget(dlg);

}

void GlyphViewerWindow::CreateSceneViewer() {
	
	QStackedWidget* antzWidgetContainer = dynamic_cast<QStackedWidget*>(centralWidget());
	if (m_viewer) {
		
		antzWidgetContainer->removeWidget(m_viewer);
		delete m_viewer;
		m_viewer = nullptr;
	}

	m_viewer = new SynGlyphX::SceneViewer( this, SynGlyphX::ViewerMode::Full );
	m_viewer->setSelectionModel( m_glyphForestModel, m_glyphForestSelectionModel );
	//if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd())
		//m_viewer->setLogoFile("textures/glyphed.png");

	m_openURLAction = new QAction(tr("Open URL"), this);
	m_openURLAction->setShortcut(Qt::Key_U);
	m_openURLAction->setEnabled(false);
	QObject::connect(m_openURLAction, &QAction::triggered, this, &GlyphViewerWindow::OnOpenURLs);

	m_propertiesAction = new QAction(tr("Properties"), this);
	m_propertiesAction->setEnabled(false);
	QObject::connect(m_propertiesAction, &QAction::triggered, this, &GlyphViewerWindow::OnPropertiesActivated);

	m_enableDisableSelEffectActionMenu = new QAction( tr( "Enable/Disable Selection Effect" ), this );
	m_enableDisableSelEffectActionMenu->setCheckable( true );
	m_enableDisableSelEffectActionMenu->setChecked( true );
	m_enableDisableSelEffectActionMenu->setEnabled( false );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableSelEffectActionMenu );
	QObject::connect( m_enableDisableSelEffectActionMenu, &QAction::toggled, this, &GlyphViewerWindow::OnEnableDisableSelEffect );

	m_viewer->addAction(m_openURLAction);
	m_viewer->addAction(m_propertiesAction);
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
	
	QObject::connect( m_resetCameraToDefaultPosition, &QAction::triggered, this, [this]{ m_viewer->resetCamera(); } );
	QObject::connect(m_showAnimation, &QAction::toggled, this, [this]{ m_viewer->enableAnimation( !m_viewer->animationEnabled() ); });
	QObject::connect(m_showTagsAction, &QAction::triggered, this, [this]{ m_viewer->showTagsOfSelectedObjects(true); });
	QObject::connect(m_hideTagsAction, &QAction::triggered, this, [this]{ m_viewer->showTagsOfSelectedObjects(false); });
	QObject::connect( m_hideAllTagsAction, &QAction::triggered, this, [this]{ m_viewer->hideAllTags(); });

	m_viewer->setOnSelectionChanged( [this]( bool selection_exists )
	{
		m_clearSelectionAction->setEnabled( selection_exists );
		m_showTagsAction->setEnabled( selection_exists );
		m_hideTagsAction->setEnabled( selection_exists );

		m_openURLAction->setEnabled( selection_exists );
		m_propertiesAction->setEnabled( selection_exists );
	} );

	m_filteringManager->SetViewer(m_viewer);
}

void GlyphViewerWindow::CreateMenus() {

	//Create File Menu
	m_fileMenu = menuBar()->addMenu(tr("File"));

	QAction* openVisAction = CreateMenuAction(m_fileMenu, tr("Open Visualization"), QKeySequence::Open);
	QObject::connect(openVisAction, &QAction::triggered, this, &GlyphViewerWindow::OpenVisualisation);
	
	QAction* refreshVisualizationAction = nullptr;
	if (!SynGlyphX::GlyphBuilderApplication::IsGlyphEd())
	{
		refreshVisualizationAction = CreateMenuAction(m_fileMenu, tr("Refresh Visualization"), QKeySequence::Refresh);
		QIcon refreshVizIcon;
		QPixmap refresh(":SGXGUI/Resources/Icons/icon-refresh.png");
		refreshVizIcon.addPixmap(refresh.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
		refreshVisualizationAction->setIcon(refreshVizIcon);
		QObject::connect(refreshVisualizationAction, &QAction::triggered, this, &GlyphViewerWindow::RefreshVisualization);
		m_loadedVisualizationDependentActions.push_back(refreshVisualizationAction);
	}

	m_fileMenu->addSeparator();

	QAction* closeVisualizationAction = CreateMenuAction(m_fileMenu, tr("Close Visualization"), QKeySequence::Close);
	QIcon closeVizIcon;
	//closeVizIcon.addFile(":GlyphViewer/Resources/icon-close-viz.png", QSize(), QIcon::Normal, QIcon::Off);
	QPixmap close(":GlyphViewer/Resources/icon-close-viz.png");
	closeVizIcon.addPixmap(close.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	//closeVizIcon.addFile(":GlyphViewer/Resources/icon-close-viz.png", QSize(), QIcon::Normal, QIcon::On);
	closeVisualizationAction->setIcon(closeVizIcon);
	QObject::connect(closeVisualizationAction, &QAction::triggered, this, &GlyphViewerWindow::CloseVisualization);
	m_loadedVisualizationDependentActions.push_back(closeVisualizationAction);

	m_fileToolbar->addAction(closeVisualizationAction);
	if (refreshVisualizationAction) m_fileToolbar->addAction(refreshVisualizationAction);

	CreateExportToPortableVisualizationSubmenu();
	
	QObject::connect(m_glyphForestSelectionModel, &SynGlyphX::ItemFocusSelectionModel::selectionChanged, this, &GlyphViewerWindow::OnSelectionChanged);

	m_fileMenu->addActions(m_recentFileActions);

	m_fileMenu->addSeparator();

	QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
	QObject::connect(exitAction, &QAction::triggered, this, &GlyphViewerWindow::close);

	//Create View Menu
	CreateViewMenu();

	m_stereoAction = CreateMenuAction(m_viewMenu, tr("Stereo"));
	m_stereoAction->setCheckable(true);
	QObject::connect(m_stereoAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeStereoMode);

	m_viewMenu->addSeparator();

	m_showAnimation = m_viewMenu->addAction(tr("Show Animation"));
	m_showAnimation->setCheckable(true);
	m_showAnimation->setChecked(true);

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
	QPixmap camera(":SGXGUI/Resources/Icons/icon-reset-camera.png");
	resetCamIcon.addPixmap(camera.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_resetCameraToDefaultPosition->setIcon( resetCamIcon );
	m_loadedVisualizationDependentActions.push_back(m_resetCameraToDefaultPosition);

	m_viewMenu->addSeparator();

	m_viewMenu->addSeparator();

	m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	m_remapRootPositionMappingsAction = m_toolsMenu->addAction(tr("Change X, Y, Z Position Axes"));
	QIcon remapIcon;
	QPixmap xyz_off(":SGXGUI/Resources/Icons/icon-xyz.png");
	QPixmap xyz_on(":SGXGUI/Resources/Icons/icon-xyz-a.png");
	remapIcon.addPixmap(xyz_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	remapIcon.addPixmap(xyz_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_remapRootPositionMappingsAction->setIcon(remapIcon);
	QObject::connect(m_remapRootPositionMappingsAction, &QAction::triggered, this, &GlyphViewerWindow::RemapRootPositionMappings);
	m_loadedVisualizationDependentActions.push_back(m_remapRootPositionMappingsAction);

	m_toolsMenu->addSeparator();

	QAction* optionsAction = m_toolsMenu->addAction(tr("Options"));
	QObject::connect(optionsAction, &QAction::triggered, this, static_cast<void (GlyphViewerWindow::*)()>(&GlyphViewerWindow::ChangeOptions));

	m_toolsMenu->addSeparator();

	QAction* bcAction = m_toolsMenu->addAction(tr("Change Background Color"));
	QObject::connect(bcAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeBackgroundColor);

	m_toolsMenu->addSeparator();

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeMapDownloadSettings);

	CreateHelpMenu();

	QAction* openGLSettingsAction = new QAction(tr("OpenGL Settings"), m_helpMenu);
	QObject::connect(openGLSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ShowOpenGLSettings);

	m_helpMenu->insertAction(m_aboutBoxAction, openGLSettingsAction);
	m_helpMenu->insertSeparator(m_aboutBoxAction);

#ifdef __APPLE__
	menuBar()->addSeparator();
#endif
	SynGlyphX::MainWindow::CreateLoginMenu();

}

void GlyphViewerWindow::CreateDockWidgets() {

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	m_legendsWidget = new LegendsDisplayWidget(this);
	m_legendsDockWidget = new QDockWidget(tr("Legends"), this);
	m_legendsDockWidget->setAllowedAreas(Qt::NoDockWidgetArea);
	m_legendsDockWidget->setWidget(m_legendsWidget);
	m_legendsDockWidget->setFloating(true);
	m_showHideToolbar->setIconSize(SynGlyphX::Application::DynamicQSize(42, 32));
	QIcon legendIcon;
	QPixmap legend_off(":SGXGUI/Resources/Icons/icon-legend.png");
	QPixmap legend_on(":SGXGUI/Resources/Icons/icon-legend-a.png");
	legendIcon.addPixmap(legend_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	legendIcon.addPixmap(legend_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	QAction* act = m_legendsDockWidget->toggleViewAction();
	//act->setIconVisibleInMenu(false);
	act->setIcon(legendIcon);
	m_loadedVisualizationDependentActions.push_back(act);
	m_viewMenu->addAction(act);
	m_showHideToolbar->addAction(act);
	m_legendsDockWidget->move(100, 100);
	m_legendsDockWidget->resize(SynGlyphX::Application::DynamicQSize(400, 280));
	m_legendsDockWidget->hide();

	m_interactiveLegend = new InteractiveLegend(this, m_mappingModel->GetDataMapping());
	m_interactiveLegendDock = new QDockWidget(tr("Interactive Legend"), this);
	m_interactiveLegendDock->hide();
	m_interactiveLegendDock->setFloating(true);
	m_interactiveLegendDock->setWidget(m_interactiveLegend);
	m_interactiveLegendAction = m_interactiveLegendDock->toggleViewAction(); //m_viewMenu->addAction(tr("Show Interactive Legend"));
	QIcon intrLegendIcon;
	QPixmap intrlegend_off(":SGXGUI/Resources/Icons/icon-onoff.png");
	QPixmap intrlegend_on(":SGXGUI/Resources/Icons/icon-onoff-a.png");
	intrLegendIcon.addPixmap(intrlegend_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	intrLegendIcon.addPixmap(intrlegend_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_interactiveLegendAction->setIcon(intrLegendIcon);
	m_viewMenu->addAction(m_interactiveLegendAction);
	m_showHideToolbar->addAction(m_interactiveLegendAction);
	QObject::connect(m_interactiveLegendAction, &QAction::toggled, this, &GlyphViewerWindow::ToggleInteractiveLegend);
	m_loadedVisualizationDependentActions.push_back(m_interactiveLegendAction);

	if (!SynGlyphX::GlyphBuilderApplication::IsGlyphEd() && !SynGlyphX::GlyphBuilderApplication::AWSEnabled()) {

		m_glyphPropertiesWidgetContainer = new GlyphPropertiesWidgetsContainer(m_glyphForestModel, m_glyphForestSelectionModel, this);

		QDockWidget* textPropertiesDockWidget = new QDockWidget(tr("Text Properties"), this);
		textPropertiesDockWidget->setWidget(m_glyphPropertiesWidgetContainer->GetTextProperitesWidget());
		addDockWidget(Qt::LeftDockWidgetArea, textPropertiesDockWidget);
		act = textPropertiesDockWidget->toggleViewAction();
		m_loadedVisualizationDependentActions.push_back(act);
		QIcon textIcon;
		QPixmap text_off(":SGXGUI/Resources/Icons/icon-text.png");
		QPixmap text_on(":SGXGUI/Resources/Icons/icon-text-a.png");
		textIcon.addPixmap(text_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
		textIcon.addPixmap(text_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
		act->setIcon(textIcon);
		m_viewMenu->addAction(act);
		m_showHideToolbar->addAction(act);
		m_showHideToolbar->addAction(textPropertiesDockWidget->toggleViewAction());
		textPropertiesDockWidget->hide();
	}

	m_leftDockWidget = new QDockWidget(tr("Active Front End Filters"));
	auto sv = new QScrollArea(m_leftDockWidget);
	m_FEfilterListWidget = new FrontEndFilterListWidget(m_leftDockWidget);
	m_leftDockWidget->setWidget(sv);
	sv->setWidget(m_FEfilterListWidget);
	sv->setWidgetResizable(true);
	sv->setMinimumWidth(300);
	sv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_ToggleFEFilterListAction = m_leftDockWidget->toggleViewAction();
	addDockWidget(Qt::LeftDockWidgetArea, m_leftDockWidget);
	m_FEfilterListWidget->setVisible(true);
	sv->setVisible(true);

	m_loadedVisualizationDependentActions.push_back(m_ToggleFEFilterListAction);
	QIcon feFilterIcon;
	QPixmap fe_filter_off(":SGXGUI/Resources/Icons/icon-filter-list.png");
	QPixmap fe_filter_on(":SGXGUI/Resources/Icons/icon-filter-list-a.png");
	feFilterIcon.addPixmap(fe_filter_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	feFilterIcon.addPixmap(fe_filter_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_ToggleFEFilterListAction->setIcon(feFilterIcon);
	m_viewMenu->addAction(m_ToggleFEFilterListAction);
	m_showHideToolbar->addAction(m_ToggleFEFilterListAction);
	m_leftDockWidget->hide();

	m_rightDockWidget = new QDockWidget(tr("Filtering"), this);
	m_filteringWidget = new FilteringWidget(m_columnsModel, m_filteringManager, m_rightDockWidget);
	m_rightDockWidget->setWidget(m_filteringWidget);
	addDockWidget(Qt::RightDockWidgetArea, m_rightDockWidget);
	act = m_rightDockWidget->toggleViewAction();
	m_loadedVisualizationDependentActions.push_back(act);
	QIcon filterIcon;
	QPixmap filter_off(":SGXGUI/Resources/Icons/icon-filter.png");
	QPixmap filter_on(":SGXGUI/Resources/Icons/icon-filter-a.png");
	filterIcon.addPixmap(filter_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	filterIcon.addPixmap(filter_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	act->setIcon(filterIcon);
	m_viewMenu->addAction(act);
	m_showHideToolbar->addAction(act);
	m_rightDockWidget->hide();

	QObject::connect(m_filteringWidget, &FilteringWidget::LoadSubsetVisualization, this, [this](QString filename){ LoadNewVisualization(filename); }, Qt::QueuedConnection);

	m_bottomDockWidget = new QDockWidget(tr("Time Animated Filter"), this);
	m_pseudoTimeFilterWidget = new PseudoTimeFilterWidget(m_columnsModel, m_filteringManager, m_bottomDockWidget);
	m_bottomDockWidget->setWidget(m_pseudoTimeFilterWidget);
	addDockWidget(Qt::BottomDockWidgetArea, m_bottomDockWidget);
	act = m_bottomDockWidget->toggleViewAction();
	m_loadedVisualizationDependentActions.push_back(act);
	QIcon filterTimeIcon;
	QPixmap time_off(":SGXGUI/Resources/Icons/icon-filter-time.png");
	QPixmap time_on(":SGXGUI/Resources/Icons/icon-filter-time-a.png");
	filterTimeIcon.addPixmap(time_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	filterTimeIcon.addPixmap(time_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	act->setIcon(filterTimeIcon);
	m_viewMenu->addAction(act);
	m_showHideToolbar->addAction(act);

	QObject::connect(m_columnsModel, &SourceDataInfoModel::modelReset, m_pseudoTimeFilterWidget, &PseudoTimeFilterWidget::ResetForNewVisualization);
	m_bottomDockWidget->hide();

}

void GlyphViewerWindow::OpenProject() {
	QString openFile = GetFileNameOpenDialog("ProjectDir", tr("Open Project"), "", tr("SynGlyphX Project Files (*.xdt)"));
	//if (!openFile.isEmpty())
		//m_homePage->LoadProject(openFile);

}

void GlyphViewerWindow::OpenVisualisation() {

	if (HasValidLicense()){

		QString openFile = GetFileNameOpenDialog("VisualizationDir", tr("Open Visualization"), "", tr("SynGlyphX Visualization Files (*.sdt);;SynGlyphX Data Transform Files (*.sdt)"));
		if (!openFile.isEmpty()) {

			try {

				SynGlyphX::DataTransformMapping::SharedPtr mapping = std::make_shared<SynGlyphX::DataTransformMapping>();
				mapping->ReadFromFile(openFile.toStdString());

				ValidateDataMappingFile(mapping, openFile);

				MultiTableDistinctValueFilteringParameters filters;

				//LoadNewVisualization(openFile);
				LoadNewVisualization(openFile, filters, filters.size() > 0);
			}
			catch (const std::exception& e) {

				QMessageBox::critical(this, tr("Visualization failed to load"), tr("Visualization failed to load: ") + e.what());
			}
		}
	}
	else
	{	
		//QString contact = SynGlyphX::GlyphBuilderApplication::IsGlyphEd() ? "<a href=\"mailto:mark@GlyphEd.co\">Mark@GlyphEd.co</a>" : "<a href=\"mailto:mark@synglyphx.com\">Mark@SynGlyphX.com</a>";
		QString contact = "<a href=\"http://www.synglyphx.com\">SynGlyphX</a>";
		QMessageBox::information(this, tr("No Active User"),
			QString::fromStdWString(L"<p>This application requires an active user account to perform this action. Please log in to continue.</p>"
			L"<p>To obtain an account, or to renew your license please contact " + contact.toStdWString() + L".</p>"),
			QMessageBox::Ok);
	}
}

bool GlyphViewerWindow::DoesVisualizationNeedToBeRecreated(const SynGlyphX::DataTransformMapping& mapping) const {

	if ((m_mappingModel->GetDataMapping()->operator!=(mapping))) {

		return true;
	}

	if (m_sourceDataCache->IsCacheOutOfDate(m_mappingModel->GetDataMapping()->GetDatasourcesInUse())) {

		return true;
	}

	QDir cacheDir(QString::fromStdWString(m_cacheManager.GetCacheDirectory(m_mappingModel->GetDataMapping()->GetID())));
	QDateTime lastUpdateTimeForCache = cacheDir.entryInfoList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files).first().lastModified();
	for (const auto& baseObject : m_mappingModel->GetDataMapping()->GetBaseObjects()) {

		SynGlyphX::UserDefinedBaseImageProperties::ConstSharedPtr properties = std::dynamic_pointer_cast<const SynGlyphX::UserDefinedBaseImageProperties>(baseObject.GetProperties());
		if (properties) {

			if (QFileInfo(QString::fromStdWString(properties->GetFilename())).lastModified() > lastUpdateTimeForCache) {

				return true;
			}
		}
	}

	return false;
}

void GlyphViewerWindow::RefreshVisualization() {

	try {
		
		SynGlyphX::DataTransformMapping mapping;
		mapping.ReadFromFile(m_currentFilename.toStdString());
		if (DoesVisualizationNeedToBeRecreated(mapping)) {

			ClearAllData();
			LoadVisualization(m_currentFilename);
		}
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("Failed To Refresh Visualization"), tr("Failed to refresh visualization.  Error: ") + e.what(), QMessageBox::Ok);
		return;
	}

	statusBar()->showMessage("Visualization successfully refreshed", 3000);
}

void GlyphViewerWindow::CloseVisualization() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	if (!glyphDrawer->isHidden()) {
		glyphDrawer->hide();
	}
	/*
	m_pseudoTimeFilterWidget->Disable();
	ClearAllData();
	EnableLoadedVisualizationDependentActions(false);
	ClearCurrentFile();
	if (m_legendsDockWidget->isFloating()) {

		m_legendsDockWidget->hide();
	}
	if (m_leftDockWidget->isVisible()) {

		m_leftDockWidget->hide();
	}
	if (m_rightDockWidget->isVisible()){

		m_rightDockWidget->hide();
	}
	if (m_bottomDockWidget->isVisible()){

		m_bottomDockWidget->hide();
	}

	QStackedWidget* centerWidgetsContainer = dynamic_cast<QStackedWidget*>(centralWidget());
	centerWidgetsContainer->setCurrentIndex(0);
	m_viewer->setBackgroundColor( SynGlyphX::render::color::black() );
	m_viewer->clearScene();
	if (m_showHomePage || SynGlyphX::GlyphBuilderApplication::IsGlyphEd())
		centerWidgetsContainer->setCurrentIndex(0);
	*/
	SynGlyphX::Application::restoreOverrideCursor();
}

bool GlyphViewerWindow::IsUserLoggedIn() {

	/*if (!SynGlyphX::GlyphBuilderApplication::IsGlyphEd() && !SynGlyphX::GlyphBuilderApplication::AWSEnabled()) {
		return true;
	}

	QSettings settings;
	settings.beginGroup("LoggedInUser");
	QString user = settings.value("Username", "Guest").toString();
	QString pass = settings.value("Password", "").toString();
	QString name = settings.value("Name", "Guest").toString();
	QString inst = settings.value("Institution", "").toString();
	bool logged = settings.value("StayLogged", false).toBool();
	settings.endGroup();

	if (m_dataEngineConnection->UserAccessControls()->IsValidConnection()){
		return true;
	}
	else{
		//m_dataEngineConnection->UserAccessControls()->InitializeConnection();
		if (logged){
			int valid = m_dataEngineConnection->UserAccessControls()->ValidateCredentials(user, pass);
			if (valid == 1 || valid == 2){
				//m_dataEngineConnection->UserAccessControls()->PresetLogoPath(m_dataEngineConnection->GetGlyphEdPath() + inst);
				//MainWindow::UpdateUserMenu(name);
				//UpdateUserMenu();
				return true;
			}
		}
		return false;
	}*/
	return true;
}

void GlyphViewerWindow::UpdateUserMenu(){
	QObject::connect(MainWindow::LogoutMenu(), &QAction::triggered, this, &GlyphViewerWindow::Logout);
}

void GlyphViewerWindow::Logout(){
	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	MainWindow::UserLogOut();
	if (MainWindow::HasOpenFile()){
		CloseVisualization();
	}

	QSettings settings;
	settings.beginGroup("LoggedInUser");
	settings.setValue("Username", "Guest");
	settings.setValue("Password", "");
	settings.setValue("Name", "Guest");
	settings.setValue("Institution", "");
	settings.setValue("StayLogged", false);
	settings.endGroup();

	if (MainWindow::HasValidLicense()){
		m_groupsComboBox->clear();
	}

	//m_dataEngineConnection->UserAccessControls()->ResetConnection();

	//m_homePage->ResetViews();
	//m_homePage->LoggedOut();
	SynGlyphX::Application::restoreOverrideCursor();
}

void GlyphViewerWindow::CreateUserSettingsDialog(QStringList groups) {

	/*QSettings settings;
	settings.beginGroup(QString::number(m_dataEngineConnection->UserAccessControls()->GetUserID()));
	QString groupName = settings.value("GroupName", "Default").toString();
	bool onStartupChecked = settings.value("OnStartupChecked", false).toBool();
	bool reqOnStartupChecked = settings.value("ReqOnStartupChecked", true).toBool();
	settings.endGroup();*/
	QString groupName = "Default";
	bool onStartupChecked = false;
	bool reqOnStartupChecked = true;

	QDialog* s = new QDialog(this);
	s->setWindowTitle(tr("User Settings"));
	QVBoxLayout* mainLayout = new QVBoxLayout(s);

	QGroupBox *groupBox = new QGroupBox(tr("Visualization Groups"));
	groupBox->setStyleSheet("QGroupBox{background-color:white;}");
	
	QVBoxLayout* vlayout = new QVBoxLayout(s);

	QCheckBox *autoLoadCheckbox = new QCheckBox(this);
	autoLoadCheckbox->setText("Require Selection on Startup");
	autoLoadCheckbox->setChecked(reqOnStartupChecked);
	autoLoadCheckbox->setDisabled(groups.size() <= 1);
	vlayout->addWidget(autoLoadCheckbox);

	QCheckBox *checkbox = new QCheckBox(this);
	checkbox->setText("Load "+groupName+" on Startup");
	checkbox->setChecked(onStartupChecked);
	checkbox->setDisabled(groups.size() <= 1);
	vlayout->addWidget(checkbox);

	m_groupsComboBox = new QComboBox(s);
	m_groupsComboBox->addItems(groups);
	vlayout->addWidget(m_groupsComboBox);

	QHBoxLayout* hlayout = new QHBoxLayout(s);
	QPushButton* defButton = new QPushButton(tr("Set as Default"), s);
	defButton->setDisabled(groups.size() <= 1);
	hlayout->addWidget(defButton, 1);
	QPushButton* loadButton = new QPushButton(tr("Load"), s);
	loadButton->setDisabled(groups.isEmpty());
	hlayout->addWidget(loadButton, 1);

	vlayout->addLayout(hlayout);
	groupBox->setLayout(vlayout);

	mainLayout->addWidget(groupBox);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, s);
	mainLayout->addWidget(buttonBox);

	s->setLayout(mainLayout);

	/*QObject::connect(defButton, &QPushButton::clicked, checkbox, [=](){
		checkbox->setText("Load " + m_groupsComboBox->currentText() + " on Startup");
		autoLoadCheckbox->setChecked(false);
		checkbox->setChecked(true);
		QSettings st1;
		st1.beginGroup(QString::number(m_dataEngineConnection->UserAccessControls()->GetUserID()));
		st1.setValue("GroupName", m_groupsComboBox->currentText());
		st1.endGroup();
	});
	QObject::connect(checkbox, &QCheckBox::stateChanged, checkbox, [=](){
		QSettings st2;
		st2.beginGroup(QString::number(m_dataEngineConnection->UserAccessControls()->GetUserID()));
		st2.setValue("OnStartupChecked", checkbox->isChecked());
		st2.endGroup();
	});
	QObject::connect(autoLoadCheckbox, &QCheckBox::stateChanged, autoLoadCheckbox, [=](){
		QSettings st3;
		st3.beginGroup(QString::number(m_dataEngineConnection->UserAccessControls()->GetUserID()));
		st3.setValue("ReqOnStartupChecked", autoLoadCheckbox->isChecked());
		st3.endGroup();
	});*/
	QObject::connect(loadButton, &QPushButton::clicked, this, &GlyphViewerWindow::SwitchVisualizationGroup);
	QObject::connect(buttonBox, &QDialogButtonBox::rejected, s, &QDialog::reject);

	SetUserSettingsDialog(s);
}

void GlyphViewerWindow::SwitchVisualizationGroup() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	m_userSettings->accept();

	QString groupName = m_groupsComboBox->currentText();

	if (MainWindow::HasOpenFile()){
		CloseVisualization();
	}
	//m_homePage->ResetViews();

	//m_dataEngineConnection->UserAccessControls()->SetChosenGroup(groupName);

	//m_dataEngineConnection->UserAccessControls()->FlushOutFilterSetup();

	//m_homePage->ReSyncFilesAndLoadViews();

	/*QSettings groupSettings;
	groupSettings.beginGroup(groupName);
	groupSettings.setValue("LastUserToAccess", m_dataEngineConnection->UserAccessControls()->GetUserID());
	groupSettings.setValue("DirectoryPath", m_dataEngineConnection->UserAccessControls()->GlyphEdPath());
	groupSettings.setValue("VisualizationNames", m_dataEngineConnection->UserAccessControls()->VizualizationNames());
	groupSettings.endGroup();*/

	SynGlyphX::Application::restoreOverrideCursor();
}

void GlyphViewerWindow::ClearAllData() {

	m_glyphForestSelectionModel->ClearAll();
	m_sourceDataCache->Close();
	m_glyphForestModel->ClearAndReset();
	m_mappingModel->ClearAndReset();
	m_legendsWidget->ClearLegends();
	//m_filteringWidget->OnNewVisualization();
	m_interactiveLegendDock->close();
	m_hudGenerationInfo.clear();
}

void GlyphViewerWindow::LoadVisualization(const QString& filename, const MultiTableDistinctValueFilteringParameters& filters) {

	QFileInfo fileInfo(filename);
	QString extension = fileInfo.suffix().toLower();
	if (extension != "sdt") {

		throw std::runtime_error("File is not a recognized format");
	}

	if (!fileInfo.exists()) {

		throw std::runtime_error("File does not exist");
	}

	QStackedWidget* centerWidgetsContainer = dynamic_cast<QStackedWidget*>(centralWidget());
	centerWidgetsContainer->setCurrentIndex(1);

	if (m_glyphForestModel->rowCount() > 0) {

		ClearAllData();
		// if ( m_glyph3DView ) m_glyph3DView->update();
	}

	m_viewer->clearScene();

	if (extension == "sdt") {

		LoadDataTransform(filename, filters);

		/*try {

			SynGlyphX::DataTransformMapping::SharedPtr mapping = std::make_shared<SynGlyphX::DataTransformMapping>();
			mapping->ReadFromFile(filename.toStdString());

			ValidateDataMappingFile(mapping, filename);

			MultiTableDistinctValueFilteringParameters filters;
			if (!mapping->GetFrontEndFilters().empty()) {

				LoadingFilterDialog loadingFilterDialog(m_dataEngineConnection, filename, this);
				loadingFilterDialog.SetupFilters(*mapping);
				if (loadingFilterDialog.exec() == QDialog::Rejected) {

					return;
				}
				filters = loadingFilterDialog.GetFilterValues();
			}

			LoadDataTransform(filename, filters);
		}
		catch (const std::exception& e) {

			QMessageBox::critical(this, tr("Visualization failed to load"), tr("Visualization failed to load: ") + e.what());
		}*/
	}

	if (m_legendsWidget->HasLegends()) {

		m_legendsDockWidget->show();
	}
	else if (m_legendsDockWidget->isFloating()) {

		m_legendsDockWidget->hide();
	}

	if (m_rightDockWidget->isHidden()) {

		m_rightDockWidget->show();
	}
}

bool GlyphViewerWindow::LoadNewVisualization(const QString& filename, MultiTableDistinctValueFilteringParameters filters, bool useFEFilterList) {

	//if (HasValidLicense()){

		SGX_PROFILE_SCOPE
		QString nativeFilename = QDir::toNativeSeparators(filename);
		if (nativeFilename == m_currentFilename) {

			return true;
		}

		if (useFEFilterList)
		{
			m_FEfilterListWidget->update(nativeFilename.toStdString().c_str(), filters);
		}
		else
		{
			m_leftDockWidget->hide();
		}

		GVGlobal::Services()->ClearUndoStack();
		try {

			LoadVisualization(nativeFilename, filters);
		}
		catch (const std::exception& e) {

			QMessageBox critical_error(QMessageBox::Critical, tr("Failed To Open Project"), tr("Failed to open visualization.  Error: ") + e.what(), QMessageBox::Ok, this);
			//critical_error.setDetailedText(m_dataEngineConnection->JavaErrors());
			critical_error.setStyleSheet("QLabel{margin-right:75px;},QTextEdit{min-width:500px;}");
			critical_error.setStandardButtons(QMessageBox::Ok);
			critical_error.setDefaultButton(QMessageBox::Ok);
			critical_error.setEscapeButton(QMessageBox::Ok);
			critical_error.exec();
			//m_dataEngineConnection->ClearJavaErrors();
			CloseVisualization();
			//QMessageBox::critical(this, tr("Failed To Open Visualization"), tr("Failed to open visualization.  Error: ") + e.what(), QMessageBox::Ok);
			return false;
		}

		SetCurrentFile(nativeFilename);
		if (!filters.empty()) {

			s_recentFilters[nativeFilename] = filters;
		}
		else if (s_recentFilters.contains(nativeFilename)) {

			s_recentFilters.remove(nativeFilename);
		}
		EnableLoadedVisualizationDependentActions(true);
		m_ToggleFEFilterListAction->setEnabled(useFEFilterList);

		statusBar()->showMessage("Visualization successfully opened", 3000);
		return true;
	/*}
	else
	{
		//QString contact = SynGlyphX::GlyphBuilderApplication::IsGlyphEd() ? "<a href=\"mailto:mark@GlyphEd.co\">Mark@GlyphEd.co</a>" : "<a href=\"mailto:mark@synglyphx.com\">Mark@SynGlyphX.com</a>";
		QString contact = "<a href=\"http://www.synglyphx.com\">SynGlyphX</a>";
		QMessageBox::information(this, tr("No Active User"),
			QString::fromStdWString(L"<p>This application requires an active user account to perform this action. Please log in to continue.</p>"
			L"<p>To obtain an account, or to renew your license please contact " + contact.toStdWString() + L".</p>"),
			QMessageBox::Ok);
		return true;
	}*/
}

bool GlyphViewerWindow::LoadRecentFile(const QString& filename) {

	if (s_recentFilters.contains(filename)) {

		return m_homePage->LoadVisualization(filename, s_recentFilters[filename], true);
	}
	else {

		return LoadNewVisualization(filename);
	}
}

void GlyphViewerWindow::ValidateDataMappingFile(SynGlyphX::DataTransformMapping::SharedPtr mapping, const QString& filename) {

	QFileInfo fi(filename);
	if (!fi.isWritable()) {

		return;
	}

	if (mapping->GetDatasources().empty()) {

		throw std::runtime_error("Visualization has no datasources.");
	}

	if (mapping->GetGlyphGraphs().empty()) {

		throw std::runtime_error("Visualization has no glyph templates.");
	}

	if (!mapping->DoesAtLeastOneGlyphGraphHaveBindingsOnPosition()) {

		throw std::runtime_error("Visualization has no glyph templates with bindings on Position X, Position Y, or Position Z.");
	}

	bool wasDataTransformUpdated = false;

	std::vector<boost::uuids::uuid> missingFileDatasources = mapping->GetFileDatasourcesWithInvalidFiles(true);
	std::vector<unsigned int> missingLegends = mapping->GetLegendsWithInvalidFiles();
	std::vector<unsigned int> localBaseImageIndexes = mapping->GetFileBaseObjectsWithInvalidFiles();

	if (!localBaseImageIndexes.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeImageFileDialog::UpdateImageFiles(localBaseImageIndexes, filename, mapping, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		if (!wasDataTransformUpdated) {

			throw std::runtime_error("Visualization has missing base images that need to be updated to their correct locations before the visualization can be loaded");
		}
	}

	if (!missingLegends.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeImageFileDialog::UpdateLegendFiles(missingLegends, filename, mapping, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		if (!wasDataTransformUpdated) {

			throw std::runtime_error("Visualization has missing legends that need to be updated to their correct locations before the visualization can be loaded");
		}
	}

	if (!missingFileDatasources.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeDatasourceFileDialog::UpdateDatasourceFiles(missingFileDatasources, filename, mapping, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		if (!wasDataTransformUpdated) {

			throw std::runtime_error("Visualization has missing data sources that need to be updated to their correct locations before the visualization can be loaded");
		}
	}

	/*if (wasDataTransformUpdated) {

		mapping->WriteToFile(filename.toStdString());
	}*/

	mapping->WriteToFile(filename.toStdString());

	SynGlyphX::Application::restoreOverrideCursor();
}

void GlyphViewerWindow::LoadDataTransform(const QString& filename, const MultiTableDistinctValueFilteringParameters& filters) {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	try {

		m_mappingModel->LoadDataTransformFile(filename);

		std::string dcd = GlyphViewerOptions::GetDefaultCacheDirectory().toStdString();
		std::string cacheDirectoryPath = dcd + ("/cache_" + boost::uuids::to_string(m_mappingModel->GetDataMapping()->GetID()));

		std::string dirPath = cacheDirectoryPath + "/";
		std::string baseImageDir = SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation().toStdString();

		QFile dir(QString::fromStdString(dirPath));
		QFileInfo info(dir);
		qint64 mod = info.lastModified().toSecsSinceEpoch();
		bool needsUpdate = (lastModified - mod) > 0;
		SynGlyphX::Application::restoreOverrideCursor();
	}
	catch (const std::exception&) {

		SynGlyphX::Application::restoreOverrideCursor();
		throw;
	}
}

void GlyphViewerWindow::LoadFilesIntoModel() {

	m_glyphForestModel->LoadGlyphForestInfo( m_viewer->getScene() );
	
	SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping = m_mappingModel->GetDataMapping();
	auto ifm = std::const_pointer_cast<SynGlyphX::DataTransformMapping>(dataTransformMapping)->GetInputFieldManager();
	
	m_hudGenerationInfo.clear();
	for (const auto& rootGlyph : dataTransformMapping->GetGlyphGraphs()) {

		QMap<unsigned int, QString> fields, displayNames;
		SynGlyphX::InputTable table = dataTransformMapping->GetInputTable(rootGlyph.first);
		auto fieldToAliasMap = dataTransformMapping->GetFieldToAliasMapForTable(table);
		
		for (unsigned int i = 0; i < 3; ++i) {

			const SynGlyphX::InputBinding& posInputBinding = rootGlyph.second->GetRoot()->second.GetPosition()[i].GetBinding();
			SynGlyphX::InputField field = ifm->GetInputField(posInputBinding.GetInputFieldID());
			if (field.IsValid()) {

				fields.insert(i, QString::fromStdWString(field.GetField()));
				if (fieldToAliasMap.count(field.GetField()) == 0) {

					displayNames.insert(i, fields[i]);
					displayNames[i].replace('_', ' ');
				}
				else {

					displayNames.insert(i, QString::fromStdWString(fieldToAliasMap[field.GetField()]));
				}
			}
		}

		m_hudGenerationInfo.push_back(HUDGenerationInfo(table, fields, displayNames));
	}

	UpdateAxisNamesAndSourceDataPosition();
}
	
void GlyphViewerWindow::UpdateAxisNamesAndSourceDataPosition() {
	
	try {
		unsigned int hudInfoIndex = 0;
		QModelIndexList selectedIndexes = m_glyphForestSelectionModel->selectedIndexes();
		if (!selectedIndexes.empty()) {
			/*
			try {

				unsigned long rootIndex = SynGlyphX::ItemFocusSelectionModel::GetRootRow(selectedIndexes.back());

				//out << "og_root: " << rootIndex << endl;

				int index = rootIndex+6;
				QString uid = m_viewer->reader().getIndexToUID()[index];
				QList<int> ids = m_viewer->reader().getStackedGlyphMap()[uid]->glyphIds;
				//float z = m_viewer->reader().getStackedGlyphMap()[uid].tagValue;
				float z = m_viewer->reader().getStackedGlyphMap()[uid]->currentTagValue;
				rootIndex = ids[0]-6;

				boost::optional<std::pair<unsigned int, unsigned long>> indexes = m_filteringManager->GetGlyphTemplateAndTableIndex(rootIndex);

				if (indexes == boost::none) {
					AwsLogger::getInstance()->localLogger("Could not get source data for position X, Y, Z");
					throw std::runtime_error(tr("Could not get source data for position X, Y, & Z").toStdString().c_str());
				}
				else {

					hudInfoIndex = indexes.get().first;
					const QMap<unsigned int, QString>& fields = m_hudGenerationInfo[hudInfoIndex].GetFields();
					QList<QVariant> posSourceDataVar = m_sourceDataCache->GetValuesForRow(m_hudGenerationInfo[hudInfoIndex].GetTable(),
						fields.values(), indexes.get().second);
					std::map<std::wstring, SynGlyphX::FieldProperties> fieldProperties = m_filteringManager->GetFieldPropertiesForTable(
						m_hudGenerationInfo[hudInfoIndex].GetTable().GetDatasourceID(),
						m_hudGenerationInfo[hudInfoIndex].GetTable().GetTable());

					std::array<std::string, 3> posSourceData;
					unsigned int j = 0;
					for (unsigned int i = 0; i < 3; ++i) {

						if (fields.contains(i)) {

							//Conversion is being done this way to allow formatting numbers
							if (fieldProperties.find(fields[i].toStdWString()) != fieldProperties.end()){
								posSourceData[i] = fieldProperties.at(fields[i].toStdWString()).transformData(posSourceDataVar[j].toString()).replace("%", "%%").toStdString();
							}
							else{
								posSourceData[i] = posSourceDataVar[j].toString().toStdString();
							}
							++j;
						}
					}

				}
			}
			catch (const std::exception& e) {

				QMessageBox::warning(this, tr("Source Data Error"), e.what());
			}
			*/

			unsigned long rootIndex = SynGlyphX::ItemFocusSelectionModel::GetRootRow(selectedIndexes.back());
			int index = rootIndex + 6;
			QString uid = m_viewer->reader().getIndexToUID()[index];
			QList<int> ids = m_viewer->reader().getStackedGlyphMap()[uid]->glyphIds;
			rootIndex = ids[0] - 6;

			AwsLogger::getInstance()->localLogger("index: " + QString::number(rootIndex));

			GetRowById(rootIndex);
		}

		const QMap<unsigned int, QString>& displayNames = m_hudGenerationInfo[hudInfoIndex].GetDisplayNames();
		m_viewer->setAxisNames(displayNames.contains(0) ? displayNames[0].toStdString().c_str() : "",
			displayNames.contains(1) ? displayNames[1].toStdString().c_str() : "",
			displayNames.contains(2) ? displayNames[2].toStdString().c_str() : "");
	}
	catch (const std::exception& e) {
		QMessageBox::warning(this, tr("Source Data Error"), e.what());
	}
}

QString GlyphViewerWindow::HitAthenaAPI(QList<int> ids, bool async=false) {

	try {
		QString url = "https://q8gv8df1a2.execute-api.us-east-2.amazonaws.com/prod/etl/get-row-by-id";
		QNetworkRequest request(url);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		QString arr_str = "[";
		for (int i = 0; i < ids.size(); i++)
		{
			arr_str += QString::number(ids[i] - 6 + 1);
			if (i < ids.size() - 1)
				arr_str += ",";
		}
		arr_str += "]";

		QJsonObject obj;
		obj["tableName"] = athenaTableName; //"01388c0b_5f5f_49ff_8c15_b6e902604b14";
		obj["rowNum"] = arr_str;
		QJsonDocument doc(obj);
		QByteArray data = doc.toJson();

		QNetworkAccessManager networkManager;
		QNetworkReply *reply = networkManager.post(request, data);

		if (!async) {
			QEventLoop loop;
			QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

			if (reply->isRunning()) {
				loop.exec();
			}
		}
		reply->deleteLater();

		if (reply->error() != QNetworkReply::NoError) {
			qDebug() << (reply->errorString().toStdString()).c_str();
			throw DownloadException(("Network Error: " + reply->errorString() + "\n_" + url + "_").toStdString().c_str());
		}

		return async ? "" : QString::fromUtf8(reply->readAll());

	}
	catch (const std::exception& e) {

		QMessageBox::warning(this, tr("Source Data Error"), e.what());
	}
	return "";
}

void GlyphViewerWindow::GetRowById(long id) {

	WaitingSpinnerWidget* spinner = new WaitingSpinnerWidget(drawerDock, true, false);
	spinner->setColor(QColor(254, 205, 8));
	spinner->setMinimumWidth(glyphDrawer->width());
	drawerDock->setWidget(spinner);
	drawerDock->show();
	spinner->start();

	QTimer* timer = new QTimer(this);
	long _revolutionsPerSecond = 1.57079632679489661923;
	long _numberOfLines = 20;
	timer->setInterval(1000 / (_numberOfLines * _revolutionsPerSecond));
	QObject::connect(timer, &QTimer::timeout, spinner, &WaitingSpinnerWidget::rotate);
	timer->start();

	if (!drawerDockHeightSet) {
		drawerDock->setMinimumHeight(this->size().height()*0.1);
		drawerDockHeightSet = true;
	}
	
	try {

		int index = id+6;
		QString uid = m_viewer->reader().getIndexToUID()[index];
		//QList<int> ids = m_viewer->reader().getStackedGlyphMap()[uid].glyphIds;
		QList<int> ids = m_viewer->reader().getStackedGlyphMap()[uid]->currentGlyphIds;
		
		QString contents = HitAthenaAPI(ids);
		if (contents.size() == 0) {

			//throw DownloadException("Returned no data");
			drawerDock->hide();
		}
		else {

			QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
			QJsonArray arr = doc.array();
			QJsonArray fields = arr.at(0).toObject().value("Data").toArray();
			QList<QJsonArray> rows;
			for (int i = 1; i < arr.size(); i++) {
				rows.append(arr.at(i).toObject().value("Data").toArray());
			}

			QTableWidget *table = new QTableWidget(rows.size(), fields.size(), this);
			QHeaderView* header = table->horizontalHeader();
			header->setSectionResizeMode(QHeaderView::Stretch);

			const QMap<unsigned int, QString>& displayNames = m_hudGenerationInfo[0].GetDisplayNames();
			int x_idx = 0;
			int y_idx = 1;
			int z_idx = 2;
			table->setStyleSheet(QString::fromUtf8("QWidget{background-color: #0f172a;color: #fff;}"));
			for (int i = 0; i < fields.size(); i++) {
				QString field = fields.at(i).toObject().value("VarCharValue").toString();
				QTableWidgetItem *header = new QTableWidgetItem(field);
				table->setHorizontalHeaderItem(i, header);
				if (field == displayNames[0]) {
					x_idx = i;
				}
				if (field == displayNames[1]) {
					y_idx = i;
				}
				if (field == displayNames[2]) {
					z_idx = i;
				}
			}

			double valZ = 0;
			for (int r = 0; r < rows.size(); r++) {
				for (int i = 0; i < fields.size(); i++) {
					QString value = rows.at(r).at(i).toObject().value("VarCharValue").toString();
					QTableWidgetItem *dataVal = new QTableWidgetItem(value);
					table->setItem(r, i, dataVal);
					if (i == z_idx) {
						valZ += value.toDouble();
					}
				}
			}
			drawerDock->setWidget(table);
			if (fields.size() == 0) {
				drawerDock->hide();
			}

			QString valX = rows.at(0).at(x_idx).toObject().value("VarCharValue").toString();
			QString valY = rows.at(0).at(y_idx).toObject().value("VarCharValue").toString();
			std::array<std::string, 3> posSourceData;
			posSourceData[0] = valX.toStdString();
			posSourceData[1] = valY.toStdString();
			posSourceData[2] = QString::number(valZ).toStdString();
			m_viewer->setOverridePositionXYZ(posSourceData);

		}
	}
	catch (const std::exception& e) {

		QMessageBox::warning(this, tr("Source Data Error"), e.what());
	}

}

void GlyphViewerWindow::ChangeMapDownloadSettings() {

	DownloadOptionsDialog dialog(this);
	dialog.exec();
}

void GlyphViewerWindow::ShowOpenGLSettings() {

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

void GlyphViewerWindow::ChangeStereoMode() {

	if (m_viewer->stereoSupported()) {

		try {

			m_viewer->setStereoMode(!m_viewer->stereoMode());
		}
		catch (const std::exception& e) {

			QMessageBox::information(this, tr("Error"), tr("Stereo Error: ") + e.what());
		}
	}
	else {

		m_stereoAction->setChecked(false);
		QMessageBox::information(this, tr("Stereo not supported"), tr("Stereo is not supported. Check your driver settings to see if stereo is enabled and available for your hardware.  Contact the manufacturer of your GPU for assitance."));
	}
}

void GlyphViewerWindow::EnableLoadedVisualizationDependentActions(bool enable) {

	for (int i = 0; i < m_loadedVisualizationDependentActions.length(); ++i) {

		m_loadedVisualizationDependentActions[i]->setEnabled(enable);
	}
	m_hideFilteredCheckBox->setEnabled(enable);

	m_stereoAction->setEnabled(!enable);

	m_clearSelectionAction->setEnabled(false);
	m_showTagsAction->setEnabled(false);
	m_hideTagsAction->setEnabled(false);
}

void GlyphViewerWindow::ChangeOptions() {

	GlyphViewerOptions oldOptions = CollectOptions();
	OptionsWidget* optionsWidget = new OptionsWidget(oldOptions, (m_glyphForestModel->rowCount() == 0), this);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, optionsWidget, this);
	dialog.setWindowTitle(tr("Glyph Viewer Options"));

	if (dialog.exec() == QDialog::Accepted) {

		try {

			ChangeOptions(oldOptions, optionsWidget->GetOptions());
		}
		catch (const std::exception& e) {

			QMessageBox::warning(this, tr("Options Error"), tr("Options was not updated: ") + e.what());
		}
	}
}

void GlyphViewerWindow::ChangeBackgroundColor() {
	QColor color = QColorDialog::getColor(Qt::black, this);
	if (color.isValid())
	{
		auto r = color.redF();
		auto g = color.greenF();
		auto b = color.blueF();
		if (m_viewer) m_viewer->setBackgroundColor(glm::vec4(float(r), float(g), float(b), 1.f));
	}
}

void GlyphViewerWindow::ChangeOptions(const GlyphViewerOptions& oldOptions, const GlyphViewerOptions& newOptions) {

	if (oldOptions != newOptions) {

		if (oldOptions.GetCacheDirectory() != newOptions.GetCacheDirectory()) {

			QString newCacheDirectory(newOptions.GetCacheDirectory());
			QDir cacheDir(newCacheDirectory);
			if (!cacheDir.exists()) {

				if (!cacheDir.mkpath(newCacheDirectory)) {

					throw std::invalid_argument("Unable to create " + newCacheDirectory.toStdString());
				}
			}

			m_cacheManager.SetBaseCacheDirectory(newCacheDirectory.toStdWString());
		}

		if (oldOptions.GetShowHUDAxisObject() != newOptions.GetShowHUDAxisObject()) {

			m_viewer->enableHUDAxes(newOptions.GetShowSceneAxisObject());
		}

		if (oldOptions.GetHUDAxisObjectLocation() != newOptions.GetHUDAxisObjectLocation()) {

			m_viewer->setHUDAxesLocation(SynGlyphX::HUDAxesLocation(newOptions.GetHUDAxisObjectLocation()));
		}

		if (oldOptions.GetShowSceneAxisObject() != newOptions.GetShowSceneAxisObject()) {

			m_viewer->enableSceneAxes(newOptions.GetShowSceneAxisObject());
			
		}

#ifdef USE_ZSPACE
		// TODO: implement
/*		if (oldOptions.GetZSpaceOptions() != newOptions.GetZSpaceOptions()) {

			m_viewer->setZSpaceOptions(newOptions.GetZSpaceOptions());
		}*/
#endif

		if (oldOptions.GetHideUnselectedGlyphTrees() != newOptions.GetHideUnselectedGlyphTrees()) {

			m_linkedWidgetsManager->SetFilterView(newOptions.GetHideUnselectedGlyphTrees());
		}

		if (oldOptions.GetLoadSubsetVisualization() != newOptions.GetLoadSubsetVisualization()) {

			//m_filteringWidget->SetLoadSubsetVisualization(newOptions.GetLoadSubsetVisualization());
		}

		if (oldOptions.GetLoadSubsetVisualizationInNewInstance() != newOptions.GetLoadSubsetVisualizationInNewInstance()) {

			//m_filteringWidget->SetLoadSubsetVisualizationInNewInstance(newOptions.GetLoadSubsetVisualizationInNewInstance());
		}

		if (oldOptions.GetShowMessageWhenImagesDidNotDownload() != newOptions.GetShowMessageWhenImagesDidNotDownload()) {

			m_showErrorFromTransform = newOptions.GetShowMessageWhenImagesDidNotDownload();
		}

		m_viewer->setFilteredGlyphOpacity( newOptions.GetFilteredGlyphOpacity() );
		if (oldOptions.GetShowHomePage() != newOptions.GetShowHomePage()) {

			m_showHomePage = newOptions.GetShowHomePage();
		}

		if (oldOptions.GetDefaultProject() != newOptions.GetDefaultProject()) {

			m_defaultProject = newOptions.GetDefaultProject();
		}
	}

	m_showHideHUDAxisAction->setChecked(newOptions.GetShowHUDAxisObject());
	m_showHideSceneAxisAction->setChecked(newOptions.GetShowSceneAxisObject());
}

void GlyphViewerWindow::ReadSettings() {

	SynGlyphX::MainWindow::ReadSettings();

	s_subsetFileList.ReadFromSettings();

	GlyphViewerOptions options;

	boost::uuids::string_generator gen;

	QSettings settings;
	s_recentFilters.clear();
	settings.beginGroup("Filters");
	QStringList files = settings.childGroups();

	for (const QString& file : files) {

		MultiTableDistinctValueFilteringParameters filtersAllTables;
		settings.beginGroup(file);

		QStringList tables = settings.childGroups();

		for (const QString& table : tables) {

			DistinctValueFilteringParameters filtersSingleTable;

			settings.beginGroup(table);
			QStringList datasourceTablePair = table.split(':');
			if (datasourceTablePair.size() != 2)
				continue;
			SynGlyphX::InputTable inputTable(gen(datasourceTablePair[0].toStdString()), datasourceTablePair[1].toStdWString());

			QStringList fieldNames = settings.childGroups();
			for (const QString& fieldName : fieldNames) {

				int numberOfValues = settings.beginReadArray(fieldName);
				QSet<QString> filterValues;
				for (int i = 0; i < numberOfValues; ++i) {

					settings.setArrayIndex(i);
					filterValues.insert(settings.value("FilterValue").toString());
				}
				settings.endArray(); //filter values array

				filtersSingleTable.SetDistinctValueFilter(fieldName, filterValues);
			}

			filtersAllTables.push_back(std::make_pair(inputTable, filtersSingleTable));

			settings.endGroup(); //table group
		}

		settings.endGroup(); //file group

		s_recentFilters[file] = filtersAllTables;
	}

	settings.endGroup(); //filter group

	settings.beginGroup("Display");
	if (!settings.value("ShowAnimation", true).toBool()) {

		m_showAnimation->toggle();
	}
	settings.endGroup();

	options.ReadFromSettings();

	ChangeOptions(CollectOptions(), options);
	if (m_showHomePage || SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {
		dynamic_cast<QStackedWidget*>(centralWidget())->setCurrentIndex(0); 
		//if (!options.GetDefaultProject().isEmpty())
			//m_homePage->LoadProject(options.GetDefaultProject());
	}
	else
		dynamic_cast<QStackedWidget*>(centralWidget())->setCurrentIndex(1);
}

void GlyphViewerWindow::WriteSettings() {

	SynGlyphX::MainWindow::WriteSettings();

	QSettings settings;

	settings.beginGroup("Display");
	settings.setValue("ShowAnimation", m_showAnimation->isChecked());
	settings.endGroup();

	settings.beginGroup("Filters");
	settings.remove("");

	for (auto file : s_recentFilters.keys()) {

		settings.beginGroup(file);

		for (const auto& table : s_recentFilters[file]) {

			QString tableGroup = QString::fromStdString(boost::uuids::to_string(table.first.GetDatasourceID())) + ":" + 
				QString::fromStdWString(table.first.GetTable());
			settings.beginGroup(tableGroup);

			QStringList fieldNames = settings.allKeys();
			for (const auto& field : table.second.GetDistinctValueFilters()) {

				settings.beginWriteArray(field.first);
				QSet<QString> filterValues;
				unsigned index = 0;
				for (const auto& filterValue : field.second) {

					settings.setArrayIndex(index++);
					settings.setValue("FilterValue", filterValue);
				}
				settings.endArray(); //filter values array
			}

			settings.endGroup(); //table group
		}

		settings.endGroup(); //file group
	}

	settings.endGroup();

	GlyphViewerOptions options = CollectOptions();
	options.WriteToSettings();
}

GlyphViewerOptions GlyphViewerWindow::CollectOptions() {

	GlyphViewerOptions options;

	options.SetCacheDirectory(QString::fromStdWString(m_cacheManager.GetBaseCacheDirectory()));
	
	if ( m_viewer )
	{
		auto location = m_viewer->hudAxesLocation();

		options.SetShowHUDAxisObject( m_viewer->hudAxesEnabled() );
		options.SetHUDAxisObjectLocation( location );
		options.SetShowSceneAxisObject( m_viewer->sceneAxesEnabled() );
#ifdef USE_ZSPACE
		// TODO 
		// options.SetZSpaceOptions( m_glyph3DView->GetZSpaceOptions() );
#endif
	}

	options.SetHideUnselectedGlyphTrees(m_linkedWidgetsManager->GetFilterView());
	//options.SetLoadSubsetVisualization(m_filteringWidget->GetLoadSubsetVisualization());
	//options.SetLoadSubsetVisualizationInNewInstance(m_filteringWidget->GetLoadSubsetVisualizationInNewInstance());

	options.SetShowMessageWhenImagesDidNotDownload(m_showErrorFromTransform);
	options.SetShowHomePage(m_showHomePage);
	options.SetDefaultProject(m_defaultProject);

	return options;
}

void GlyphViewerWindow::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	bool selectionIsNotEmpty = !m_glyphForestSelectionModel->selectedIndexes().isEmpty();
	m_clearSelectionAction->setEnabled(selectionIsNotEmpty);
	m_showTagsAction->setEnabled(selectionIsNotEmpty);
	m_hideTagsAction->setEnabled(selectionIsNotEmpty);

	m_openURLAction->setEnabled(selectionIsNotEmpty);
	m_propertiesAction->setEnabled(selectionIsNotEmpty);

	UpdateAxisNamesAndSourceDataPosition();
}

void GlyphViewerWindow::CreateExportToPortableVisualizationSubmenu() {

	if (m_portableVisualizationExport.DoAnyPlatformsHaveSourceDirectories()) {

		m_exportGlyphPortableAction = new QAction(tr("GlyphPortable"), this);
		m_loadedVisualizationDependentActions.push_back(m_exportGlyphPortableAction);

		m_fileMenu->addSeparator();
		QMenu* portableVisualizationMenu = m_fileMenu->addMenu(tr("Create Portable Visualization"));

		m_portableVisualizationExport.CreateSubmenu( portableVisualizationMenu );
		QObject::connect(&m_portableVisualizationExport, &SynGlyphX::PortableVisualizationExport::CreatePortableVisualization, this, &GlyphViewerWindow::CreatePortableVisualization);

		for (auto action : portableVisualizationMenu->actions()) {

			m_loadedVisualizationDependentActions.push_back(action);
		}
	}
}

void GlyphViewerWindow::CreatePortableVisualization(SynGlyphX::PortableVisualizationExport::Platform platform) {

	QSet<QString> projectFileDirs;
	projectFileDirs.insert(QDir::toNativeSeparators(m_currentFilename));
	for (const auto& datasource : m_mappingModel->GetDataMapping()->GetDatasources()) {

		if (datasource.second->GetSourceType() == SynGlyphX::Datasource::SourceType::File) {

			SynGlyphX::FileDatasource::ConstSharedPtr fileDatasource = std::dynamic_pointer_cast<const SynGlyphX::FileDatasource>(datasource.second);
			projectFileDirs.insert(QDir::toNativeSeparators(QString::fromStdWString(fileDatasource->GetFilename())));
		}
	}

	QString csvDirectory = QDir::toNativeSeparators(GetExistingEmptyDirectory(projectFileDirs, "PortableVisualizationExportDir", tr("Select Directory For Portable Visualization"), "", tr("Selected directory contains one or more files relevant to the project.")));
	if (csvDirectory.isEmpty()) {

		return;
	}


	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	try {

		QDir destDir( csvDirectory );
		if ( destDir.exists() )
			SynGlyphX::Filesystem::RemoveContentsOfDirectory( csvDirectory.toStdString() );
			
		m_portableVisualizationExport.CopyContentsOfSourceDirectory( platform, csvDirectory );

		// compute the cache path and copy the cache to the output
		std::string dcd = GlyphViewerOptions::GetDefaultCacheDirectory().toStdString();
		std::string cacheDirectoryPath = dcd + ( "/cache_" + boost::uuids::to_string( m_mappingModel->GetDataMapping()->GetID() ) );
		std::string dirPath = cacheDirectoryPath + "/";
		QString cachePath = QString::fromStdString( dirPath + "scene/" );
		//m_portableVisualizationExport.CopyContentsOfSourceDirectory( platform, cachePath );
#ifdef __APPLE__
        csvDirectory += "/GlyphPortable.app/Contents/MacOS";
#endif
		for (QString cbImg : m_currentBaseImages){
			SynGlyphX::Filesystem::CopyFileOverwrite(cbImg.toStdString(), csvDirectory.toStdString() + "/base_img.png");
		}
		SynGlyphX::Filesystem::CopyFileOverwrite(cachePath.toStdString() + "/glyphs.sgc", csvDirectory.toStdString() + "/glyphs.sgc");
		SynGlyphX::Filesystem::CopyFileOverwrite(cachePath.toStdString() + "/glyphs.sgn", csvDirectory.toStdString() + "/glyphs.sgn");
		//SynGlyphX::Filesystem::CopyDirectoryOverwrite(cachePath.toStdString(), csvDirectory.toStdString(), true);

		SynGlyphX::Application::restoreOverrideCursor();
	}
	catch (const std::exception& e) {

		try {

			SynGlyphX::Filesystem::RemoveContentsOfDirectory(csvDirectory.toStdString());
			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::critical(this, tr("Create Portable Visualization Error"), e.what());
		}
		catch (...) {

			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::information(this, tr("Directory in use"), tr("Could not create portable visualization because files in this directory are currently in use."), QMessageBox::Ok);
		}
		return;
	}

	statusBar()->showMessage("Portable visualization successfully created", 6000);
}

/*void GlyphViewerWindow::DownloadBaseImages(DataEngine::GlyphEngine& ge) {

	try {

		ge.getDownloadedBaseImage(m_mappingModel->GetDataMapping().get()->GetBaseObjects());
	}
	catch (const DownloadException& e) {

		SynGlyphX::Application::restoreOverrideCursor();
		QMessageBox::information(this, "Download Image Error", tr("Base image failed to download so the world map was used instead.\n\nError: ") + tr(e.what()), QMessageBox::Ok);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	}
	catch (const std::exception&) {

		throw;
	}
}*/

void GlyphViewerWindow::closeEvent(QCloseEvent* event) {

	//m_filteringWidget->CloseSourceDataWidgets();

	SynGlyphX::MainWindow::closeEvent(event);
}

void GlyphViewerWindow::RemapRootPositionMappings() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	RemapDialog remapDialog(m_mappingModel->GetDataMapping(), m_sourceDataCache->GetConnectionID(), this);
	SynGlyphX::Application::restoreOverrideCursor();

	remapDialog.SetSaveFilename(m_currentFilename);

	if (remapDialog.exec() == QDialog::Accepted) {

		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping = remapDialog.GetNewMapping();
		QString remapFilename = remapDialog.GetSaveFilename();
		dataTransformMapping->WriteToFile(remapFilename.toStdString());
		SynGlyphX::Application::restoreOverrideCursor();
		
		if (s_recentFilters.count(m_currentFilename) > 0) {

			LoadNewVisualization(remapFilename, s_recentFilters[m_currentFilename]);
		}
		else {

			LoadNewVisualization(remapFilename);
		}
	}
}

void GlyphViewerWindow::ToggleInteractiveLegend()
{
	/*
	if (m_interactiveLegendDock->isVisible())
	{
		m_interactiveLegendDock->hide();
	}
	else
	{*/
		m_interactiveLegend->setPrimaryViewer(m_viewer);
		addDockWidget(Qt::DockWidgetArea::NoDockWidgetArea, m_interactiveLegendDock);
		m_interactiveLegendDock->resize(m_interactiveLegend->size());
		//m_interactiveLegendDock->show();
	//}
}

void GlyphViewerWindow::CreateInteractionToolbar() {

	m_interactionToolbar = addToolBar(tr("Interaction"));
	m_interactionToolbar->setObjectName("main_toolbar");
	m_interactionToolbar->setFloatable(true);
	m_interactionToolbar->setMovable(true);
	m_interactionToolbar->setIconSize(SynGlyphX::Application::DynamicQSize(42, 32));

	m_showHideHUDAxisAction = new QAction(tr("Show/Hide HUD Axis"), m_interactionToolbar);
	m_showHideHUDAxisAction->setCheckable(true);
	QIcon hudAxisIcon;
	QPixmap axis_off(":SGXGUI/Resources/Icons/icon-axis-hud.png");
	QPixmap axis_on(":SGXGUI/Resources/Icons/icon-axis-hud-a.png");
	hudAxisIcon.addPixmap(axis_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	hudAxisIcon.addPixmap(axis_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_showHideHUDAxisAction->setIcon(hudAxisIcon);
	//Replace with scene viewer function when available
	QObject::connect(m_showHideHUDAxisAction, &QAction::toggled, this, &GlyphViewerWindow::OnShowHideHUDAxis);
	m_interactionToolbar->addAction(m_showHideHUDAxisAction);
	m_loadedVisualizationDependentActions.push_back(m_showHideHUDAxisAction);

	m_showHideSceneAxisAction = new QAction(tr("Show/Hide Scene Axis"), m_interactionToolbar);
	m_showHideSceneAxisAction->setCheckable(true);
	QIcon sceneAxisIcon;
	QPixmap scene_off(":SGXGUI/Resources/Icons/icon-axis-scene.png");
	QPixmap scene_on(":SGXGUI/Resources/Icons/icon-axis-scene-a.png");
	sceneAxisIcon.addPixmap(scene_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	sceneAxisIcon.addPixmap(scene_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_showHideSceneAxisAction->setIcon(sceneAxisIcon);
	//Replace with scene viewer function when available
	QObject::connect(m_showHideSceneAxisAction, &QAction::toggled, this, &GlyphViewerWindow::OnShowHideSceneAxis);
	m_interactionToolbar->addAction(m_showHideSceneAxisAction);
	m_loadedVisualizationDependentActions.push_back(m_showHideSceneAxisAction);

	m_interactionToolbar->addAction(m_remapRootPositionMappingsAction);

	m_interactionToolbar->addSeparator();

	m_interactionToolbar->addAction( m_resetCameraToDefaultPosition );

	m_enableDisableFlyToObjectAction = new QAction( tr( "Enable/Disable Fly-to-Object" ), m_interactionToolbar );
	m_enableDisableFlyToObjectAction->setCheckable( true );
	QIcon flyToObjectIcon;
	QPixmap flyby_off(":SGXGUI/Resources/Icons/icon-flyby.png");
	QPixmap flyby_on(":SGXGUI/Resources/Icons/icon-flyby-a.png");
	flyToObjectIcon.addPixmap(flyby_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	flyToObjectIcon.addPixmap(flyby_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_enableDisableFlyToObjectAction->setIcon( flyToObjectIcon );
	QObject::connect( m_enableDisableFlyToObjectAction, &QAction::toggled, this, &GlyphViewerWindow::OnEnableDisableFlyToObjectAction );
	m_interactionToolbar->addAction( m_enableDisableFlyToObjectAction );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableFlyToObjectAction );

	m_enableDisableFreeSelectionCameraAction = new QAction( tr( "Enable/Disable Orbit Selection" ), m_interactionToolbar );
	m_enableDisableFreeSelectionCameraAction->setCheckable( true );
	m_enableDisableFreeSelectionCameraAction->setChecked( true );
	QIcon freeSelCamIcon;
	QPixmap orbit_off(":SGXGUI/Resources/Icons/icon-orbit.png");
	QPixmap orbit_on(":SGXGUI/Resources/Icons/icon-orbit-a.png");
	freeSelCamIcon.addPixmap(orbit_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	freeSelCamIcon.addPixmap(orbit_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_enableDisableFreeSelectionCameraAction->setIcon( freeSelCamIcon );
	QObject::connect( m_enableDisableFreeSelectionCameraAction, &QAction::toggled, this, &GlyphViewerWindow::OnEnableDisableFreeSelectionCamera );
	m_interactionToolbar->addAction( m_enableDisableFreeSelectionCameraAction );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableFreeSelectionCameraAction );

	m_enableDisableSelEffectAction = new QAction( tr( "Enable/Disable Selection Effect" ), m_interactionToolbar );
	m_enableDisableSelEffectAction->setCheckable( true );
	m_enableDisableSelEffectAction->setChecked( true );
	QIcon selEffIcon;
	QPixmap blink_off(":SGXGUI/Resources/Icons/icon-blink.png");
	QPixmap blink_on(":SGXGUI/Resources/Icons/icon-blink-a.png");
	selEffIcon.addPixmap(blink_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	selEffIcon.addPixmap(blink_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_enableDisableSelEffectAction->setIcon( selEffIcon );
	QObject::connect( m_enableDisableSelEffectAction, &QAction::toggled, this, &GlyphViewerWindow::OnEnableDisableSelEffect );
	m_interactionToolbar->addAction( m_enableDisableSelEffectAction );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableSelEffectAction );

	m_enableDisableSuperimposedGlyphGadgets = new QAction( tr( "Enable/Disable Overlapping Group Indicators" ), m_interactionToolbar );
	m_enableDisableSuperimposedGlyphGadgets->setCheckable( true );
	m_enableDisableSuperimposedGlyphGadgets->setChecked( false );
	QIcon sggIcon;
	QPixmap groups_off(":SGXGUI/Resources/Icons/icon-show-groups.png");
	QPixmap groups_on(":SGXGUI/Resources/Icons/icon-show-groups-a.png");
	sggIcon.addPixmap(groups_off.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::Off);
	sggIcon.addPixmap(groups_on.scaled(SynGlyphX::Application::DynamicQSize(42, 32)), QIcon::Normal, QIcon::On);
	m_enableDisableSuperimposedGlyphGadgets->setIcon( sggIcon );
	QObject::connect( m_enableDisableSuperimposedGlyphGadgets, &QAction::toggled, this, &GlyphViewerWindow::OnEnableDisableSuperimposedGadgets );
	m_interactionToolbar->addAction( m_enableDisableSuperimposedGlyphGadgets );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableSuperimposedGlyphGadgets );

	m_interactionToolbar->addSeparator();

	m_hideFilteredCheckBox = new QCheckBox(tr("Hide Filtered"), this);
	m_interactionToolbar->addWidget(m_hideFilteredCheckBox);
	m_linkedWidgetsManager->AddFilterViewCheckbox(m_hideFilteredCheckBox);

	//m_toolbarsSubMenu->addAction(m_interactionToolbar->toggleViewAction());

}

void GlyphViewerWindow::OnStereoSetup(bool stereoEnabled) {

	m_stereoAction->setChecked(stereoEnabled);
}

void GlyphViewerWindow::OnShowHideHUDAxis(bool show) {

	m_viewer->enableHUDAxes( show );
}

void GlyphViewerWindow::OnShowHideSceneAxis(bool show) {

	m_viewer->enableSceneAxes( show );
}

void GlyphViewerWindow::OnOpenURLs() {

	bool anyURLsOpened = m_glyphForestModel->OpenURLs(m_glyphForestSelectionModel->selectedIndexes());
	if (!anyURLsOpened) {

		QMessageBox::information(this, tr("No URLs Opened"), tr("No files or URLs were opened since none of the selected objects had a file or URL associated with them."));
	}
}

void GlyphViewerWindow::OnPropertiesActivated() {

	const QModelIndexList& selectedItems = m_glyphForestSelectionModel->selectedIndexes();
	if (!selectedItems.empty()) {

		const QModelIndex& index = selectedItems.back();
		const SynGlyphX::GlyphForestInfoModel::GlyphTextProperties& glyphTextProperties = m_glyphForestModel->GetGlyphTextProperties(index);

		SynGlyphX::TextGlyphPropertiesWidget* glyphPropertiesWidget = new SynGlyphX::TextGlyphPropertiesWidget(this);
		glyphPropertiesWidget->SetWidget(glyphTextProperties[0], glyphTextProperties[2], glyphTextProperties[1]);
		glyphPropertiesWidget->SetReadOnly(true);

		SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok, glyphPropertiesWidget, this);
		dialog.setWindowTitle(tr("Glyph Properties"));
		dialog.exec();
	}
}

const SynGlyphX::SettingsStoredFileList& GlyphViewerWindow::GetSubsetFileListInstance() {

	return s_subsetFileList;
}

void GlyphViewerWindow::AddSubsetVisualization(const QString& filename) {

	// read and rewrite the settings immediately so that we stay in sync with any other open instances of the software
	s_subsetFileList.ReadFromSettings();
	s_subsetFileList.AddFile(filename);
	s_subsetFileList.WriteToSettings();
}

void GlyphViewerWindow::OnEnableDisableFlyToObjectAction( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
		m_viewer->enableFlyToObject( enable );
}

void GlyphViewerWindow::OnEnableDisableFreeSelectionCamera( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
		m_viewer->enableFreeSelectionCamera( !enable );
}

void GlyphViewerWindow::OnEnableDisableSelEffect( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
	{
		m_viewer->enableSelectionEffect( enable );
		m_enableDisableSelEffectActionMenu->setChecked( enable );
		m_enableDisableSelEffectAction->setChecked( enable );
	}
}

void GlyphViewerWindow::OnEnableDisableSuperimposedGadgets( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
	{
		m_viewer->enableSuperimposedGlyphGadgets( enable );
	}
}

bool GlyphViewerWindow::DoesHelpExist() const {

	return SynGlyphX::GlyphBuilderApplication::IsGlyphEd();
}

void GlyphViewerWindow::ClearRecentFileList() {

	SynGlyphX::MainWindow::ClearRecentFileList();
	s_recentFilters.clear();
}

void GlyphViewerWindow::UpdateFilenameWindowTitle(const QString& title) {

	QString prefix;
	if (title != s_noFileName) {

		prefix = title + " - ";
	}

	setWindowTitle(prefix + GetApplicationDisplayName());
}

QString GlyphViewerWindow::GetApplicationDisplayName() const {

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

		//return SynGlyphX::Application::applicationName() + " (" + SynGlyphX::Application::organizationName() + " Inside)";
		return SynGlyphX::Application::organizationName() + " " + SynGlyphX::getFullVersionString().c_str();
	}
	else {

		return SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
	}
}

void GlyphViewerWindow::LoadSettings(QMap<QString, QJsonValue> settings) {
	
	if (settings.contains("userid")) {
		QSettings qsettings;
		qsettings.beginGroup("GlyphX_User");
		qsettings.setValue("userid", settings["userid"].toString());
	}
	if (settings.contains("summation")) {
		summation = settings["summation"].toBool();
	}
	if (settings.contains("explosion")) {
		explosion = settings["explosion"].toBool();
	}
}
