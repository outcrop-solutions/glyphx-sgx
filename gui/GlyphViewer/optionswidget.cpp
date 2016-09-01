#include "optionswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include "groupboxsinglewidget.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include "filesystem.h"
#include "glyphviewerwindow.h"
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include "browselineedit.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGroupBox>

OptionsWidget::OptionsWidget(const GlyphViewerOptions& options, bool enableCacheOptions, QWidget *parent)
	: QTabWidget(parent)
{
	CreateCacheTab(enableCacheOptions);
	Create3DTab();
	CreateFilteringTab();
	CreateUITab();

	SetCacheValues(options);
	Set3DValues(options);
	SetFilteringValues(options);
	SetUIValues(options);
}

OptionsWidget::~OptionsWidget()
{

}

void OptionsWidget::CreateCacheTab(bool enableCacheOptions) {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* tabLayout = new QVBoxLayout(tab);

	m_cacheDirectoryWidget = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::Directory, true, tab);
	m_cacheDirectoryWidget->setContentsMargins(4, 4, 4, 4);
	m_cacheDirectoryWidget->setEnabled(enableCacheOptions);
	SynGlyphX::GroupBoxSingleWidget* cacheDirectoryGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Cache Directory"), m_cacheDirectoryWidget, tab);

	tabLayout->addWidget(cacheDirectoryGroupBox);

	QHBoxLayout* buttonLayout = new QHBoxLayout(tab);

	QPushButton* clearCacheDirectoryButton = new QPushButton(tr("Clear Cache"), tab);
	clearCacheDirectoryButton->setEnabled(enableCacheOptions);
	QObject::connect(clearCacheDirectoryButton, &QPushButton::clicked, this, &OptionsWidget::ClearCache);
	buttonLayout->addWidget(clearCacheDirectoryButton);

	buttonLayout->addStretch(1);

	QPushButton* defaultCacheDirectoryButton = new QPushButton(tr("Set To Default"), tab);
	defaultCacheDirectoryButton->setEnabled(enableCacheOptions);
	QObject::connect(defaultCacheDirectoryButton, &QPushButton::clicked, this, &OptionsWidget::SetToDefaultCacheDirectory);
	buttonLayout->addWidget(defaultCacheDirectoryButton);

	tabLayout->addLayout(buttonLayout);

	tabLayout->addStretch(1);

	tab->setLayout(tabLayout);
	addTab(tab, tr("Cache"));
}

void OptionsWidget::Create3DTab() {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* tabLayout = new QVBoxLayout(tab);

	QHBoxLayout* sceneAxisOuterLayout = new QHBoxLayout(tab);

	QGroupBox* hudGroupBox = new QGroupBox(tr("HUD Axis Info"), tab);
	QHBoxLayout* hudAxisInfoLayout = new QHBoxLayout(tab);
	
	m_showHUDAxisInfoObjectCheckBox = new QCheckBox(tr("Show"), tab);
	hudAxisInfoLayout->addWidget(m_showHUDAxisInfoObjectCheckBox);

	hudAxisInfoLayout->addWidget(new QLabel(tr("Location:"), tab));

	m_axisObjectLocationComboBox = new QComboBox(tab);
	QStringList locations;
	locations << tr("Top Left") << tr("Bottom Left") << tr("Bottom Right");
	m_axisObjectLocationComboBox->addItems(locations);
	hudAxisInfoLayout->addWidget(m_axisObjectLocationComboBox);

	hudGroupBox->setLayout(hudAxisInfoLayout);
	sceneAxisOuterLayout->addWidget(hudGroupBox);

	QGroupBox* sceneGroupBox = new QGroupBox(tr("Scene Axis Info"), tab);
	QHBoxLayout* sceneAxisInfoLayout = new QHBoxLayout(tab);

	m_showSceneAxisInfoObjectCheckBox = new QCheckBox(tr("Show"), tab);
	sceneAxisInfoLayout->addWidget(m_showSceneAxisInfoObjectCheckBox);

	sceneGroupBox->setLayout(sceneAxisInfoLayout);
	sceneAxisOuterLayout->addWidget(sceneGroupBox);

	sceneAxisOuterLayout->addStretch(1);
	tabLayout->addLayout(sceneAxisOuterLayout);

#ifdef USE_ZSPACE
	QHBoxLayout* zSpaceLayout = new QHBoxLayout(tab);
	m_zSpaceOptionsWidget = new SynGlyphX::ZSpaceOptionsWidget(this);
	m_zSpaceOptionsWidget->layout()->setContentsMargins(0, 0, 0, 0);
	zSpaceLayout->addWidget(m_zSpaceOptionsWidget);
	zSpaceLayout->addStretch(1);
	tabLayout->addLayout(zSpaceLayout);
#endif

	tabLayout->addStretch(1);

	tab->setLayout(tabLayout);
	addTab(tab, tr("3D"));

	QObject::connect(m_showHUDAxisInfoObjectCheckBox, &QCheckBox::toggled, m_axisObjectLocationComboBox, &QComboBox::setEnabled);
}

void OptionsWidget::CreateFilteringTab() {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* tabLayout = new QVBoxLayout(tab);

	m_hideSelectedGlyphsCheckbox = new QCheckBox(tr("Hide Filtered"), this);
	tabLayout->addWidget(m_hideSelectedGlyphsCheckbox);

	QHBoxLayout* filterAlphaLayout = new QHBoxLayout( tab );
	filterAlphaLayout->addWidget( new QLabel( tr( "Filtered Glyph Opacity:" ), tab ) );
	m_filterAlphaSlider = new QSlider( Qt::Orientation::Horizontal, this );
	m_filterAlphaSlider->setMinimum(0);
	m_filterAlphaSlider->setMaximum(100);
	m_filterAlphaSlider->setTickInterval(10);
	filterAlphaLayout->addWidget( m_filterAlphaSlider );
	tabLayout->addLayout( filterAlphaLayout );
	
	QGroupBox* createSubsetGroupBox = new QGroupBox(tr("Create Subset Options"), tab);
	QHBoxLayout* createSubsetLayout = new QHBoxLayout(tab);

	m_loadSubsetVisualizationCheckBox = new QCheckBox(tr("Load after subset is created"), tab);
	createSubsetLayout->addWidget(m_loadSubsetVisualizationCheckBox);

	m_loadSubsetVisualizationInNewInstanceCheckBox = new QCheckBox(tr("Load subset into new Glyph Viewer"), tab);
	createSubsetLayout->addWidget(m_loadSubsetVisualizationInNewInstanceCheckBox);

	createSubsetGroupBox->setLayout(createSubsetLayout);
	tabLayout->addWidget(createSubsetGroupBox);

	tabLayout->addStretch(1);

	tab->setLayout(tabLayout);
	addTab(tab, tr("Filtering"));

	QObject::connect(m_loadSubsetVisualizationCheckBox, &QCheckBox::toggled, m_loadSubsetVisualizationInNewInstanceCheckBox, &QCheckBox::setEnabled);
}

void OptionsWidget::CreateUITab() {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* tabLayout = new QVBoxLayout(tab);

	m_showDownloadedImageErrorMessages = new QCheckBox(tr("Show an error message when an image failed to download"), this);
	tabLayout->addWidget(m_showDownloadedImageErrorMessages);

	QGroupBox* homePageGroupBox = new QGroupBox(tr("Home Page"), tab);
	QVBoxLayout* homePageLayout = new QVBoxLayout(tab);

	m_showHomePage = new QCheckBox(tr("Show home page on startup"), this);
	homePageLayout->addWidget(m_showHomePage);
	QHBoxLayout* clearListsLayout = new QHBoxLayout(tab);
	homePageLayout->addLayout(clearListsLayout);
	m_clearRecentListButton = new QPushButton(tr("Clear Recent Views"), tab);
	clearListsLayout->addWidget(m_clearRecentListButton);
	QObject::connect(m_clearRecentListButton, &QPushButton::clicked, this, &OptionsWidget::OnClearRecentViews);

	m_clearSubsetListButton = new QPushButton(tr("Clear My Views"), tab);
	clearListsLayout->addWidget(m_clearSubsetListButton);
	QObject::connect(m_clearSubsetListButton, &QPushButton::clicked, this, &OptionsWidget::OnClearSubsetViews);

	homePageGroupBox->setLayout(homePageLayout);

	tabLayout->addWidget(homePageGroupBox);

	tabLayout->addStretch(1);

	tab->setLayout(tabLayout);
	addTab(tab, tr("UI"));
}

void OptionsWidget::ClearCache() {

	QDir cacheDir(m_cacheDirectoryWidget->GetText());
	if (cacheDir.exists()) {

		try {

			SynGlyphX::Filesystem::RemoveContentsOfDirectory(QDir::toNativeSeparators(cacheDir.canonicalPath()).toStdString());
			QMessageBox::information(this, tr("Cache cleared"), tr("Cache is now empty."));
		}
		catch (const std::exception& e) {

			QMessageBox::critical(this, tr("Cache clear error"), tr("Failed to clear cache: ") + e.what());
		}
	}
}

void OptionsWidget::SetToDefaultCacheDirectory() {

	m_cacheDirectoryWidget->SetText(GlyphViewerOptions::GetDefaultCacheDirectory());
}

GlyphViewerOptions OptionsWidget::GetOptions() const {

	GlyphViewerOptions options;

	//Cache
	options.SetCacheDirectory(m_cacheDirectoryWidget->GetText());
	
	//3D
	options.SetShowHUDAxisObject(m_showHUDAxisInfoObjectCheckBox->isChecked());
	options.SetHUDAxisObjectLocation(static_cast<SynGlyphXANTz::ANTzForestWidget::HUDLocation>(m_axisObjectLocationComboBox->currentIndex()));

	options.SetShowSceneAxisObject(m_showSceneAxisInfoObjectCheckBox->isChecked());

#ifdef USE_ZSPACE
	options.SetZSpaceOptions(m_zSpaceOptionsWidget->GetOptions());
#endif

	//Filtering
	options.SetHideUnselectedGlyphTrees(m_hideSelectedGlyphsCheckbox->isChecked());
	options.SetLoadSubsetVisualization(m_loadSubsetVisualizationCheckBox->isChecked());
	options.SetLoadSubsetVisualizationInNewInstance(m_loadSubsetVisualizationInNewInstanceCheckBox->isChecked());
	options.SetFilteredGlyphOpacity(m_filterAlphaSlider->value() / 100.f);

	//UI
	options.SetShowMessageWhenImagesDidNotDownload(m_showDownloadedImageErrorMessages->isChecked());
	options.SetShowHomePage(m_showHomePage->isChecked());

	return options;
}

void OptionsWidget::SetCacheValues(const GlyphViewerOptions& options) {

	m_cacheDirectoryWidget->SetText(options.GetCacheDirectory());
}

void OptionsWidget::Set3DValues(const GlyphViewerOptions& options) {

	m_showHUDAxisInfoObjectCheckBox->setChecked(options.GetShowHUDAxisObject());
	m_axisObjectLocationComboBox->setEnabled(options.GetShowHUDAxisObject());
	m_axisObjectLocationComboBox->setCurrentIndex(options.GetHUDAxisObjectLocation());

	m_showSceneAxisInfoObjectCheckBox->setChecked(options.GetShowSceneAxisObject());

#ifdef USE_ZSPACE
	m_zSpaceOptionsWidget->SetOptions(options.GetZSpaceOptions());
#endif
}

void OptionsWidget::SetFilteringValues(const GlyphViewerOptions& options) {

	m_hideSelectedGlyphsCheckbox->setChecked(options.GetHideUnselectedGlyphTrees());
	m_loadSubsetVisualizationCheckBox->setChecked(options.GetLoadSubsetVisualization());
	m_loadSubsetVisualizationInNewInstanceCheckBox->setChecked(options.GetLoadSubsetVisualizationInNewInstance());
	m_filterAlphaSlider->setValue( int( options.GetFilteredGlyphOpacity() * 100.f ) );
}

void OptionsWidget::SetUIValues(const GlyphViewerOptions& options) {

	m_showDownloadedImageErrorMessages->setChecked(options.GetShowMessageWhenImagesDidNotDownload());
	m_showHomePage->setChecked(options.GetShowHomePage());
	m_clearRecentListButton->setEnabled(!GlyphViewerWindow::GetRecentFileListInstance().GetFiles().isEmpty());
	m_clearSubsetListButton->setEnabled(!GlyphViewerWindow::GetSubsetFileListInstance().GetFiles().isEmpty());
}

void OptionsWidget::OnClearRecentViews() {

	GlyphViewerWindow::ClearRecentFileList();
	QMessageBox::information(this, tr("Recent Views Cleared"), tr("Recent views are cleared."));
	m_clearRecentListButton->setEnabled(false);
}

void OptionsWidget::OnClearSubsetViews() {

	GlyphViewerWindow::ClearSubsetFileList();
	QMessageBox::information(this, tr("My Views Cleared"), tr("My views are cleared."));
	m_clearSubsetListButton->setEnabled(false);
}