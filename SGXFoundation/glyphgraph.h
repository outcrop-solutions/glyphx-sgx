#ifndef SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H
#define SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H

#include "sgxfoundation.h"
#include "glyphstructuralproperties.h"
#include "glyphtemplate.h"
#include "glyphcolor.h"
//#include <boost/graph/directed_graph.hpp>
#include <containers/ntree.hpp>
#include <memory>
#include <array>

namespace SynGlyphX {

	typedef GlyphTemplate< double, GlyphColor, std::wstring > Glyph;

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

	class SGXFOUNDATION_API GlyphGraph : public stlplus::ntree<Glyph> {

	public:
		typedef std::shared_ptr<GlyphGraph> SharedPtr;
		typedef std::shared_ptr<const GlyphGraph> ConstSharedPtr;

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