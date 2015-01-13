#ifndef SYNGLYPHX_DATAMAPPINGGLYPHGRAPHEXPORTVISITOR_H
#define SYNGLYPHX_DATAMAPPINGGLYPHGRAPHEXPORTVISITOR_H

#include "graphcopyvisitor.h"
#include "glyphgraph.h"
#include "datamappingglyphgraph.h"

namespace SynGlyphX {

	class DataMappingGlyphGraphMinExportVisitor : public GraphCopyVisitor<GlyphGraph>
	{
	public:
		

	protected:
		virtual GlyphGraph::Vertex AddVertex(DataMappingGlyphGraph::Vertex u, const DataMappingGlyphGraph& g) const {

			return m_newGraph->add_vertex(g[u].GetMinGlyph());
		}
	};

	class DataMappingGlyphGraphMaxExportVisitor : public GraphCopyVisitor<GlyphGraph>
	{
	public:


	protected:
		virtual GlyphGraph::Vertex AddVertex(DataMappingGlyphGraph::Vertex u, const DataMappingGlyphGraph& g) const {

			return m_newGraph->add_vertex(g[u].GetMaxGlyph());
		}
	};
}

#endif //SYNGLYPHX_DATAMAPPINGGLYPHGRAPHEXPORTVISITOR_H