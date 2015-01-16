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
//#include <boost/graph/directed_graph.hpp>
#include <containers/ntree.hpp>
#include "glyphgraph.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DataMappingGlyphGraph : public stlplus::ntree<DataMappingGlyph> {

	public:
		typedef std::unordered_map<InputField::HashID, InputField> InputFieldMap;
		typedef std::unordered_map<InputField::HashID, unsigned int> InputFieldCountMap;
		typedef boost::property_tree::wptree PropertyTree;

		typedef std::shared_ptr<DataMappingGlyphGraph> SharedPtr;
		typedef std::shared_ptr<const DataMappingGlyphGraph> ConstSharedPtr;

		typedef stlplus::ntree_node<DataMappingGlyph> Node;
		
		DataMappingGlyphGraph();
		DataMappingGlyphGraph(const boost::property_tree::wptree& propertyTree);
		DataMappingGlyphGraph(const GlyphGraph& graph);
		DataMappingGlyphGraph(const DataMappingGlyphGraph& graph);
		virtual ~DataMappingGlyphGraph();

		PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const;

		bool operator==(const DataMappingGlyphGraph& graph) const;
		bool operator!=(const DataMappingGlyphGraph& graph) const;

		//void WriteToFile(const std::string& filename) const;
		//void ReadFromFile(const std::string& filename);

		void SetInputField(DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field, const InputField& inputfield);
		void ClearInputBinding(DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field);

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
		void ExportToPropertyTree(const DataMappingGlyphGraph::const_iterator& parent, boost::property_tree::wptree& propertyTreeParent) const;
		void ProcessPropertyTreeChildren(const DataMappingGlyphGraph::iterator& parent, const boost::property_tree::wptree& propertyTree);
		void AddGraphGlyphSubgraph(DataMappingGlyphGraph::iterator& parent, const GlyphGraph::const_iterator& glyphGraphParent, const GlyphGraph& graph);
		void CreateMinOrMaxGlyphSubtree(const DataMappingGlyphGraph::const_iterator& parent, GlyphGraph::iterator& newVertex, GlyphGraph::SharedPtr newGlyphGraph, bool isMax) const;
		bool AreSubtreesEqual(const DataMappingGlyphGraph::const_iterator& thisTreeNode, const DataMappingGlyphGraph::const_iterator& otherTreeNode, const DataMappingGlyphGraph& otherTree) const;

		InputFieldMap m_inputFields;
		InputFieldCountMap m_inputFieldReferenceCounts;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGGLYPHGRAPH_H