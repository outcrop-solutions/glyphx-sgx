#include "portablevisualizationexport.h"
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include "glyphbuilderapplication.h"

namespace SynGlyphX {

	PortableVisualizationExport::PortableVisualizationExport()
	{

	}

	PortableVisualizationExport::~PortableVisualizationExport()
	{

	}

	void PortableVisualizationExport::SetupSourceDirectories() {

		QSettings settings;
		settings.beginGroup("ANTzExport");
		s_sourceDirectories[Platform::Windows] = settings.value("default", SynGlyphX::Application::applicationDirPath() + QDir::separator() + "ANTzTemplate").toString();
		s_sourceDirectories[Platform::WindowsZSpace] = settings.value("zSpace", SynGlyphX::Application::applicationDirPath() + QDir::separator() + "ANTzzSpaceTemplate").toString();
		s_sourceDirectories[Platform::Mac] = settings.value("mac", SynGlyphX::Application::applicationDirPath() + QDir::separator() + "ANTzMacTemplate").toString();
		settings.endGroup();
	}

	const QString& PortableVisualizationExport::GetSourceDirectory(Platform platform) {

		if (s_sourceDirectories.count(platform) == 0) {

			throw std::exception("Platform does not exist");
		}

		return s_sourceDirectories[platform];
	}

} //namespace SynGlyphX