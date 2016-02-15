#include "portablevisualizationexport.h"
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include "glyphbuilderapplication.h"
#include "filesystem.h"

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

		if (DoesPlatformHaveSourceDirectory(Platform::WindowsZSpace)) {

			QAction* zSpaceAction = menu->addAction(tr("Windows (zSpace)"));
			QObject::connect(zSpaceAction, &QAction::triggered, this, [this]{ emit CreatePortableVisualization(Platform::WindowsZSpace); });
		}

		if (DoesPlatformHaveSourceDirectory(Platform::Mac)) {

			QAction* macAction = menu->addAction(tr("Mac"));
			QObject::connect(macAction, &QAction::triggered, this, [this]{ emit CreatePortableVisualization(Platform::Mac); });
		}
	}

	void PortableVisualizationExport::SetupSourceDirectories() {

		QString sourceBaseDir = QDir::toNativeSeparators(QFileInfo(SynGlyphX::Application::applicationDirPath()).canonicalFilePath()) + QDir::separator();

		QSettings settings;
		settings.beginGroup("ANTzExport");
		AddSourceDirectoryToPlatformIfItExists(Platform::Windows, settings.value("windows", sourceBaseDir + "ANTzTemplate").toString());
		AddSourceDirectoryToPlatformIfItExists(Platform::WindowsZSpace, settings.value("zSpace", sourceBaseDir + "ANTzzSpaceTemplate").toString());
		AddSourceDirectoryToPlatformIfItExists(Platform::Mac, settings.value("mac", sourceBaseDir + "ANTzMacTemplate").toString());
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

		if (!baseSourceDir.cd("usr")) {

			return false;
		}

		if (!baseSourceDir.exists("csv")) {

			return false;
		}

		if (!baseSourceDir.exists("images")) {

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
		if ((platform == Platform::Windows) || (platform == Platform::WindowsZSpace)) {

			QString appPath = SynGlyphX::Application::applicationDirPath() + QDir::separator();

			QFile::copy(appPath + "msvcp120.dll", destinationDir + "/msvcp120.dll");
			QFile::copy(appPath + "msvcr120.dll", destinationDir + "/msvcr120.dll");
			QFile::copy(appPath + "vccorlib120.dll", destinationDir + "/vccorlib120.dll");
		}
	}

} //namespace SynGlyphX