#ifndef GLYPHVIEWEROPTIONS_H
#define GLYPHVIEWEROPTIONS_H

#include <QtCore/QString>
#include <SceneViewer/sceneviewer.h>

#ifdef USE_ZSPACE
#include "zspaceoptions.h"
#endif

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

	void SetHUDAxisObjectLocation(SynGlyphX::HUDAxesLocation location);
	SynGlyphX::HUDAxesLocation GetHUDAxisObjectLocation() const;

	void SetShowSceneAxisObject(bool show);
	bool GetShowSceneAxisObject() const;

#ifdef USE_ZSPACE
	void SetZSpaceOptions(const SynGlyphX::ZSpaceOptions& options);
	const SynGlyphX::ZSpaceOptions& GetZSpaceOptions() const;
#endif

	void SetShowMessageWhenImagesDidNotDownload(bool showMessageWhenImagesDidNotDownload);
	bool GetShowMessageWhenImagesDidNotDownload() const;
	void SetShowHomePage(bool v) { m_showHomePage = v;  }
	bool GetShowHomePage() const { return m_showHomePage;  }

	void SetDefaultProject(const QString& proj) { m_defaultProject = proj; }
	const QString& GetDefaultProject() const { return m_defaultProject; }

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
	SynGlyphX::HUDAxesLocation m_sceneAxisHUDObjectLocation;

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
	bool m_showHomePage;
	QString m_defaultProject;
};

#endif //GLYPHVIEWEROPTIONS_H