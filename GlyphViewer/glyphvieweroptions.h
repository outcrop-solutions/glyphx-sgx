#ifndef GLYPHVIEWEROPTIONS_H
#define GLYPHVIEWEROPTIONS_H

#include <QtCore/QString>
#include "zspaceoptions.h"
#include "antzforestwidget.h"

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

	void SetShowSceneAxisHUDObject(bool show);
	bool GetShowSceneAxisHUDObject() const;

	void SetSceneAxisObjectLocation(SynGlyphXANTz::ANTzForestWidget::HUDLocation location);
	SynGlyphXANTz::ANTzForestWidget::HUDLocation GetSceneAxisObjectLocation() const;

	void SetZSpaceOptions(const SynGlyphX::ZSpaceOptions& options);
	const SynGlyphX::ZSpaceOptions& GetZSpaceOptions() const;

	void SetShowMessageWhenImagesDidNotDownload(bool showMessageWhenImagesDidNotDownload);
	bool GetShowMessageWhenImagesDidNotDownload() const;

private:
	//Cache options
	QString m_cacheDirectory;

	//3D options
	bool m_hideUnselectedGlyphTrees;
	bool m_showSceneAxisHUDObject;
	SynGlyphXANTz::ANTzForestWidget::HUDLocation m_sceneAxisHUDObjectLocation;

	SynGlyphX::ZSpaceOptions m_zSpaceOptions;

	//UI options
	bool m_showMessageWhenImagesDidNotDownload;
};

#endif //GLYPHVIEWEROPTIONS_H