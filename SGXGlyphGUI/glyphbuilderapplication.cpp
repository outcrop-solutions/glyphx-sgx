#include "glyphbuilderapplication.h"
#include <QtCore/QDir>
#include <QtCore/QtGlobal>

namespace SynGlyphX {

	QString GlyphBuilderApplication::s_defaultBaseImagesLocation;
	const char* GlyphBuilderApplication::s_internalSGXFeaturesEnvName = "SGXGlyphBuilderInternal";

	GlyphBuilderApplication::GlyphBuilderApplication(int& argc, char** argv)
		: Application(argc, argv)
	{

	}

	GlyphBuilderApplication::~GlyphBuilderApplication()
	{

	}
	/*
	void GlyphBuilderApplication::Setup(const QString& appName, const QString& appVersion) {

		Application::Setup(appName, appVersion);

		s_defaultBaseImagesLocation = QDir::toNativeSeparators(applicationDirPath() + QDir::separator() + "DefaultBaseImages" + QDir::separator());
	}*/

	const QString& GlyphBuilderApplication::GetDefaultBaseImagesLocation() {

		if (s_defaultBaseImagesLocation.isNull()) {

			s_defaultBaseImagesLocation = QDir::toNativeSeparators(QDir::cleanPath(applicationDirPath()) + QDir::separator() + "DefaultBaseImages" + QDir::separator());
		}

		return s_defaultBaseImagesLocation;
	}

	bool GlyphBuilderApplication::AreInternalSGXFeaturesEnabled() {

		if (qEnvironmentVariableIsSet(s_internalSGXFeaturesEnvName)) {

			return (qgetenv(s_internalSGXFeaturesEnvName).toInt() != 0);
		}

		return false;
	}

} //namespace SynGlyphX