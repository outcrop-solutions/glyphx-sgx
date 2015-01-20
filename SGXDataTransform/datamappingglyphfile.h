#ifndef SYNGLYPHX_DATAMAPPINGGLYPHFILE_H
#define SYNGLYPHX_DATAMAPPINGGLYPHFILE_H

#include "sgxdatatransform_global.h"
#include "xmlpropertytreefile.h"
#include "datamappingglyphgraph.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DataMappingGlyphFile : public XMLPropertyTreeFile
	{
	public:
		DataMappingGlyphFile(SynGlyphX::DataMappingGlyphGraph::SharedPtr graph = nullptr);
		virtual ~DataMappingGlyphFile();

		SynGlyphX::DataMappingGlyphGraph::SharedPtr GetDataMappingGlyphGraph() const;

	protected:
		virtual void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree);
		virtual void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const;

		SynGlyphX::DataMappingGlyphGraph::SharedPtr m_glyphGraph;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGGLYPHFILE_H