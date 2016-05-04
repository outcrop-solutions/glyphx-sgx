#include "glyphbuilderapplication.h"
#include <QtCore/QDir>
#include <QtCore/QtGlobal>
#include <QtGui/QIcon>

namespace SynGlyphX {

	QString GlyphBuilderApplication::s_defaultBaseImagesLocation;
	QString GlyphBuilderApplication::s_splashScreenLocation;
	QMap<GlyphBuilderApplication::LogoType, QString> GlyphBuilderApplication::s_logoLocations;
	const char* GlyphBuilderApplication::s_internalSGXFeaturesEnvName = "SGXGlyphBuilderInternal";

	GlyphBuilderApplication::GlyphBuilderApplication(int& argc, char** argv)
		: Application(argc, argv)
	{

	}

	GlyphBuilderApplication::~GlyphBuilderApplication()
	{

	}
	
	void GlyphBuilderApplication::SetupIconsAndLogos() {

		if (IsGlyphEd()) {

			SetupIcons(QIcon(":SGXGUI/Resources/GlyphEd/synglyphx_x_ED.ico"));
			s_splashScreenLocation = ":SGXGUI/Resources/GlyphEd/glyphed_splash.png";
			s_logoLocations[LogoType::NoBorder] = ":SGXGUI/Resources/GlyphEd/glyphed_logo_no_border.png";
			s_logoLocations[LogoType::WhiteBorder] = ":SGXGUI/Resources/GlyphEd/glyphed_logo_white_border.png";
		}
		else {

			SetupIcons(QIcon(":SGXGUI/Resources/synglyphx_x.ico"));
			s_splashScreenLocation = ":SGXGUI/Resources/synglyphx_splash.png";
			s_logoLocations[LogoType::NoBorder] = ":SGXGUI/Resources/synglyphx_logo_white_border.png";
			s_logoLocations[LogoType::WhiteBorder] = ":SGXGUI/Resources/synglyphx_logo_white_border.png";
		}
	}

	const QString& GlyphBuilderApplication::GetDefaultBaseImagesLocation() {

		if (s_defaultBaseImagesLocation.isNull()) {

			s_defaultBaseImagesLocation = QDir::toNativeSeparators(QDir::cleanPath(applicationDirPath()) + QDir::separator() + "DefaultBaseImages" + QDir::separator());
		}

		return s_defaultBaseImagesLocation;
	}

	const QString& GlyphBuilderApplication::GetLogoLocation(LogoType logoType) {

		return s_logoLocations[logoType];
	}

	const QString& GlyphBuilderApplication::GetSplashScreenLocation() {

		return s_splashScreenLocation;
	}

	bool GlyphBuilderApplication::AreInternalSGXFeaturesEnabled() {

		if (qEnvironmentVariableIsSet(s_internalSGXFeaturesEnvName)) {

			return (qgetenv(s_internalSGXFeaturesEnvName).toInt() != 0);
		}

		return false;
	}

	bool GlyphBuilderApplication::IsGlyphEd() {

		QString glyphEdDir = QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::applicationDirPath()) + "/GlyphEd";
		return QDir(glyphEdDir).exists();
	}

} //namespace SynGlyphX