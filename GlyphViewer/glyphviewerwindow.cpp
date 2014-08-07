#include "glyphviewerwindow.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include "application.h"
#include "datatransform.h"
#include "downloadedmapproperties.h"
#include "networkdownloader.h"

GlyphViewerWindow::GlyphViewerWindow(QWidget *parent)
	: SynGlyphX::MainWindow(parent)
{
	m_cacheDir = QDir::currentPath() + QDir::separator() + "cache";
	CreateMenus();
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

	//m_viewMenu->addSeparator();

	m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeMapDownloadSettings);

	m_helpMenu = menuBar()->addMenu(tr("Help"));
	QAction* aboutBoxAction = m_helpMenu->addAction("About " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
	QObject::connect(aboutBoxAction, &QAction::triggered, this, &GlyphViewerWindow::ShowAboutBox);
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
		SynGlyphX::DataTransform transform;
		transform.ReadFromFile(filename.toStdString());
		ConvertTransformToANTz(transform);
	}
	catch (const std::exception& e) {
		QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + e.what(), QMessageBox::Ok);
		return;
	}

	SetCurrentFile(filename);

	statusBar()->showMessage("Project successfully opened", 3000);
}

void GlyphViewerWindow::ConvertTransformToANTz(SynGlyphX::DataTransform& transform) {

	QString transformCacheDir = m_cacheDir;
	if () {

		SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);
		const SynGlyphX::BaseImage& baseImage = transform.GetBaseImage();

		if (baseImage.GetType() == SynGlyphX::BaseImage::Type::DownloadedMap) {

			std::vector<GeographicPoint> points;
			transform.GetPositionXYForAllGlyphTrees(points);
			QString baseImageFile = transformCacheDir + QDir::separator() + "map00001.jpg";
			const SynGlyphX::DownloadedMapProperties* const properties = dynamic_cast<const SynGlyphX::DownloadedMapProperties* const>(baseImage.GetProperties());
			NetworkDownloader& downloader = NetworkDownloader::Instance();
			GeographicBoundingBox boundingBox = downloader.DownloadMap(points, baseImageFile.toStdString(), properties);
			transform.SetPositionXYMinMaxToGeographicForAllGlyphTrees(boundingBox);
		}

		QString csvFile = transformCacheDir + QDir::separator() + "antz0001.csv";
		QString tagFile = transformCacheDir + QDir::separator() + "antztag0001.csv";
		transform.TransformToCSV(csvFile.toStdString(), tagFile.toStdString());

		SynGlyphX::Application::restoreOverrideCursor();
		statusBar()->showMessage("Data transform sucessfully converted", 3000);
	}
}