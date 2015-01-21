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

		typedef std::vector<ConstSharedPtr> ConstSharedVector;

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