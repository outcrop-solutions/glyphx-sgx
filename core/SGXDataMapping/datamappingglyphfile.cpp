#include "datamappingglyphfile.h"
#include <stdexcept>

namespace SynGlyphX {

	DataMappingGlyphFile::DataMappingGlyphFile(SynGlyphX::DataMappingGlyphGraph::SharedPtr graph) :
		XMLPropertyTreeFile(),
		m_glyphGraph(graph)
	{
	}


	DataMappingGlyphFile::~DataMappingGlyphFile()
	{
	}

	SynGlyphX::DataMappingGlyphGraph::SharedPtr DataMappingGlyphFile::GetDataMappingGlyphGraph() const {

		return m_glyphGraph;
	}

	void DataMappingGlyphFile::ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) {

		boost::optional<const boost::property_tree::wptree&> glyphPropertyTree = filePropertyTree.get_child_optional(L"Glyph");
		if (glyphPropertyTree.is_initialized()) {

			m_glyphGraph = std::make_shared<DataMappingGlyphGraph>(glyphPropertyTree.get());
		}
		else {

			throw std::runtime_error("File does not have glyph tree");
		}
	}

	void DataMappingGlyphFile::ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const {

		if (m_glyphGraph) {

			m_glyphGraph->ExportToPropertyTree(filePropertyTree);
		}
		else {

			throw std::runtime_error("Can't write a null DataMappingGlyphGraph");
		}
	}

} //namespace SynGlyphX
