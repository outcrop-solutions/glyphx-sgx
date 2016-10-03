#include "glyphvieweroptions.h"
#include <exception>
#include "application.h"
#include <QtCore/QDir>
#include <QtCore/QSettings>

GlyphViewerOptions::GlyphViewerOptions() :
	m_cacheDirectory(GetDefaultCacheDirectory()),
	m_hideUnselectedGlyphTrees(false),
	m_showHUDAxisObject(true),
	m_sceneAxisHUDObjectLocation(SynGlyphX::HUDAxesLocation::TopLeft),
	m_showSceneAxisObject(true),
	m_showMessageWhenImagesDidNotDownload(true),
	m_showHomePage(true),
	m_defaultProject(QString()),
	m_loadSubsetVisualization(true),
	m_loadSubsetVisualizationInNewInstance(true),
	m_FilteredGlyphOpacity(0.5f) {
	
}

GlyphViewerOptions::GlyphViewerOptions(const GlyphViewerOptions& options) :
	m_cacheDirectory(options.m_cacheDirectory),
	m_hideUnselectedGlyphTrees(options.m_hideUnselectedGlyphTrees),
	m_showHUDAxisObject(options.m_showHUDAxisObject),
	m_sceneAxisHUDObjectLocation(options.m_sceneAxisHUDObjectLocation),
	m_showSceneAxisObject(options.m_showSceneAxisObject),
#ifdef USE_ZSPACE
	m_zSpaceOptions(options.m_zSpaceOptions),
#endif
	m_showMessageWhenImagesDidNotDownload(options.m_showMessageWhenImagesDidNotDownload),
	m_showHomePage(options.m_showHomePage),
	m_defaultProject(options.m_defaultProject),
	m_loadSubsetVisualization(options.m_loadSubsetVisualization),
	m_loadSubsetVisualizationInNewInstance(options.m_loadSubsetVisualizationInNewInstance),
	m_FilteredGlyphOpacity(options.m_FilteredGlyphOpacity) {

}

GlyphViewerOptions::~GlyphViewerOptions()
{
}

GlyphViewerOptions& GlyphViewerOptions::operator=(const GlyphViewerOptions& options) {

	m_cacheDirectory = options.m_cacheDirectory;
	m_hideUnselectedGlyphTrees = options.m_hideUnselectedGlyphTrees;
	m_FilteredGlyphOpacity = options.m_FilteredGlyphOpacity;
	m_showHUDAxisObject = options.m_showHUDAxisObject;
	m_sceneAxisHUDObjectLocation = options.m_sceneAxisHUDObjectLocation;
	m_showSceneAxisObject = options.m_showSceneAxisObject;
#ifdef USE_ZSPACE
	m_zSpaceOptions = options.m_zSpaceOptions;
#endif
	m_showMessageWhenImagesDidNotDownload = options.m_showMessageWhenImagesDidNotDownload;
	m_showHomePage = options.m_showHomePage;
	m_defaultProject = options.m_defaultProject;
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

	if ( m_FilteredGlyphOpacity != options.m_FilteredGlyphOpacity ) {
		return false;
	}

	if (m_showHUDAxisObject != options.m_showHUDAxisObject) {

		return false;
	}

	if (m_sceneAxisHUDObjectLocation != options.m_sceneAxisHUDObjectLocation) {

		return false;
	}

	if (m_showSceneAxisObject != options.m_showSceneAxisObject) {

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

	if (m_showHomePage != options.m_showHomePage) {

		return false;
	}

	if (m_defaultProject != options.m_defaultProject) {

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
	settings.setValue("axisInfoShow", m_showHUDAxisObject);
	settings.setValue("axisInfoLocation", static_cast<unsigned int>(m_sceneAxisHUDObjectLocation));
	settings.setValue("sceneAxisInfoShow", m_showSceneAxisObject);
	settings.setValue("showFailedToDownloadImageMessage", m_showMessageWhenImagesDidNotDownload);
	settings.setValue("showHomeScreen", m_showHomePage);
	settings.setValue("defaultProject", m_defaultProject);
	settings.setValue("loadSubsetVisualization", m_loadSubsetVisualization);
	settings.setValue("loadSubsetVisualizationInNewInstance", m_loadSubsetVisualizationInNewInstance);
	settings.setValue("filteredGlyphOpacity", m_FilteredGlyphOpacity);
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
	SetShowHUDAxisObject(settings.value("axisInfoShow", true).toBool());
	SetHUDAxisObjectLocation(static_cast<SynGlyphX::HUDAxesLocation>(settings.value("axisInfoLocation").toInt()));
	SetShowSceneAxisObject(settings.value("sceneAxisInfoShow", true).toBool());
	SetShowMessageWhenImagesDidNotDownload(settings.value("showFailedToDownloadImageMessage", true).toBool());
	SetShowHomePage(settings.value("showHomeScreen", true).toBool());
	SetDefaultProject(settings.value("defaultProject", "").toString());
	SetLoadSubsetVisualization(settings.value("loadSubsetVisualization", true).toBool());
	SetLoadSubsetVisualizationInNewInstance(settings.value("loadSubsetVisualizationInNewInstance", false).toBool());
	SetFilteredGlyphOpacity(settings.value("filteredGlyphOpacity", 0.5f).toFloat());
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

void GlyphViewerOptions::SetShowHUDAxisObject(bool show) {

	m_showHUDAxisObject = show;
}

bool GlyphViewerOptions::GetShowHUDAxisObject() const {

	return m_showHUDAxisObject;
}

void GlyphViewerOptions::SetHUDAxisObjectLocation(SynGlyphX::HUDAxesLocation location) {

	m_sceneAxisHUDObjectLocation = location;
}

SynGlyphX::HUDAxesLocation GlyphViewerOptions::GetHUDAxisObjectLocation() const {

	return m_sceneAxisHUDObjectLocation;
}

void GlyphViewerOptions::SetShowSceneAxisObject(bool show) {

	m_showSceneAxisObject = show;
}

bool GlyphViewerOptions::GetShowSceneAxisObject() const {

	return m_showSceneAxisObject;
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

void GlyphViewerOptions::SetLoadSubsetVisualization(bool loadSubsetVisualization) {

	m_loadSubsetVisualization = loadSubsetVisualization;
}

bool GlyphViewerOptions::GetLoadSubsetVisualization() const {

	return m_loadSubsetVisualization;
}

void GlyphViewerOptions::SetLoadSubsetVisualizationInNewInstance(bool loadSubsetVisualizationInNewInstance) {

	m_loadSubsetVisualizationInNewInstance = loadSubsetVisualizationInNewInstance;
}

bool GlyphViewerOptions::GetLoadSubsetVisualizationInNewInstance() const {

	return m_loadSubsetVisualizationInNewInstance;
}

void GlyphViewerOptions::SetFilteredGlyphOpacity(float transparency) {
	m_FilteredGlyphOpacity = transparency;
}

float GlyphViewerOptions::GetFilteredGlyphOpacity() const {
	return m_FilteredGlyphOpacity;
}