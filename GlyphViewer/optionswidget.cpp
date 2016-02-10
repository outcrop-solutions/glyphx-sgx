#include "optionswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include "groupboxsinglewidget.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include "filesystem.h"

OptionsWidget::OptionsWidget(const GlyphViewerOptions& options, bool enableCacheOptions, QWidget *parent)
	: QTabWidget(parent)
{
	CreateCacheTab(options, enableCacheOptions);
	Create3DTab(options);

#ifdef USE_ZSPACE
	m_zSpaceOptionsWidget = new SynGlyphX::ZSpaceOptionsWidget(this);
	m_zSpaceOptionsWidget->SetOptions(options.GetZSpaceOptions());
	addTab(m_zSpaceOptionsWidget, tr("zSpace"));
#endif

	CreateUITab(options);
}

OptionsWidget::~OptionsWidget()
{

}

void OptionsWidget::CreateCacheTab(const GlyphViewerOptions& options, bool enableCacheOptions) {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(tab);

	m_cacheDirectoryWidget = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::Directory, true, tab);
	m_cacheDirectoryWidget->setContentsMargins(4, 4, 4, 4);
	m_cacheDirectoryWidget->setEnabled(enableCacheOptions);
	m_cacheDirectoryWidget->SetText(options.GetCacheDirectory());
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

void OptionsWidget::Create3DTab(const GlyphViewerOptions& options) {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(tab);

	m_hideSelectedGlyphsCheckbox = new QCheckBox(tr("Filter View (Hide unselected glyph trees when there is an active selection)"), this);
	m_hideSelectedGlyphsCheckbox->setChecked(options.GetHideUnselectedGlyphTrees());
	layout->addWidget(m_hideSelectedGlyphsCheckbox);

	tab->setLayout(layout);
	addTab(tab, tr("3D"));
}

void OptionsWidget::CreateUITab(const GlyphViewerOptions& options) {

	QWidget* tab = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(tab);

	m_showDownloadedImageErrorMessages = new QCheckBox(tr("Show an error message when an image failed to download"), this);
	m_showDownloadedImageErrorMessages->setChecked(options.GetShowMessageWhenImagesDidNotDownload());
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

#ifdef USE_ZSPACE
	options.SetZSpaceOptions(m_zSpaceOptionsWidget->GetOptions());
#endif

	options.SetShowMessageWhenImagesDidNotDownload(m_showDownloadedImageErrorMessages->isChecked());

	return options;
}