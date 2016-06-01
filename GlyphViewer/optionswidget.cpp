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

OptionsWidget::OptionsWidget(const GlyphViewerOptions& options, bool enableCacheOptions, QWidget *parent)
	: QTabWidget(parent)
{
	CreateCacheTab(enableCacheOptions);
	Create3DTab();

#ifdef USE_ZSPACE
	m_zSpaceOptionsWidget = new SynGlyphX::ZSpaceOptionsWidget(this);
	m_zSpaceOptionsWidget->SetOptions(options.GetZSpaceOptions());
	addTab(m_zSpaceOptionsWidget, tr("zSpace"));
#endif

	CreateUITab();

	SetCacheValues(options);
	Set3DValues(options);
	SetUIValues(options);
}

OptionsWidget::~OptionsWidget()
{

}

void OptionsWidget::CreateCacheTab(bool enableCacheOptions) {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(tab);

	m_cacheDirectoryWidget = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::Directory, true, tab);
	m_cacheDirectoryWidget->setContentsMargins(4, 4, 4, 4);
	m_cacheDirectoryWidget->setEnabled(enableCacheOptions);
	SynGlyphX::GroupBoxSingleWidget* cacheDirectoryGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Cache Directory"), m_cacheDirectoryWidget, tab);

	layout->addWidget(cacheDirectoryGroupBox);

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

	layout->addLayout(buttonLayout);

	tab->setLayout(layout);
	addTab(tab, tr("Cache"));
}

void OptionsWidget::Create3DTab() {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* tabLayout = new QVBoxLayout(tab);

	m_hideSelectedGlyphsCheckbox = new QCheckBox(tr("Filter View (Hide unselected glyph trees when there is an active selection)"), this);
	tabLayout->addWidget(m_hideSelectedGlyphsCheckbox);

	QHBoxLayout* hudOuterLayout = new QHBoxLayout(tab);

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
	hudOuterLayout->addWidget(hudGroupBox);

	hudOuterLayout->addStretch(1);
	tabLayout->addLayout(hudOuterLayout);

	tab->setLayout(tabLayout);
	addTab(tab, tr("3D"));

	QObject::connect(m_showHUDAxisInfoObjectCheckBox, &QCheckBox::toggled, m_axisObjectLocationComboBox, &QComboBox::setEnabled);
}

void OptionsWidget::CreateUITab() {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(tab);

	m_showDownloadedImageErrorMessages = new QCheckBox(tr("Show an error message when an image failed to download"), this);
	layout->addWidget(m_showDownloadedImageErrorMessages);

	tab->setLayout(layout);
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
	options.SetCacheDirectory(m_cacheDirectoryWidget->GetText());
	options.SetHideUnselectedGlyphTrees(m_hideSelectedGlyphsCheckbox->isChecked());
	options.SetShowSceneAxisHUDObject(m_showHUDAxisInfoObjectCheckBox->isChecked());
	options.SetSceneAxisObjectLocation(static_cast<SynGlyphXANTz::ANTzForestWidget::HUDLocation>(m_axisObjectLocationComboBox->currentIndex()));

#ifdef USE_ZSPACE
	options.SetZSpaceOptions(m_zSpaceOptionsWidget->GetOptions());
#endif

	options.SetShowMessageWhenImagesDidNotDownload(m_showDownloadedImageErrorMessages->isChecked());

	return options;
}

void OptionsWidget::SetCacheValues(const GlyphViewerOptions& options) {

	m_cacheDirectoryWidget->SetText(options.GetCacheDirectory());
}

void OptionsWidget::Set3DValues(const GlyphViewerOptions& options) {

	m_hideSelectedGlyphsCheckbox->setChecked(options.GetHideUnselectedGlyphTrees());
	m_showHUDAxisInfoObjectCheckBox->setChecked(options.GetShowSceneAxisHUDObject());
	m_axisObjectLocationComboBox->setEnabled(options.GetShowSceneAxisHUDObject());
	m_axisObjectLocationComboBox->setCurrentIndex(options.GetSceneAxisObjectLocation());
}

void OptionsWidget::SetUIValues(const GlyphViewerOptions& options) {

	m_showDownloadedImageErrorMessages->setChecked(options.GetShowMessageWhenImagesDidNotDownload());
}