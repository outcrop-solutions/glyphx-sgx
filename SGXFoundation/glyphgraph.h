#ifndef SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H
#define SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H

#include "sgxfoundation.h"
#include "glyphtemplate.h"
#include "color.h"
#include <boost/graph/undirected_graph.hpp>
#include <boost/bimap.hpp>
#include <memory>
#include <array>

namespace SynGlyphX {

	class SGXFOUNDATION_API Glyph : public GlyphTemplate < double, Color, std::wstring > {

	public:
		Glyph();
		Glyph(const Glyph& glyph);
		virtual ~Glyph();
	};

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
		typedef boost::bimap<Glyph::Shape, std::wstring> ShapeBimap;
		typedef boost::bimap<Glyph::Surface, std::wstring> SurfaceBimap;
		typedef boost::bimap<Glyph::VirtualTopology, std::wstring> VirtualTopologyBimap;

		typedef std::shared_ptr<GlyphGraph> SharedPtr;
		typedef std::shared_ptr<const GlyphGraph> SharedPtr;

		GlyphGraph();
		GlyphGraph(const GlyphGraph& graph);
		~GlyphGraph();

		static const ShapeBimap s_shapeNames;
		static const SurfaceBimap s_surfaceNames;
		static const VirtualTopologyBimap s_virtualTopologyNames;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H