
#pragma once

#include <QtWidgets/QWidget>
#include <unordered_set>
#include "datatransformmapping.h"

namespace SynGlyphX { class SceneViewer; }
class QPushButton;

// todo:
// - needs to display some kind of names
//   - pass 1 - use tag input field names (via aliases)
//   - pass 2 - default to above but provide a custom option in datamapper
// - try to fix the camera swoop when starting up (SceneViewer single glyph mode issue)
// - improve centering if possible (ex glyphed "current year RC with prior year admits" centers on a weird spot)
class InteractiveLegend : public QWidget
{
	Q_OBJECT

public:
	InteractiveLegend(QWidget *parent, SynGlyphX::DataTransformMapping::ConstSharedPtr mapping);
	void setPrimaryViewer(SynGlyphX::SceneViewer* v) { primary_viewer = v; }	// provide the main scene's viewer so we can update its scene

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
	int glyph_index = 0;
};
