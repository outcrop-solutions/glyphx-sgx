#ifndef SYNGLYPHX_DATAMAPPINGGLYPHGRAPH_H
#define SYNGLYPHX_DATAMAPPINGGLYPHGRAPH_H

#include "sgxdatatransform_global.h"
#include <containers/ntree.hpp>
#include "datamappingglyph.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>
#include "inputbinding.h"
#include "inputfield.h"
#include <unordered_map>
#include "csvfilehandler.h"
#include <boost/graph/directed_graph.hpp>
#include "glyphgraph.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DataMappingGlyphGraph : public boost::directed_graph<DataMappingGlyph> {

	public:
		typedef DataMappingGlyphGraph::vertex_descriptor Vertex;

		typedef std::unordered_map<InputField::HashID, InputField> InputFieldMap;
		typedef std::unordered_map<InputField::HashID, unsigned int> InputFieldCountMap;
		typedef boost::property_tree::wptree PropertyTree;

		typedef std::shared_ptr<DataMappingGlyphGraph> SharedPtr;
		typedef std::shared_ptr<const DataMappingGlyphGraph> ConstSharedPtr;
		
		DataMappingGlyphGraph();
		DataMappingGlyphGraph(const boost::property_tree::wptree& propertyTree);
		DataMappingGlyphGraph(const GlyphGraph& graph);
		DataMappingGlyphGraph(const DataMappingGlyphGraph& graph);
		virtual ~DataMappingGlyphGraph();

		PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const;

		const Vertex& GetRootVertex() const;

		//bool operator==(const TemplateGlyphGraph& graph) const;
		//bool operator!=(const TemplateGlyphGraph& graph) const;

		//void WriteToFile(const std::string& filename) const;
		//void ReadFromFile(const std::string& filename);

		void SetInputField(InputBinding& binding, const InputField& inputfield);
		void ClearInputBinding(InputBinding& binding);

		const InputFieldMap& GetInputFields() const;

		bool DoesRootGlyphPositionXYHaveBindings() const;

		GlyphGraph::SharedPtr GetMinGlyphTree() const;
		GlyphGraph::SharedPtr GetMaxGlyphTree() const;

		static SharedPtr CreateDefault();

	private:
		void IncrementInputBindingCountsFromGlyph(const DataMappingGlyph& glyph);
		void IncrementInputBindingCount(const InputBinding& binding);
		//void ReadFromSGTFile(const std::string& filename);
		//void ReadFromANTzCSVFile(const std::string& filename);
		void ExportToPropertyTree(const Vertex& parent, boost::property_tree::wptree& propertyTreeParent) const;
		void ProcessPropertyTreeChildren(Vertex& parent, const boost::property_tree::wptree& propertyTree);
		void AddGraphGlyphSubgraph(Vertex& parent, const GlyphGraph::Vertex& glyphGraphParent, const GlyphGraph& graph);
		void CreateMinOrMaxGlyphSubtree(const Vertex& parent, const GlyphGraph::Vertex& newVertex, GlyphGraph::SharedPtr newGlyphGraph, bool isMax) const;
		//bool AreSubtreesEqual(const MinMaxGlyphTree::const_iterator& thisTreeNode, const MinMaxGlyphTree::const_iterator& otherTreeNode, const MinMaxGlyphTree& otherTree) const;

		InputFieldMap m_inputFields;
		InputFieldCountMap m_inputFieldReferenceCounts;

		Vertex m_rootVertex;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGGLYPHGRAPH_H