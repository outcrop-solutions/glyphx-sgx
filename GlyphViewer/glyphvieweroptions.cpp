#include "glyphvieweroptions.h"
#include <exception>
#include "application.h"
#include <QtCore/QDir>
#include <QtCore/QSettings>

GlyphViewerOptions::GlyphViewerOptions() :
	m_cacheDirectory(GetDefaultCacheDirectory()),
	m_hideUnselectedGlyphTrees(false),
	m_showSceneAxisHUDObject(true),
	m_sceneAxisHUDObjectLocation(SynGlyphXANTz::ANTzForestWidget::HUDLocation::TopLeft),
	m_showMessageWhenImagesDidNotDownload(true),
	m_loadSubsetVisualization(true),
	m_loadSubsetVisualizationInNewInstance(true) {
	
}

GlyphViewerOptions::GlyphViewerOptions(const GlyphViewerOptions& options) :
	m_cacheDirectory(options.m_cacheDirectory),
	m_hideUnselectedGlyphTrees(options.m_hideUnselectedGlyphTrees),
	m_showSceneAxisHUDObject(options.m_showSceneAxisHUDObject),
	m_sceneAxisHUDObjectLocation(options.m_sceneAxisHUDObjectLocation),
#ifdef USE_ZSPACE
	m_zSpaceOptions(options.m_zSpaceOptions),
#endif
	m_showMessageWhenImagesDidNotDownload(options.m_showMessageWhenImagesDidNotDownload),
	m_loadSubsetVisualization(options.m_loadSubsetVisualization),
	m_loadSubsetVisualizationInNewInstance(options.m_loadSubsetVisualizationInNewInstance) {

}

GlyphViewerOptions::~GlyphViewerOptions()
{
}

GlyphViewerOptions& GlyphViewerOptions::operator=(const GlyphViewerOptions& options) {

	m_cacheDirectory = options.m_cacheDirectory;
	m_hideUnselectedGlyphTrees = options.m_hideUnselectedGlyphTrees;
	m_showSceneAxisHUDObject = options.m_showSceneAxisHUDObject;
	m_sceneAxisHUDObjectLocation = options.m_sceneAxisHUDObjectLocation;
#ifdef USE_ZSPACE
	m_zSpaceOptions = options.m_zSpaceOptions;
#endif
	m_showMessageWhenImagesDidNotDownload = options.m_showMessageWhenImagesDidNotDownload;
	m_loadSubsetVisualization = options.m_loadSubsetVisualization;
	m_loadSubsetVisualizationInNewInstance = options.m_loadSubsetVisualizationInNewInstance;

	return *this;
}

bool GlyphViewerOptions::operator==(const GlyphViewerOptions& options) const {

	if (m_cacheDirectory != options.m_cacheDirectory) {

		return false;
	}

	if (m_hideUnselectedGlyphTrees != options.m_hideUnselectedGlyphTrees) {

		return false;
	}

	if (m_showSceneAxisHUDObject != options.m_showSceneAxisHUDObject) {

		return false;
	}

	if (m_sceneAxisHUDObjectLocation != options.m_sceneAxisHUDObjectLocation) {

		return false;
	}

#ifdef USE_ZSPACE
	if (m_zSpaceOptions != options.m_zSpaceOptions) {

		return false;
	}
#endif

	if (m_showMessageWhenImagesDidNotDownload != options.m_showMessageWhenImagesDidNotDownload) {

		return false;
	}

	if (m_loadSubsetVisualization != options.m_loadSubsetVisualization) {

		return false;
	}

	if (m_loadSubsetVisualizationInNewInstance != options.m_loadSubsetVisualizationInNewInstance) {

		return false;
	}

	return true;
}

bool GlyphViewerOptions::operator!=(const GlyphViewerOptions& options) const {

	return !operator==(options);
}

void GlyphViewerOptions::WriteToSettings() const {

	QSettings settings;

	settings.beginGroup("Options");

	if (GetCacheDirectory() != GlyphViewerOptions::GetDefaultCacheDirectory()) {

		settings.setValue("cacheDirectory", m_cacheDirectory);
	}
	else {

		settings.setValue("cacheDirectory", "");
	}
	settings.setValue("hideUnselectedGlyphs", m_hideUnselectedGlyphTrees);
	settings.setValue("axisInfoShow", m_showSceneAxisHUDObject);
	settings.setValue("axisInfoLocation", m_sceneAxisHUDObjectLocation);
	settings.setValue("showFailedToDownloadImageMessage", m_showMessageWhenImagesDidNotDownload);
	settings.setValue("loadSubsetVisualization", m_loadSubsetVisualization);
	settings.setValue("loadSubsetVisualizationNewInstance", m_loadSubsetVisualizationInNewInstance);
	settings.endGroup();

#ifdef USE_ZSPACE
	settings.beginGroup("zSpace");
	settings.setValue("stylusColor", GetZSpaceOptions().GetStylusColor());
	settings.setValue("stylusLength", GetZSpaceOptions().GetStylusLength());
	settings.endGroup();
#endif
}

void GlyphViewerOptions::ReadFromSettings() {

	QSettings settings;

	settings.beginGroup("Options");

	QString cacheDirectory = QDir::toNativeSeparators(settings.value("cacheDirectory", GlyphViewerOptions::GetDefaultCacheDirectory()).toString());
	if (cacheDirectory.isEmpty()) {

		cacheDirectory = GlyphViewerOptions::GetDefaultCacheDirectory();
	}
	SetCacheDirectory(cacheDirectory);
	SetHideUnselectedGlyphTrees(settings.value("hideUnselectedGlyphs", false).toBool());
	SetShowSceneAxisHUDObject(settings.value("axisInfoShow", true).toBool());
	SetSceneAxisObjectLocation(static_cast<SynGlyphXANTz::ANTzForestWidget::HUDLocation>(settings.value("axisInfoLocation").toInt()));
	SetShowMessageWhenImagesDidNotDownload(settings.value("showFailedToDownloadImageMessage", true).toBool());
	settings.endGroup();

#ifdef USE_ZSPACE
	settings.beginGroup("zSpace");
	SynGlyphX::ZSpaceOptions zSpaceOptions;
	zSpaceOptions.SetStylusColor(settings.value("stylusColor", QColor(Qt::green)).value<QColor>());
	zSpaceOptions.SetStylusLength(settings.value("stylusLength", 0.15f).toFloat());
	settings.endGroup();

	SetZSpaceOptions(zSpaceOptions);
#endif
}

void GlyphViewerOptions::SetCacheDirectory(const QString& newCacheDirectory) {

	m_cacheDirectory = newCacheDirectory;
}

const QString& GlyphViewerOptions::GetCacheDirectory() const {

	return m_cacheDirectory;
}

QString GlyphViewerOptions::GetDefaultCacheDirectory() {

	return QDir::toNativeSeparators(SynGlyphX::Application::GetWriteableLocation() + QDir::separator() + "cache");
}

void GlyphViewerOptions::SetHideUnselectedGlyphTrees(bool hideUnselectedGlyphTrees) {

	m_hideUnselectedGlyphTrees = hideUnselectedGlyphTrees;
}

bool GlyphViewerOptions::GetHideUnselectedGlyphTrees() const {

	return m_hideUnselectedGlyphTrees;
}

void GlyphViewerOptions::SetShowSceneAxisHUDObject(bool show) {

	m_showSceneAxisHUDObject = show;
}

bool GlyphViewerOptions::GetShowSceneAxisHUDObject() const {

	return m_showSceneAxisHUDObject;
}

void GlyphViewerOptions::SetSceneAxisObjectLocation(SynGlyphXANTz::ANTzForestWidget::HUDLocation location) {

	m_sceneAxisHUDObjectLocation = location;
}

SynGlyphXANTz::ANTzForestWidget::HUDLocation GlyphViewerOptions::GetSceneAxisObjectLocation() const {

	return m_sceneAxisHUDObjectLocation;
}

#ifdef USE_ZSPACE
void GlyphViewerOptions::SetZSpaceOptions(const SynGlyphX::ZSpaceOptions& options) {

	m_zSpaceOptions = options;
}

const SynGlyphX::ZSpaceOptions& GlyphViewerOptions::GetZSpaceOptions() const {

	return m_zSpaceOptions;
}
#endif

void GlyphViewerOptions::SetShowMessageWhenImagesDidNotDownload(bool showMessageWhenImagesDidNotDownload) {

	m_showMessageWhenImagesDidNotDownload = showMessageWhenImagesDidNotDownload;
}

bool GlyphViewerOptions::GetShowMessageWhenImagesDidNotDownload() const {

	return m_showMessageWhenImagesDidNotDownload;
}