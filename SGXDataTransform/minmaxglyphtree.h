#ifndef SYNGLYPHX_MINMAXGLYPHTREE_H
#define SYNGLYPHX_MINMAXGLYPHTREE_H

#include "sgxdatatransform_global.h"
#include <containers/ntree.hpp>
#include "minmaxglyph.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>
#include "inputbinding.h"
#include "inputfield.h"
#include <unordered_map>
#include "csvfilehandler.h"
#include <boost/graph/undirected_graph.hpp>
#include "glyphgraph.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DataMappingGlyphGraph : public boost::undirected_graph<DataMappingGlyph> {

	public:
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
		//bool operator==(const TemplateGlyphGraph& graph) const;
		//bool operator!=(const TemplateGlyphGraph& graph) const;

		//void WriteToFile(const std::string& filename) const;
		//void ReadFromFile(const std::string& filename);

		void SetInputField(MinMaxGlyphTree::const_iterator& node, unsigned int index, const InputField& inputfield);
		void ClearInputBinding(MinMaxGlyphTree::const_iterator& node, unsigned int index);

		const InputFieldMap& GetInputFields() const;

		bool DoesRootGlyphPositionXYHaveBindings() const;

		SynGlyphXANTz::GlyphTree::SharedPtr GetMinGlyphTree() const;
		SynGlyphXANTz::GlyphTree::SharedPtr GetMaxGlyphTree() const;

		static SharedPtr CreateDefault();

	private:
		//void ReadFromSGTFile(const std::string& filename);
		//void ReadFromANTzCSVFile(const std::string& filename);
		void ExportToPropertyTree(const MinMaxGlyphTree::const_iterator& parent, boost::property_tree::wptree& propertyTreeParent) const;
		void ProcessPropertyTreeChildren(const MinMaxGlyphTree::iterator& iT, const boost::property_tree::wptree& propertyTree);
		void AddGlyphSubtree(MinMaxGlyphTree::iterator& parentNode, const SynGlyphXANTz::GlyphTree& glyphTree, const SynGlyphXANTz::GlyphTree::const_iterator& iT);
		void CreateMinGlyphSubtree(const MinMaxGlyphTree::const_iterator& parentNode, SynGlyphXANTz::GlyphTree::iterator& newParent, SynGlyphXANTz::GlyphTree::SharedPtr newGlyphTree) const;
		void CreateMaxGlyphSubtree(const MinMaxGlyphTree::const_iterator& parentNode, SynGlyphXANTz::GlyphTree::iterator& newParent, SynGlyphXANTz::GlyphTree::SharedPtr newGlyphTree) const;
		//bool AreSubtreesEqual(const MinMaxGlyphTree::const_iterator& thisTreeNode, const MinMaxGlyphTree::const_iterator& otherTreeNode, const MinMaxGlyphTree& otherTree) const;

		InputFieldMap m_inputFields;
		InputFieldCountMap m_inputFieldReferenceCounts;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINMAXGLYPHTREE_H