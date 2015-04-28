#ifndef GLYPHVIEWEROPTIONS_H
#define GLYPHVIEWEROPTIONS_H

#include <QtCore/QString>
#include "zspaceoptions.h"

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
	static QString GetDefaultCacheDirectory();

	void SetHideUnselectedGlyphTrees(bool hideUnselectedGlyphTrees);
	bool GetHideUnselectedGlyphTrees() const;

	void SetZSpaceOptions(const ZSpaceOptions& options);
	const ZSpaceOptions& GetZSpaceOptions() const;

private:
	//Cache options
	QString m_cacheDirectory;

	//3D options
	bool m_hideUnselectedGlyphTrees;

	ZSpaceOptions m_zSpaceOptions;
};

#endif //GLYPHVIEWEROPTIONS_H