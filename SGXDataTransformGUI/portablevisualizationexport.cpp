#include "portablevisualizationexport.h"
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include "glyphbuilderapplication.h"

namespace SynGlyphX {

	std::unordered_map<PortableVisualizationExport::Platform, QString> PortableVisualizationExport::s_sourceDirectories;

	PortableVisualizationExport::PortableVisualizationExport()
	{

	}

	PortableVisualizationExport::~PortableVisualizationExport()
	{

	}

	void PortableVisualizationExport::SetupSourceDirectories() {

		QSettings settings;
		settings.beginGroup("ANTzExport");
		AddSourceDirectoryToPlatformIfItExists(Platform::Windows, settings.value("windows", SynGlyphX::Application::applicationDirPath() + QDir::separator() + "ANTzTemplate").toString());
		AddSourceDirectoryToPlatformIfItExists(Platform::WindowsZSpace, settings.value("zSpace", SynGlyphX::Application::applicationDirPath() + QDir::separator() + "ANTzzSpaceTemplate").toString());
		AddSourceDirectoryToPlatformIfItExists(Platform::Mac, settings.value("mac", SynGlyphX::Application::applicationDirPath() + QDir::separator() + "ANTzMacTemplate").toString());
		settings.endGroup();
	}

	const QString& PortableVisualizationExport::GetSourceDirectory(Platform platform) {

		if (s_sourceDirectories.count(platform) == 0) {

			throw std::exception("Platform does not exist");
		}

		return s_sourceDirectories[platform];
	}

	bool PortableVisualizationExport::DoesPlatformHaveSourceDirectory(Platform platform) {

		return (s_sourceDirectories.count(platform) == 1);
	}

	bool PortableVisualizationExport::DoAnyPlatformsHaveSourceDirectories() {

		return (s_sourceDirectories.size() > 0);
	}

	void PortableVisualizationExport::AddSourceDirectoryToPlatformIfItExists(Platform platform, const QString& directoryName) {

		if (DoesSourceDirectoryExist(directoryName)) {

			s_sourceDirectories[platform] = directoryName;
		}
	}

	bool PortableVisualizationExport::DoesSourceDirectoryExist(const QString directoryName) {

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

} //namespace SynGlyphX