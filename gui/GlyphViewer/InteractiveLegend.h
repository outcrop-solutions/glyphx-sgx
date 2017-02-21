
#pragma once

#include <QtWidgets/QWidget>
#include <unordered_set>
#include "datatransformmapping.h"
namespace SynGlyphX { class SceneViewer; }

// todo:
// - needs to display some kind of names
//   - pass 1 - use tag input field names (via aliases)
//   - pass 2 - default to above but provide a custom option in datamapper
// - needs to handle scenes with multiple glyphs (look at how remapdialog does it?)
// - try to fix the camera swoop when starting up (SceneViewer single glyph mode issue)
class InteractiveLegend : public QWidget
{
	Q_OBJECT

public:
	InteractiveLegend(QWidget *parent, SynGlyphX::DataTransformMapping::ConstSharedPtr mapping, SynGlyphX::SceneViewer* primary_viewer);

protected:
	void showEvent(QShowEvent* event) override;

private:
	void update_viewer();

	SynGlyphX::SceneViewer* viewer;	// our window's viewer
	SynGlyphX::SceneViewer* primary_viewer;	// the main scene's viewer so we can update it
	SynGlyphX::DataTransformMapping::ConstSharedPtr mapping;
	std::unordered_set<uint32_t> hidden_elements;
};
