#include "glyphvieweroptions.h"
#include <exception>
#include "application.h"
#include <QtCore/QDir>

GlyphViewerOptions::GlyphViewerOptions() :
	m_cacheDirectory(GetDefaultCacheDirectory()),
	m_hideUnselectedGlyphTrees(false),
	m_showSceneAxisHUDObject(true),
	m_sceneAxisHUDObjectLocation(SynGlyphXANTz::ANTzForestWidget::HUDLocation::TopLeft),
	m_showMessageWhenImagesDidNotDownload(true) {
	
}

GlyphViewerOptions::GlyphViewerOptions(const GlyphViewerOptions& options) :
	m_cacheDirectory(options.m_cacheDirectory),
	m_hideUnselectedGlyphTrees(options.m_hideUnselectedGlyphTrees),
	m_showSceneAxisHUDObject(options.m_showSceneAxisHUDObject),
	m_sceneAxisHUDObjectLocation(options.m_sceneAxisHUDObjectLocation),
#ifdef USE_ZSPACE
	m_zSpaceOptions(options.m_zSpaceOptions),
#endif
	m_showMessageWhenImagesDidNotDownload(options.m_showMessageWhenImagesDidNotDownload) {

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

	return true;
}

bool GlyphViewerOptions::operator!=(const GlyphViewerOptions& options) const {

	return !operator==(options);
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