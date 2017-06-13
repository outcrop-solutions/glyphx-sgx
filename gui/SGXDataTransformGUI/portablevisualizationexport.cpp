#include "portablevisualizationexport.h"
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include "glyphbuilderapplication.h"
#include "filesystem.h"
#include <iostream>

namespace SynGlyphX {

	PortableVisualizationExport::PortableVisualizationExport()
	{
		SetupSourceDirectories();
	}

	PortableVisualizationExport::~PortableVisualizationExport()
	{

	}

	void PortableVisualizationExport::CreateSubmenu(QMenu* menu) {

		if (DoesPlatformHaveSourceDirectory(Platform::Windows)) {

			QAction* windowsAction = menu->addAction(tr("Windows"));
			QObject::connect(windowsAction, &QAction::triggered, this, [this]{ emit CreatePortableVisualization(Platform::Windows); });
		}

		if (DoesPlatformHaveSourceDirectory(Platform::Mac)) {

			QAction* macAction = menu->addAction(tr("Mac"));
			QObject::connect(macAction, &QAction::triggered, this, [this]{ emit CreatePortableVisualization(Platform::Mac); });
		}
	}

	void PortableVisualizationExport::SetupSourceDirectories() {

		QString sourceBaseDir = QDir::toNativeSeparators(QFileInfo(SynGlyphX::Application::applicationDirPath()).canonicalFilePath()) + QDir::separator();
		
		QSettings settings;
		settings.beginGroup("ExportPortable");
		AddSourceDirectoryToPlatformIfItExists(Platform::Windows, settings.value("windows", sourceBaseDir + "GlyphPortableWin64").toString());
		AddSourceDirectoryToPlatformIfItExists(Platform::Mac, settings.value("mac", sourceBaseDir + "GlyphPortableOSX").toString());
		settings.endGroup();
	}

	const QString& PortableVisualizationExport::GetSourceDirectory(Platform platform) const {

		if (m_sourceDirectories.count(platform) == 0) {

			throw std::runtime_error("Platform does not exist");
		}

		return m_sourceDirectories.at(platform);
	}

	bool PortableVisualizationExport::DoesPlatformHaveSourceDirectory(Platform platform) const {

		return (m_sourceDirectories.count(platform) == 1);
	}

	bool PortableVisualizationExport::DoAnyPlatformsHaveSourceDirectories() const {

		return (m_sourceDirectories.size() > 0);
	}

	void PortableVisualizationExport::AddSourceDirectoryToPlatformIfItExists(Platform platform, const QString& directoryName) {
		if (DoesSourceDirectoryExist(directoryName)) {

			m_sourceDirectories[platform] = directoryName;
		}
	}

	bool PortableVisualizationExport::DoesSourceDirectoryExist(const QString directoryName) const {

		QDir baseSourceDir(directoryName);
		if (!baseSourceDir.exists() || (baseSourceDir.count() == 0)) {

			return false;
		}

		return true;
	}

	void PortableVisualizationExport::CopyContentsOfSourceDirectory(Platform platform, const QString& destinationDir) const {

		QDir destDir(destinationDir);
		if (destDir.exists()) {

			Filesystem::RemoveContentsOfDirectory(destinationDir.toStdString());
		}
		else {

			destDir.mkpath(destinationDir);
		}

		Filesystem::CopyDirectoryOverwrite(m_sourceDirectories.at(platform).toStdString(), destinationDir.toStdString(), true);
		if ((platform == Platform::Windows)) {

			QString appPath = SynGlyphX::Application::applicationDirPath() + QDir::separator();

			QFile::copy(appPath + "msvcp120.dll", destinationDir + "/msvcp120.dll");
			QFile::copy(appPath + "msvcr120.dll", destinationDir + "/msvcr120.dll");
			QFile::copy(appPath + "vccorlib120.dll", destinationDir + "/vccorlib120.dll");
		}
        
#ifdef __APPLE__
        // On OSX the exectuable won't automatically have the execute permission bit set. Change it.
        Filesystem::SetExecutable( destinationDir.toStdString() + "/GlyphPortable" );
#endif
	}

	void PortableVisualizationExport::CopyLogo(const QString& outputDir) const {

		if (outputDir.isEmpty()) {

			throw std::runtime_error("Can not save logo to empty directory.");
		}

		unsigned int logoIndex = QDir(outputDir).entryInfoList(QStringList("*.jpg"), QDir::Filter::Files).size() + 1;
		QString saveFile = outputDir;
		if (logoIndex > 99) {

			saveFile += "map00";
		}
		else if (logoIndex > 9) {

			saveFile += "map000";
		}
		else {

			saveFile += "map0000";
		}
		saveFile += QString::number(logoIndex) + ".jpg";

		QImage logoImage(SynGlyphX::GlyphBuilderApplication::GetLogoLocation(SynGlyphX::GlyphBuilderApplication::LogoType::WhiteBorder));
		logoImage.save(saveFile, "PNG", 100);
	}

} //namespace SynGlyphX
