#ifndef GLYPHVIEWEROPTIONS_H
#define GLYPHVIEWEROPTIONS_H

#include <QtCore/QString>

class GlyphViewerOptions
{
public:
	GlyphViewerOptions();
	GlyphViewerOptions(const GlyphViewerOptions& options);
	~GlyphViewerOptions();

	GlyphViewerOptions& operator=(const GlyphViewerOptions& options);
	bool operator==(const GlyphViewerOptions& options) const;
	bool operator!=(const GlyphViewerOptions& options) const;

	void SetCacheDirectory(const QString& newCacheDirectory);
	const QString& GetCacheDirectory() const;

private:
	QString m_cacheDirectory;

};

#endif //GLYPHVIEWEROPTIONS_H