#include "glyphvieweroptions.h"
#include <exception>
#include "application.h"
#include <QtCore/QDir>

GlyphViewerOptions::GlyphViewerOptions() :
	m_cacheDirectory(GetDefaultCacheDirectory()),
	m_hideUnselectedGlyphTrees(false),
	m_showMessageWhenImagesDidNotDownload(true) {
	
}

GlyphViewerOptions::GlyphViewerOptions(const GlyphViewerOptions& options) :
	m_cacheDirectory(options.m_cacheDirectory),
	m_hideUnselectedGlyphTrees(options.m_hideUnselectedGlyphTrees),
	m_zSpaceOptions(options.m_zSpaceOptions),
	m_showMessageWhenImagesDidNotDownload(options.m_showMessageWhenImagesDidNotDownload) {

}

GlyphViewerOptions::~GlyphViewerOptions()
{
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

GlyphViewerOptions& GlyphViewerOptions::operator=(const GlyphViewerOptions& options) {

	m_cacheDirectory = options.m_cacheDirectory;
	m_hideUnselectedGlyphTrees = options.m_hideUnselectedGlyphTrees;
	m_zSpaceOptions = options.m_zSpaceOptions;
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

	if (m_zSpaceOptions != options.m_zSpaceOptions) {

		return false;
	}

	if (m_showMessageWhenImagesDidNotDownload != options.m_showMessageWhenImagesDidNotDownload) {

		return false;
	}

	return true;
}

bool GlyphViewerOptions::operator!=(const GlyphViewerOptions& options) const {

	return !operator==(options);
}

void GlyphViewerOptions::SetZSpaceOptions(const ZSpaceOptions& options) {

	m_zSpaceOptions = options;
}

const ZSpaceOptions& GlyphViewerOptions::GetZSpaceOptions() const {

	return m_zSpaceOptions;
}

void GlyphViewerOptions::SetShowMessageWhenImagesDidNotDownload(bool showMessageWhenImagesDidNotDownload) {

	m_showMessageWhenImagesDidNotDownload = showMessageWhenImagesDidNotDownload;
}

bool GlyphViewerOptions::GetShowMessageWhenImagesDidNotDownload() const {

	return m_showMessageWhenImagesDidNotDownload;
}