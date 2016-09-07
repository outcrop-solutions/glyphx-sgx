#ifndef GLYPHVIEWEROPTIONS_H
#define GLYPHVIEWEROPTIONS_H

#include <QtCore/QString>

#ifdef USE_ZSPACE
#include "zspaceoptions.h"
#endif
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

	void WriteToSettings() const;
	void ReadFromSettings();

	void SetCacheDirectory(const QString& newCacheDirectory);
	const QString& GetCacheDirectory() const;
	static QString GetDefaultCacheDirectory();

	void SetHideUnselectedGlyphTrees(bool hideUnselectedGlyphTrees);
	bool GetHideUnselectedGlyphTrees() const;

	void SetShowHUDAxisObject(bool show);
	bool GetShowHUDAxisObject() const;

	void SetHUDAxisObjectLocation(SynGlyphXANTz::ANTzForestWidget::HUDLocation location);
	SynGlyphXANTz::ANTzForestWidget::HUDLocation GetHUDAxisObjectLocation() const;

	void SetShowSceneAxisObject(bool show);
	bool GetShowSceneAxisObject() const;

#ifdef USE_ZSPACE
	void SetZSpaceOptions(const SynGlyphX::ZSpaceOptions& options);
	const SynGlyphX::ZSpaceOptions& GetZSpaceOptions() const;
#endif

	void SetShowMessageWhenImagesDidNotDownload(bool showMessageWhenImagesDidNotDownload);
	bool GetShowMessageWhenImagesDidNotDownload() const;

	void SetLoadSubsetVisualization(bool loadSubsetVisualization);
	bool GetLoadSubsetVisualization() const;

	void SetLoadSubsetVisualizationInNewInstance(bool loadSubsetVisualizationInNewInstance);
	bool GetLoadSubsetVisualizationInNewInstance() const;

	void SetFilteredGlyphOpacity(float transparency);
	float GetFilteredGlyphOpacity() const;

private:
	//Cache options
	QString m_cacheDirectory;

	//3D options
	bool m_showHUDAxisObject;
	SynGlyphXANTz::ANTzForestWidget::HUDLocation m_sceneAxisHUDObjectLocation;

	bool m_showSceneAxisObject;

#ifdef USE_ZSPACE
	SynGlyphX::ZSpaceOptions m_zSpaceOptions;
#endif

	//Filtering options
	bool m_hideUnselectedGlyphTrees;
	bool m_loadSubsetVisualization;
	bool m_loadSubsetVisualizationInNewInstance;
	float m_FilteredGlyphOpacity;

	//UI options
	bool m_showMessageWhenImagesDidNotDownload;
};

#endif //GLYPHVIEWEROPTIONS_H