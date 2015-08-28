#include "linklessgraphmimedata.h"

namespace SynGlyphX {

	const QString LinklessGraphMimeData::s_format("application/x-sgx-glyph-linklessgraph");

	LinklessGraphMimeData::LinklessGraphMimeData(const DataMappingGlyphGraph::LinklessGraph& subgraph)
		: QMimeData(),
		m_subgraph(subgraph)
	{

	}

	LinklessGraphMimeData::LinklessGraphMimeData(const DataMappingGlyph& glyph)
		: QMimeData(),
		m_subgraph() {

		m_subgraph.insert(DataMappingGlyphGraph::VertexType(0, glyph));
	}

	LinklessGraphMimeData::~LinklessGraphMimeData()
	{

	}

	QStringList LinklessGraphMimeData::formats() const {

		QStringList types;
		types.push_back(s_format);
		return types;
	}

	bool LinklessGraphMimeData::hasFormat(const QString & mimeType) const {

		return (mimeType == s_format);
	}

	const DataMappingGlyphGraph::LinklessGraph& LinklessGraphMimeData::GetSubGraph() const {

		return m_subgraph;
	}

} //namespace SynGlyphX