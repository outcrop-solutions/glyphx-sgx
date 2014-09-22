#include "glyphviewerwindow.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDockWidget>
#include "application.h"
#include "datatransformmapping.h"
#include "downloadoptionsdialog.h"
#include "data/npmapfile.h"
#include "databaseservices.h"
#include "glyphviewerantztransformer.h"
#include "changedatasourcefiledialog.h"

GlyphViewerWindow::GlyphViewerWindow(QWidget *parent)
	: SynGlyphX::MainWindow(parent)
{
	m_glyphForestModel = new GlyphForestModel(this);
	QDir::current().mkdir("cache");
	m_cacheDir = QDir::toNativeSeparators(QDir::currentPath()) + QDir::separator() + "cache";
	CreateMenus();
	CreateDockWidgets();
	
	try {
		m_antzWidget = new ANTzViewerWidget(m_glyphForestModel, m_treeView->selectionModel(), this);
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, tr("3D view error"), tr("3D view failed to create: ") + e.what());
		throw;
	}

	setCentralWidget(m_antzWidget);

	QObject::connect(m_antzWidget, &ANTzViewerWidget::NewStatusMessage, statusBar(), &QStatusBar::showMessage);

	m_stereoAction->setChecked(m_antzWidget->IsInStereoMode());

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

	m_stereoAction = CreateMenuAction(m_viewMenu, tr("Stereo"));
	m_stereoAction->setCheckable(true);
	QObject::connect(m_stereoAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeStereoMode);

	m_viewMenu->addSeparator();
	CreateFullScreenAction(m_viewMenu);

	m_viewMenu->addSeparator();

	m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeMapDownloadSettings);

	m_helpMenu = menuBar()->addMenu(tr("Help"));

	QAction* openSettingsAction = m_helpMenu->addAction(tr("OpenGL Settings"));
	QObject::connect(openSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ShowOpenGLSettings);

	m_helpMenu->addSeparator();

	QAction* aboutBoxAction = m_helpMenu->addAction(tr("About ") + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
	QObject::connect(aboutBoxAction, &QAction::triggered, this, &GlyphViewerWindow::ShowAboutBox);
}

void GlyphViewerWindow::CreateDockWidgets() {

	//Add Tree View to dock widget on left side
	QDockWidget* leftDockWidget = new QDockWidget(tr("Glyph List"), this);
	m_treeView = new GlyphTreeListView(leftDockWidget);
	m_treeView->setModel(m_glyphForestModel);

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

	if (filename == m_currentFilename) {
		return;
	}

	if (!m_treeView->selectionModel()->selectedIndexes().empty()) {

		m_treeView->selectionModel()->clearSelection();
		m_antzWidget->updateGL();
	}

	try {

		SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);
		SynGlyphX::DataTransformMapping mapping;
		mapping.ReadFromFile(filename.toStdString());

		bool wereDatasourcesUpdated = false;

		std::vector<SynGlyphX::DataTransformMapping::DatasourceMap::const_iterator> datasourcesToBeUpdated;
		for (SynGlyphX::DataTransformMapping::DatasourceMap::const_iterator datasource = mapping.GetDatasources().begin(); datasource != mapping.GetDatasources().end(); ++datasource) {

			if (!datasource->second.CanDatasourceBeFound()) {

				datasourcesToBeUpdated.push_back(datasource);
			}
		}

		SynGlyphX::Application::restoreOverrideCursor();
		for (int i = 0; i < datasourcesToBeUpdated.size(); ++i) {

			QString acceptButtonText = tr("Next");
			if (i == datasourcesToBeUpdated.size() - 1) {
				acceptButtonText = tr("Ok");
			}

			ChangeDatasourceFileDialog dialog(datasourcesToBeUpdated[i]->second, acceptButtonText, this);
			if (dialog.exec() == QDialog::Accepted) {

				mapping.UpdateDatasourceName(datasourcesToBeUpdated[i]->first, dialog.GetNewDatasourceFile().toStdWString());
				wereDatasourcesUpdated = true;
			}
			else {
				throw std::exception("One or more datasources weren't found.");
			}
		}
		SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);

		if (wereDatasourcesUpdated) {

			mapping.WriteToFile(filename.toStdString(), false);
		}

		GlyphViewerANTzTransformer transformer(m_cacheDir);
		transformer.Transform(mapping);

		m_glyphForestModel->LoadANTzFiles(transformer.GetCSVFilenames());

		//Base image must come after loading of CSV files
		const QString& baseImageFilename = transformer.GetBaseImageFilename();
		if (baseImageFilename.isEmpty()) {

			m_glyphForestModel->UseDefaultBaseImage();
		}
		else {
			
			m_glyphForestModel->UseLocalBaseImage(baseImageFilename);
		}
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

void GlyphViewerWindow::ChangeMapDownloadSettings() {

	DownloadOptionsDialog dialog(this);
	dialog.exec();
}

void GlyphViewerWindow::ShowOpenGLSettings() {

	const QGLFormat& format = m_antzWidget->context()->format();
	QString settings = tr("OpenGL Version = ") + QString::number(format.majorVersion()) + "." + QString::number(format.minorVersion()) + '\n';
		
	settings += tr("Stereo Support") + " = ";
	if (m_antzWidget->IsStereoSupported()) {
		settings += tr("enabled");
	}
	else {
		settings += tr("disabled");
	}
	QMessageBox::information(this, tr("OpenGL Settings"), settings);
}

void GlyphViewerWindow::ChangeStereoMode() {

	if (m_antzWidget->IsStereoSupported()) {

		m_antzWidget->SetStereo(!m_antzWidget->IsInStereoMode());
	}
	else {

		m_stereoAction->setChecked(false);
		QMessageBox::information(this, tr("Stereo not supported"), tr("Stereo is not supported. Check your driver settings to see if stereo is enabled or contact the manufacturer of your GPU for assitance"));
	}
}