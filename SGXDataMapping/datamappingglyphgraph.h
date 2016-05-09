///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

#ifndef SYNGLYPHX_DATAMAPPINGGLYPHGRAPH_H
#define SYNGLYPHX_DATAMAPPINGGLYPHGRAPH_H

#include "sgxdatamapping.h"
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
#include "glyphgraphtemplate.h"
#include "glyphgraph.h"

namespace SynGlyphX {

	class SGXDATAMAPPING_API DataMappingGlyphGraph : public GlyphGraphTemplate<DataMappingGlyph> {

	public:
		typedef std::unordered_map<InputField::HashID, InputField> InputFieldMap;
		typedef std::unordered_map<InputField::HashID, unsigned int> InputFieldCountMap;
		typedef boost::property_tree::wptree PropertyTree;

		typedef std::shared_ptr<DataMappingGlyphGraph> SharedPtr;
		typedef std::shared_ptr<const DataMappingGlyphGraph> ConstSharedPtr;

		typedef std::vector<ConstSharedPtr> ConstSharedVector;

		typedef stlplus::ntree_node<std::pair<Label, DataMappingGlyph>> Node;
		
		DataMappingGlyphGraph();
		DataMappingGlyphGraph(const boost::property_tree::wptree& propertyTree);
		DataMappingGlyphGraph(const GlyphGraph& graph);
		DataMappingGlyphGraph(const DataMappingGlyphGraph& graph);
		DataMappingGlyphGraph(const LabeledTree& graph);
		virtual ~DataMappingGlyphGraph();

		PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const;

		bool operator==(const DataMappingGlyphGraph& graph) const;
		bool operator!=(const DataMappingGlyphGraph& graph) const;

		DataMappingGlyphGraph GetSubgraph(const GlyphIterator& vertex, bool includeChildren);
		DataMappingGlyphGraph GetAndRemoveSubgraph(const GlyphIterator& vertex);

		void SetInputField(DataMappingGlyphGraph::ConstGlyphIterator node, DataMappingGlyph::MappableField field, const InputField& inputfield);
		void ClearInputBinding(DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field);
		void ClearAllInputBindings(DataMappingGlyphGraph::ConstGlyphIterator& node);
		void ClearInputFieldBindings(const InputField& inputfield);
		void ClearAllInputBindings();

		const InputFieldMap& GetInputFields() const;

		bool DoesRootGlyphPositionXYHaveBindings() const;

		void ResetRootMinMaxPositionXY();

		void SetMergeRoots(bool mergeRoots);
		bool GetMergeRoots() const;

		GlyphGraph::SharedPtr GetMinGlyphTree() const;
		GlyphGraph::SharedPtr GetMaxGlyphTree() const;

		void ClearFieldGroup(const std::wstring& fieldGroupName);

		virtual GlyphIterator AddChildGlyphGraph(const GlyphIterator& vertex, const DataMappingGlyphGraph& graph);
		virtual void UpdateGlyph(const GlyphIterator& vertex, const DataMappingGlyph& glyph);

		bool IsTransformable() const;

		static SharedPtr CreateDefault();

	private:
		void AddAllInputBindingsToSubgraph(DataMappingGlyphGraph& graph, const GlyphIterator& vertex, bool removeFromThisGraph);
		void ClearAllInputBindings(DataMappingGlyphGraph& graph, const GlyphIterator& vertex);
		void ClearInputFieldBindings(DataMappingGlyphGraph& graph, const GlyphIterator& vertex, const InputField& inputfield);
		void ClearFieldGroup(const std::wstring& fieldGroupName, const GlyphIterator& vertex);
		void IncrementInputBindingCountsFromGlyph(const DataMappingGlyph& glyph);
		void IncrementInputBindingCount(const InputBinding& binding);
		void ExportChildrenToPropertyTree(const DataMappingGlyphGraph::ConstGlyphIterator& parent, boost::property_tree::wptree& propertyTreeParent) const;
		void ProcessPropertyTreeChildren(const DataMappingGlyphGraph::GlyphIterator& parent, const boost::property_tree::wptree& propertyTree);
		void AddGraphGlyphSubgraph(DataMappingGlyphGraph::GlyphIterator parent, GlyphGraph::ConstGlyphIterator glyphGraphParent, const GlyphGraph& graph);
		GlyphGraph::SharedPtr CreateMinOrMaxGlyphSubtree(bool isMax) const;
		void CreateMinOrMaxGlyphSubtree(const DataMappingGlyphGraph::ConstGlyphIterator parent, GlyphGraph::GlyphIterator newVertex, GlyphGraph::SharedPtr newGlyphGraph, bool isMax) const;
		bool AreSubtreesEqual(const DataMappingGlyphGraph::ConstGlyphIterator& thisTreeNode, const DataMappingGlyphGraph::ConstGlyphIterator& otherTreeNode, const DataMappingGlyphGraph& otherTree) const;

		InputFieldMap m_inputFields;
		InputFieldCountMap m_inputFieldReferenceCounts;

		bool m_mergeRoots;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGGLYPHGRAPH_H
