#include "glyphvieweroptions.h"
#include <exception>
#include "application.h"
#include <QtCore/QDir>

GlyphViewerOptions::GlyphViewerOptions() :
	m_cacheDirectory(GetDefaultCacheDirectory()),
	m_hideUnselectedGlyphTrees(false) {
	
}

GlyphViewerOptions::GlyphViewerOptions(const GlyphViewerOptions& options) :
	m_cacheDirectory(options.m_cacheDirectory),
	m_hideUnselectedGlyphTrees(options.m_hideUnselectedGlyphTrees) {

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

	return QDir::toNativeSeparators(SynGlyphX::Application::GetCommonDataLocation() + QDir::separator() + "gv_cache");
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

	return *this;
}

bool GlyphViewerOptions::operator==(const GlyphViewerOptions& options) const {

	if (m_cacheDirectory != options.m_cacheDirectory) {

		return false;
	}

	if (m_hideUnselectedGlyphTrees != options.m_hideUnselectedGlyphTrees) {

		return false;
	}

	return true;
}

bool GlyphViewerOptions::operator!=(const GlyphViewerOptions& options) const {

	return !operator==(options);
}