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
	layout->addLayout(buttonLayout);

	tab->setLayout(layout);
	addTab(tab, tr("Cache"));
}

void OptionsWidget::ClearCache() {

	QDir cacheDir(m_cacheDirectoryWidget->GetText());
	if (cacheDir.exists()) {

		SynGlyphX::Filesystem::RemoveContentsOfDirectory(QDir::toNativeSeparators(cacheDir.canonicalPath()).toStdString());
		QMessageBox::information(this, tr("Cache cleared"), tr("Cache is now empty."));
	}
}

GlyphViewerOptions OptionsWidget::GetOptions() const {

	GlyphViewerOptions options;
	options.SetCacheDirectory(m_cacheDirectoryWidget->GetText());

	return options;
}