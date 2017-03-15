
#pragma once

#include <QtWidgets/QWidget>
#include <unordered_set>
#include "datatransformmapping.h"

namespace SynGlyphX { class SceneViewer; }
class QPushButton;

// interactive legend, allows user to examine and show/hide individual elements or whole branches
// todo:
// - provide a custom element name option in datamapper
// - try to fix the camera swoop when starting up or switching glyphs (SceneViewer single glyph mode issue)
// - improve centering if possible (ex glyphed "current year RC with prior year admits" centers on a weird spot)
// - improve text positioning if possible (this is a general SceneViewer issue though)
class InteractiveLegend : public QWidget
{
	Q_OBJECT

public:
	InteractiveLegend(QWidget *parent, SynGlyphX::DataTransformMapping::ConstSharedPtr mapping);
	void setPrimaryViewer(SynGlyphX::SceneViewer* v) { primary_viewer = v; }	// provide the main scene's viewer so we can update its scene
	void reset() { setup = false; }

protected:
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

private:
	void update_viewer();
	void build_scene();

	SynGlyphX::SceneViewer* viewer = nullptr;	// our window's viewer
	SynGlyphX::SceneViewer* primary_viewer = nullptr;	// the main scene's viewer so we can update it
	SynGlyphX::DataTransformMapping::ConstSharedPtr mapping;
	QPushButton* prev = nullptr;
	QPushButton* next = nullptr;
	std::unordered_set<uint32_t> hidden_elements;
	int total_elements = 0;
	int glyph_index = 0;
	bool setup = false;
};
