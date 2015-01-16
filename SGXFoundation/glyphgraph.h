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
		typedef std::vector<ConstSharedPtr> ConstSharedVector;

		GlyphGraph();
		GlyphGraph(const Glyph& rootGlyph);
		GlyphGraph(const GlyphGraph& graph);
		~GlyphGraph();

		void AllocateChildSubtree(const std::vector<Glyph>& templates, const std::vector<unsigned int> instances, const GlyphGraph::iterator& parent);

		static const Glyph s_defaultGlyph;
		static const Glyph s_defaultRootGlyph;

	protected:
		GlyphGraph(const std::vector<Glyph>& templates, const std::vector<unsigned int> instances, unsigned int depth);

	private:
		static Glyph CreateDefaultGlyph(GlyphStructuralProperties::Shape geometryShape, GlyphStructuralProperties::VirtualTopology virtualTopology);
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H