#ifndef SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H
#define SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H

#include "sgxfoundation.h"
#include "glyphtemplate.h"
#include "color.h"
#include <boost/graph/undirected_graph.hpp>
#include <memory>
#include <array>

namespace SynGlyphX {

	typedef GlyphTemplate< double, Color, std::wstring > Glyph;

	class SGXFOUNDATION_API GlyphEdge {

	public:
		enum Type {

			Topological,
			Link
		};

		Type m_type;

		GlyphEdge();
		GlyphEdge(const GlyphEdge& edge);
		~GlyphEdge();
	};

	class SGXFOUNDATION_API GlyphGraph : public boost::undirected_graph < Glyph, GlyphEdge > {

	public:
		typedef std::shared_ptr<GlyphGraph> SharedPtr;
		typedef std::shared_ptr<const GlyphGraph> SharedPtr;

		GlyphGraph();
		GlyphGraph(const GlyphGraph& graph);
		~GlyphGraph();

		static const Glyph s_defaultGlyph;
		static const Glyph s_defaultRootGlyph;

	private:
		static Glyph CreateDefaultGlyph(GlyphStructuralProperties::Shape geometryShape, GlyphStructuralProperties::VirtualTopology virtualTopology);
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H