#include "Glyph3DSceneExport.h"
#include "SceneViewer/glyphscene.h"
#include "SceneViewer/glyphnode.h"

namespace SynGlyphX {

	Glyph3DSceneExport::Glyph3DSceneExport()
	{
	}


	Glyph3DSceneExport::~Glyph3DSceneExport()
	{
	}

	void Glyph3DSceneExport::ExportMaxGlyphTo3DScene(const DataMappingGlyphGraph& dataMappingGlyph, GlyphScene& scene) {

		ExportGlyphTo3DScene(dataMappingGlyph, dataMappingGlyph.GetRoot(), nullptr, true, scene);
	}

	void Glyph3DSceneExport::ExportMinGlyphTo3DScene(const DataMappingGlyphGraph& dataMappingGlyph, GlyphScene& scene) {

		ExportGlyphTo3DScene(dataMappingGlyph, dataMappingGlyph.GetRoot(), nullptr, false, scene);
	}

	void Glyph3DSceneExport::ExportGlyphTo3DScene(const DataMappingGlyphGraph& dataMappingGlyph,
		const DataMappingGlyphGraph::ConstGlyphIterator& node,
		Glyph3DNode* parent,
		bool isMax,
		GlyphScene& scene) {

		auto* glyphnode = scene.allocGlyph(node->first, parent == nullptr, SynGlyphX::Glyph3DNodeType::GlyphElement);

		for (unsigned int i = 0; i < dataMappingGlyph.ChildCount(node); ++i) {

			ExportGlyphTo3DScene(dataMappingGlyph, dataMappingGlyph.GetChild(node, i), glyphnode, isMax, scene);
		}
	}

} //namespace SynGlyphX