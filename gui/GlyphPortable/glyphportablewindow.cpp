#include "glyphportablewindow.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QStackedWidget>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QSettings>
#include <QtWidgets/QDockWidget>
#include <QtCore/QDateTime>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QColorDialog>
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

GlyphPortableWindow::GlyphPortableWindow(QWidget *parent)
	: SynGlyphX::MainWindow(4, parent),
	m_showErrorFromTransform(true),
	m_showHomePage(true),
    m_viewer( nullptr )
{
	SGX_PROFILE_SCOPE
	CreateMenus();
	CreateDockWidgets();

	QStackedWidget* centerWidgetsContainer = new QStackedWidget(this);
	centerWidgetsContainer->setContentsMargins(0, 0, 0, 0);
	centerWidgetsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setCentralWidget(centerWidgetsContainer);

	try {
		
		CreateANTzWidget();
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

	CreateInteractionToolbar();

	centerWidgetsContainer->setCurrentIndex(1);

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);

	EnableLoadedVisualizationDependentActions( true );

	UpdateFilenameWindowTitle(s_noFileName);

	std::vector<std::string> images;
}

GlyphPortableWindow::~GlyphPortableWindow()
{

}

void GlyphPortableWindow::CreateANTzWidget() {
	
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
	QObject::connect( m_enableDisableSelEffectActionMenu, &QAction::toggled, this, &GlyphPortableWindow::OnEnableDisableSelEffect );

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
	} );
}

void GlyphPortableWindow::CreateMenus() {

	//Create File Menu
	m_fileMenu = menuBar()->addMenu(tr("File"));

	QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
	QObject::connect(exitAction, &QAction::triggered, this, &GlyphPortableWindow::close);

	//Create View Menu
	CreateViewMenu();

	m_stereoAction = CreateMenuAction(m_viewMenu, tr("Stereo"));
	m_stereoAction->setCheckable(true);
	QObject::connect(m_stereoAction, &QAction::triggered, this, &GlyphPortableWindow::ChangeStereoMode);

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
	resetCamIcon.addFile( ":SGXGUI/Resources/Icons/icon-reset-camera.png", QSize(), QIcon::Normal, QIcon::On );
	m_resetCameraToDefaultPosition->setIcon( resetCamIcon );
	m_loadedVisualizationDependentActions.push_back(m_resetCameraToDefaultPosition);

	m_viewMenu->addSeparator();

	CreateHelpMenu();

	QAction* openGLSettingsAction = new QAction(tr("OpenGL Settings"), m_helpMenu);
	QObject::connect(openGLSettingsAction, &QAction::triggered, this, &GlyphPortableWindow::ShowOpenGLSettings);

	m_helpMenu->insertAction(m_aboutBoxAction, openGLSettingsAction);
	m_helpMenu->insertSeparator(m_aboutBoxAction);

#ifdef __APPLE__
	menuBar()->addSeparator();
#endif
	SynGlyphX::MainWindow::CreateLoginMenu();

}

void GlyphPortableWindow::CreateDockWidgets() {

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
}

void GlyphPortableWindow::ShowOpenGLSettings() {

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

void GlyphPortableWindow::ChangeStereoMode() {

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

void GlyphPortableWindow::EnableLoadedVisualizationDependentActions(bool enable) {

	for (int i = 0; i < m_loadedVisualizationDependentActions.length(); ++i) {

		m_loadedVisualizationDependentActions[i]->setEnabled(enable);
	}

	m_stereoAction->setEnabled(!enable);

	m_clearSelectionAction->setEnabled(false);
	m_showTagsAction->setEnabled(false);
	m_hideTagsAction->setEnabled(false);
}

void GlyphPortableWindow::ChangeBackgroundColor() {
	QColor color = QColorDialog::getColor(Qt::black, this);
	if (color.isValid())
	{
		auto r = color.redF();
		auto g = color.greenF();
		auto b = color.blueF();
		if (m_viewer) m_viewer->setBackgroundColor(glm::vec4(float(r), float(g), float(b), 1.f));
	}
}

void GlyphPortableWindow::closeEvent(QCloseEvent* event) {

	SynGlyphX::MainWindow::closeEvent(event);
}

void GlyphPortableWindow::CreateInteractionToolbar() {

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
	QObject::connect(m_showHideHUDAxisAction, &QAction::toggled, this, &GlyphPortableWindow::OnShowHideHUDAxis);
	m_interactionToolbar->addAction(m_showHideHUDAxisAction);
	m_loadedVisualizationDependentActions.push_back(m_showHideHUDAxisAction);

	m_showHideSceneAxisAction = new QAction(tr("Show/Hide Scene Axis"), m_interactionToolbar);
	m_showHideSceneAxisAction->setCheckable(true);
	QIcon sceneAxisIcon;
	sceneAxisIcon.addFile(":SGXGUI/Resources/Icons/icon-axis-scene.png", QSize(), QIcon::Normal, QIcon::Off);
	sceneAxisIcon.addFile(":SGXGUI/Resources/Icons/icon-axis-scene-a.png", QSize(), QIcon::Normal, QIcon::On);
	m_showHideSceneAxisAction->setIcon(sceneAxisIcon);
	//Replace with scene viewer function when available
	QObject::connect(m_showHideSceneAxisAction, &QAction::toggled, this, &GlyphPortableWindow::OnShowHideSceneAxis);
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
	QObject::connect( m_enableDisableFlyToObjectAction, &QAction::toggled, this, &GlyphPortableWindow::OnEnableDisableFlyToObjectAction );
	m_interactionToolbar->addAction( m_enableDisableFlyToObjectAction );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableFlyToObjectAction );

	m_enableDisableFreeSelectionCameraAction = new QAction( tr( "Enable/Disable Orbit Selection" ), m_interactionToolbar );
	m_enableDisableFreeSelectionCameraAction->setCheckable( true );
	m_enableDisableFreeSelectionCameraAction->setChecked( true );
	QIcon freeSelCamIcon;
	freeSelCamIcon.addFile( ":SGXGUI/Resources/Icons/icon-orbit.png", QSize(), QIcon::Normal, QIcon::Off );
	freeSelCamIcon.addFile( ":SGXGUI/Resources/Icons/icon-orbit-a.png", QSize(), QIcon::Normal, QIcon::On );
	m_enableDisableFreeSelectionCameraAction->setIcon( freeSelCamIcon );
	QObject::connect( m_enableDisableFreeSelectionCameraAction, &QAction::toggled, this, &GlyphPortableWindow::OnEnableDisableFreeSelectionCamera );
	m_interactionToolbar->addAction( m_enableDisableFreeSelectionCameraAction );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableFreeSelectionCameraAction );

	m_enableDisableSelEffectAction = new QAction( tr( "Enable/Disable Selection Effect" ), m_interactionToolbar );
	m_enableDisableSelEffectAction->setCheckable( true );
	m_enableDisableSelEffectAction->setChecked( true );
	QIcon selEffIcon;
	selEffIcon.addFile( ":SGXGUI/Resources/Icons/icon-blink.png", QSize(), QIcon::Normal, QIcon::Off );
	selEffIcon.addFile( ":SGXGUI/Resources/Icons/icon-blink-a.png", QSize(), QIcon::Normal, QIcon::On );
	m_enableDisableSelEffectAction->setIcon( selEffIcon );
	QObject::connect( m_enableDisableSelEffectAction, &QAction::toggled, this, &GlyphPortableWindow::OnEnableDisableSelEffect );
	m_interactionToolbar->addAction( m_enableDisableSelEffectAction );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableSelEffectAction );

	m_enableDisableSuperimposedGlyphGadgets = new QAction( tr( "Enable/Disable Overlapping Group Indicators" ), m_interactionToolbar );
	m_enableDisableSuperimposedGlyphGadgets->setCheckable( true );
	m_enableDisableSuperimposedGlyphGadgets->setChecked( false );
	QIcon sggIcon;
	sggIcon.addFile( ":SGXGUI/Resources/Icons/icon-show-groups.png", QSize(), QIcon::Normal, QIcon::Off );
	sggIcon.addFile( ":SGXGUI/Resources/Icons/icon-show-groups-a.png", QSize(), QIcon::Normal, QIcon::On );
	m_enableDisableSuperimposedGlyphGadgets->setIcon( sggIcon );
	QObject::connect( m_enableDisableSuperimposedGlyphGadgets, &QAction::toggled, this, &GlyphPortableWindow::OnEnableDisableSuperimposedGadgets );
	m_interactionToolbar->addAction( m_enableDisableSuperimposedGlyphGadgets );
	m_loadedVisualizationDependentActions.push_back( m_enableDisableSuperimposedGlyphGadgets );
}

void GlyphPortableWindow::OnStereoSetup(bool stereoEnabled) {

	m_stereoAction->setChecked(stereoEnabled);
}

void GlyphPortableWindow::OnShowHideHUDAxis(bool show) {

	m_viewer->enableHUDAxes( show );
}

void GlyphPortableWindow::OnShowHideSceneAxis(bool show) {

	m_viewer->enableSceneAxes( show );
}

void GlyphPortableWindow::OnEnableDisableFlyToObjectAction( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
		m_viewer->enableFlyToObject( enable );
}

void GlyphPortableWindow::OnEnableDisableFreeSelectionCamera( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
		m_viewer->enableFreeSelectionCamera( !enable );
}

void GlyphPortableWindow::OnEnableDisableSelEffect( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
	{
		m_viewer->enableSelectionEffect( enable );
		m_enableDisableSelEffectActionMenu->setChecked( enable );
		m_enableDisableSelEffectAction->setChecked( enable );
	}
}

void GlyphPortableWindow::OnEnableDisableSuperimposedGadgets( bool enable )
{
	if ( m_viewer && m_viewer->isInitialized() )
	{
		m_viewer->enableSuperimposedGlyphGadgets( enable );
	}
}

bool GlyphPortableWindow::DoesHelpExist() const {

	return SynGlyphX::GlyphBuilderApplication::IsGlyphEd();
}

void GlyphPortableWindow::UpdateFilenameWindowTitle(const QString& title) {

	QString prefix;
	if (title != s_noFileName) {

		prefix = title + " - ";
	}

	setWindowTitle(prefix + GetApplicationDisplayName());
}

QString GlyphPortableWindow::GetApplicationDisplayName() const {

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

		return SynGlyphX::Application::applicationName() + " (Powered by " + SynGlyphX::Application::organizationName() + ")";
	}
	else {

		return SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
	}
}
