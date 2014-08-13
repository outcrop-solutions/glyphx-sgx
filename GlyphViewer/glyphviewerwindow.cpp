#include "glyphviewerwindow.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDockWidget>
#include "application.h"
#include "datatransform.h"
#include "downloadedmapproperties.h"
#include "networkdownloader.h"
#include "downloadoptionsdialog.h"
#include "data/npmapfile.h"
#include "databaseservices.h"

GlyphViewerWindow::GlyphViewerWindow(QWidget *parent)
	: SynGlyphX::MainWindow(parent)
{
	m_glyphForestModel = new GlyphForestModel(this);
	QDir::current().mkdir("cache");
	m_cacheDir = QDir::toNativeSeparators(QDir::currentPath()) + QDir::separator() + "cache";
	CreateMenus();
	CreateDockWidgets();

	m_antzWidget = new ANTzViewerWidget(m_glyphForestModel, m_treeView->selectionModel(), this);
	setCentralWidget(m_antzWidget);

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);
}

GlyphViewerWindow::~GlyphViewerWindow()
{

}

void GlyphViewerWindow::CreateMenus() {

	//Create File Menu
	m_fileMenu = menuBar()->addMenu(tr("File"));

	QAction* openProjectAction = CreateMenuAction(m_fileMenu, tr("Open Project"), QKeySequence::Open);
	QObject::connect(openProjectAction, &QAction::triggered, this, &GlyphViewerWindow::OpenProject);

	m_fileMenu->addActions(m_recentFileActions);

	m_fileMenu->addSeparator();

	QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
	QObject::connect(exitAction, &QAction::triggered, this, &GlyphViewerWindow::close);

	//Create View Menu
	m_viewMenu = menuBar()->addMenu(tr("View"));
	CreateFullScreenAction(m_viewMenu);

	m_viewMenu->addSeparator();

	m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeMapDownloadSettings);

	m_helpMenu = menuBar()->addMenu(tr("Help"));
	QAction* aboutBoxAction = m_helpMenu->addAction("About " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
	QObject::connect(aboutBoxAction, &QAction::triggered, this, &GlyphViewerWindow::ShowAboutBox);
}

void GlyphViewerWindow::CreateDockWidgets() {

	//Add Tree View to dock widget on left side
	QDockWidget* leftDockWidget = new QDockWidget(tr("Glyphs"), this);
	m_treeView = new QTreeView(leftDockWidget);
	m_treeView->setModel(m_glyphForestModel);
	m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_treeView->setDragEnabled(false);

	leftDockWidget->setWidget(m_treeView);
	addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
	m_viewMenu->addAction(leftDockWidget->toggleViewAction());
}

void GlyphViewerWindow::ShowAboutBox() {

	QString appName = SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
	QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion());
}

void GlyphViewerWindow::OpenProject() {

	QString openFile = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("SynGlyphX Data Transform Project Files (*.sdt)"));
	if (!openFile.isEmpty()) {
		LoadDataTransform(openFile);
	}
}

void GlyphViewerWindow::LoadRecentFile(const QString& filename) {

	LoadDataTransform(filename);
}

void GlyphViewerWindow::LoadDataTransform(const QString& filename) {

	try {

		SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);
		SynGlyphX::DataTransform transform;
		transform.ReadFromFile(filename.toStdString());
		QString cacheSubDir = "cache_" + QString::fromStdWString(boost::uuids::to_wstring(transform.GetID()));
		QString transformCacheDir = m_cacheDir + QDir::separator() + cacheSubDir;

		bool useNonDefaultImage = (transform.GetBaseImage().GetType() != SynGlyphX::BaseImage::Type::Default);

		QDir dir(m_cacheDir);
		if (!dir.exists(cacheSubDir)) {
			if (!dir.mkdir(cacheSubDir)) {
				throw std::exception("Cache directory was not created");
			}
		}

		if (!DoesCacheExist(transformCacheDir, useNonDefaultImage)) {
			ConvertTransformToANTz(transform, transformCacheDir);
		}

		if (useNonDefaultImage) {
			m_glyphForestModel->UseLocalBaseImage(transformCacheDir + QDir::separator() + "map00001.jpg");
		}
		else {
			m_glyphForestModel->UseDefaultBaseImage();
		}

		QStringList files;
		files.push_back(transformCacheDir + QDir::separator() + "antz0001.csv");
		files.push_back(transformCacheDir + QDir::separator() + "antztag0001.csv");

		m_glyphForestModel->LoadANTzFiles(files);
	}
	catch (const std::exception& e) {

		SynGlyphX::Application::restoreOverrideCursor();
		QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + e.what(), QMessageBox::Ok);
		return;
	}

	SetCurrentFile(filename);
	SynGlyphX::Application::restoreOverrideCursor();
	statusBar()->showMessage("Project successfully opened", 3000);
}

bool GlyphViewerWindow::DoesCacheExist(const QString& cacheDir, bool cacheHasImage) const {

	QFile csvFile(cacheDir + QDir::separator() + "antz0001.csv");
	if (!csvFile.exists()) {
		return false;
	}

	QFile tagFile(cacheDir + QDir::separator() + "antztag0001.csv");
	if (!tagFile.exists()) {
		return false;
	}

	if (cacheHasImage) {
		QFile baseImage(cacheDir + QDir::separator() + "map00001.jpg");
		if (!baseImage.exists()) {
			return false;
		}
	}

	return true;
}

void GlyphViewerWindow::ConvertTransformToANTz(SynGlyphX::DataTransform& transform, const QString& cacheDir) {

	DatabaseServices::AddDatabaseConnections(transform.GetDatasources(), transform.GetDatasources().size());

	const SynGlyphX::BaseImage& baseImage = transform.GetBaseImage();

	if (baseImage.GetType() == SynGlyphX::BaseImage::Type::DownloadedMap) {

		std::vector<GeographicPoint> points;
		transform.GetPositionXYForAllGlyphTrees(points);
		QString baseImageFile = cacheDir + QDir::separator() + "map00001.jpg";
		const SynGlyphX::DownloadedMapProperties* const properties = dynamic_cast<const SynGlyphX::DownloadedMapProperties* const>(baseImage.GetProperties());
		NetworkDownloader& downloader = NetworkDownloader::Instance();
		GeographicBoundingBox boundingBox = downloader.DownloadMap(points, baseImageFile.toStdString(), properties);
		transform.SetPositionXYMinMaxToGeographicForAllGlyphTrees(boundingBox);
	}

	QString csvFile = cacheDir + QDir::separator() + "antz0001.csv";
	QString tagFile = cacheDir + QDir::separator() + "antztag0001.csv";
	transform.TransformToCSV(csvFile.toStdString(), tagFile.toStdString());
}

void GlyphViewerWindow::ChangeMapDownloadSettings() {

	DownloadOptionsDialog dialog(this);
	dialog.exec();
}