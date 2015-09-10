#include "linklessgraphmimedata.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <sstream>

namespace SynGlyphX {

	const QString LinklessGraphMimeData::s_format("sgx-glyph/datamapping-linklessgraph");

	LinklessGraphMimeData::LinklessGraphMimeData()
	{

	}

	LinklessGraphMimeData::~LinklessGraphMimeData()
	{

	}

	/*void LinklessGraphMimeData::ConvertToMimeData(const DataMappingGlyphGraph& graph, QMimeData* mimeData) {

		ConvertToMimeData(graph, mimeData);
	}
	
	void LinklessGraphMimeData::ConvertToMimeData(const DataMappingGlyph& glyph, QMimeData* mimeData) {

		DataMappingGlyphGraph dataMappingGraph;
		dataMappingGraph.SetRootGlyph(glyph);
		ConvertToMimeData(dataMappingGraph, mimeData);
	}*/

	void LinklessGraphMimeData::ConvertToMimeData(const DataMappingGlyphGraph& graph, QMimeData* mimeData) {

		boost::property_tree::wptree propertyTree;
		graph.ExportToPropertyTree(propertyTree);
		std::wstringstream xmlText;
		boost::property_tree::write_xml(xmlText, propertyTree);

		QByteArray utf8ByteArray = QString::fromStdWString(xmlText.str()).toUtf8();

		mimeData->setData(s_format, utf8ByteArray);
	}

	DataMappingGlyphGraph LinklessGraphMimeData::ConvertToLinklessGraph(const QMimeData* const mimeData) {

		if (!mimeData->hasFormat(s_format)) {

			throw std::invalid_argument("Invalid mime type for linkless graph");
		}

		boost::property_tree::wptree propertyTree;
		std::wstringstream xmlText;
		xmlText.str(QString::fromUtf8(mimeData->data(s_format)).toStdWString());
		boost::property_tree::read_xml(xmlText, propertyTree, boost::property_tree::xml_parser::trim_whitespace);

		DataMappingGlyphGraph graph(propertyTree.get_child(L"Glyph"));
		return graph;
	}

} //namespace SynGlyphX