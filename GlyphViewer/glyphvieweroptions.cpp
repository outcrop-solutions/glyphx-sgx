#include "glyphvieweroptions.h"
#include <exception>

GlyphViewerOptions::GlyphViewerOptions()
{
	
}

GlyphViewerOptions::GlyphViewerOptions(const GlyphViewerOptions& options) :
	m_cacheDirectory(options.m_cacheDirectory) {

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

GlyphViewerOptions& GlyphViewerOptions::operator=(const GlyphViewerOptions& options) {

	m_cacheDirectory = options.m_cacheDirectory;

	return *this;
}

bool GlyphViewerOptions::operator==(const GlyphViewerOptions& options) const {

	return (m_cacheDirectory == options.m_cacheDirectory);
}

bool GlyphViewerOptions::operator!=(const GlyphViewerOptions& options) const {

	return !operator==(options);
}