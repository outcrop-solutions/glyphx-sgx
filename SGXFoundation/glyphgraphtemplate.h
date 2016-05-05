//
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

#ifndef SYNGLYPHX_GLYPHGRAPHTEMPLATE_H
#define SYNGLYPHX_GLYPHGRAPHTEMPLATE_H

#include <containers/ntree.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <map>
//#include <boost/graph/directed_graph.hpp>
//#include <boost/graph/labeled_graph.hpp>

namespace SynGlyphX {

	template <typename GlyphType, typename LinkType = GlyphType>
	class GlyphGraphTemplate : public stlplus::ntree<std::pair<unsigned long, GlyphType>> {

	public:
		/*enum Edge {

			Topology,
			Link
		};*/

		typedef unsigned long Label;

		typedef typename stlplus::ntree<std::pair<long unsigned int, GlyphType> >::iterator GlyphIterator;
		typedef typename stlplus::ntree<std::pair<long unsigned int, GlyphType> >::const_iterator ConstGlyphIterator;

		typedef std::map<std::pair<Label, Label>, LinkType> LinkMap;
		typedef std::pair<Label, GlyphType> VertexType;
		typedef stlplus::ntree<VertexType> LabeledTree;

		GlyphGraphTemplate() :
			stlplus::ntree<std::pair<unsigned long, GlyphType>>(),
			m_nextLabel(0) {

			
		}

		GlyphGraphTemplate(const GlyphType& rootGlyph) :
			stlplus::ntree<std::pair<unsigned long, GlyphType>>(),
			m_nextLabel(0) {

			stlplus::ntree<std::pair<long unsigned int, GlyphType> >::insert(VertexType(GetNextLabel(), rootGlyph));
		}

		GlyphGraphTemplate(const GlyphGraphTemplate& graph) :
			stlplus::ntree<std::pair<unsigned long, GlyphType>>(graph),
			m_nextLabel(graph.m_nextLabel),
			m_linkGlyphs(graph.m_linkGlyphs) {

		}

		GlyphGraphTemplate(const LabeledTree& graph) :
			stlplus::ntree<std::pair<unsigned long, GlyphType>>(),
			m_nextLabel(0) {

			stlplus::ntree<std::pair<long unsigned int, GlyphType> >::insert(VertexType(GetNextLabel(), graph.root()->second));
			CopyChildren(GetRoot(), graph.root(), graph);
		}

		~GlyphGraphTemplate() {


		}

		ConstGlyphIterator GetRoot() const {

			return stlplus::ntree<std::pair<long unsigned int, GlyphType> >::root();
		}

		GlyphIterator GetRoot() {

			return stlplus::ntree<std::pair<long unsigned int, GlyphType> >::root();
		}

		ConstGlyphIterator GetParent(const ConstGlyphIterator& vertex) const {

			return stlplus::ntree<std::pair<long unsigned int, GlyphType> >::parent(vertex);
		}

		GlyphIterator GetParent(const GlyphIterator& vertex) {

			return stlplus::ntree<std::pair<long unsigned int, GlyphType> >::parent(vertex);
		}

		ConstGlyphIterator GetChild(const ConstGlyphIterator& vertex, unsigned int pos) const {

			return stlplus::ntree<std::pair<long unsigned int, GlyphType> >::child(vertex, pos);
		}

		GlyphIterator GetChild(const GlyphIterator& vertex, unsigned int pos) {

			return stlplus::ntree<std::pair<long unsigned int, GlyphType> >::child(vertex, pos);
		}

		GlyphGraphTemplate GetSubgraph(const GlyphIterator& vertex) {

			return GlyphGraphTemplate(stlplus::ntree<std::pair<long unsigned int, GlyphType> >::subtree(vertex));
		}

		GlyphGraphTemplate GetAndRemoveSubgraph(const GlyphIterator& vertex) {

			return GlyphGraphTemplate(stlplus::ntree<std::pair<long unsigned int, GlyphType> >::cut(vertex));
		}

		unsigned int ChildCount(const ConstGlyphIterator& vertex) const {

			return stlplus::ntree<std::pair<long unsigned int, GlyphType> >::children(vertex);
		}

		unsigned int GetDepth(const ConstGlyphIterator& vertex) const {

			return stlplus::ntree<std::pair<long unsigned int, GlyphType> >::depth(vertex);
		}

		GlyphIterator SetRootGlyph(const GlyphType& glyph) {

			return SetRootGlyph(glyph, GetNextLabel());
		}

		GlyphIterator AddChildGlyph(const GlyphIterator& vertex, const GlyphType& glyph) {

			return AddChildGlyph(vertex, glyph, GetNextLabel());
		}

		GlyphIterator AddChildGlyphGraph(const GlyphIterator& vertex, const LabeledTree& graph) {

			ConstGlyphIterator otherRoot = graph.root();
			GlyphIterator newIterator = AddChildGlyph(vertex, otherRoot->second);
			CopyChildren(newIterator, otherRoot, graph);

			return newIterator;
		}

		virtual GlyphIterator AddChildGlyphGraph(const GlyphIterator& vertex, const GlyphGraphTemplate& graph) {

			ConstGlyphIterator otherRoot = graph.root();
			GlyphIterator newIterator = AddChildGlyph(vertex, otherRoot->second);
			CopyChildren(newIterator, otherRoot, graph);

			return newIterator;
		}

		void RemoveChild(const GlyphIterator& vertex, unsigned int pos) {

			Remove(GetChild(vertex, pos));
		}

		void Remove(const GlyphIterator& vertex) {

			for (int i = stlplus::ntree<std::pair<long unsigned int, GlyphType> >::children(vertex) - 1; i >= 0; --i) {

				Remove(GetChild(vertex, i));
			}

			RemoveRelatedLinks(vertex);
			stlplus::ntree<std::pair<long unsigned int, GlyphType> >::erase(vertex);
		}

		virtual void UpdateGlyph(const GlyphIterator& vertex, const GlyphType& glyph) {

			vertex->second = glyph;
		}

		void AddLink(Label beginning, Label end, const LinkType& glyph) {

			if (beginning == end) {

				throw std::invalid_argument("Vertex can not link to itself");
			}

			std::pair<Label, Label> linkIndex(beginning, end);
			auto iT = m_linkGlyphs.find(linkIndex);
			if (iT != m_linkGlyphs.end()) {

				iT->second = glyph;
				return;
			}
			else {

				m_linkGlyphs.insert(std::pair<std::pair<Label, Label>, LinkType>(linkIndex, glyph));
			}
		}

		/*Label GetLabel(const GlyphIterator& vertex) const {

			return vertex->first;
		}*/

		const LinkMap& GetLinks() const {

			return m_linkGlyphs;
		}

	protected:
		GlyphIterator SetRootGlyph(const GlyphType& glyph, Label label) {

			if (!stlplus::ntree<std::pair<long unsigned int, GlyphType> >::empty()) {

				throw std::invalid_argument("Can't set root glyph on non empty glyph graph");
			}

			GlyphIterator newIterator = stlplus::ntree<std::pair<long unsigned int, GlyphType> >::insert(VertexType(label, glyph));

			return newIterator;
		}

		void CopyChildren(const GlyphIterator& vertex, const ConstGlyphIterator& parent, const LabeledTree& graph) {

			for (unsigned int i = 0; i < graph.children(parent); ++i) {

				const ConstGlyphIterator childFromOtherGraph = graph.child(parent, i);
				GlyphIterator child = AddChildGlyph(vertex, childFromOtherGraph->second);
				CopyChildren(child, childFromOtherGraph, graph);
			}
		}

		void CopyChildren(const GlyphIterator& vertex, const ConstGlyphIterator& parent, const GlyphGraphTemplate& graph) {

			for (unsigned int i = 0; i < graph.ChildCount(parent); ++i) {

				const ConstGlyphIterator childFromOtherGraph = graph.GetChild(parent, i);
				
				GlyphIterator child = AddChildGlyph(vertex, childFromOtherGraph->second);
				CopyChildren(child, childFromOtherGraph, graph);
			}
		}

		/*void AddLabels(const GlyphIterator& vertex) {

			AddLabel(vertex, GetNextLabel());

			for (unsigned int i = 0; i < ChildCount(vertex.constify()); ++i) {

				AddLabels(GetChild(vertex, i));
			}
		}

		void AddLabel(const GlyphIterator& vertex, Label label) {

			m_labels.insert(boost::bimap<boost::bimaps::set_of<Label>, GlyphIterator>::value_type(label, vertex));
		}*/

		void RemoveRelatedLinks(const GlyphIterator& vertex) {

			Label labelToRemove = vertex->first;

			std::vector<std::pair<Label, Label>> linksToRemove;
			for (auto iT = m_linkGlyphs.begin(); iT != m_linkGlyphs.end(); ++iT) {

				if ((iT->first.first == labelToRemove) || (iT->first.second == labelToRemove)) {

					linksToRemove.push_back(iT->first);
				}
			}

			for (const std::pair<Label, Label>& linkToRemove: linksToRemove) {

				m_linkGlyphs.erase(linkToRemove);
			}

			//m_labels.left.erase(labelToRemove);
		}

		Label GetNextLabel() {

			Label nextLabel = m_nextLabel;
			++m_nextLabel;
			return nextLabel;
		}

		GlyphIterator AddChildGlyph(const GlyphIterator& vertex, const GlyphType& glyph, Label label) {

			if (label + 1 > m_nextLabel) {

				m_nextLabel = label + 1;
			}

			GlyphIterator newIterator = stlplus::ntree<std::pair<long unsigned int, GlyphType> >::insert(vertex, VertexType(label, glyph));

			return newIterator;
		}

		//boost::labeled_graph < boost::directed_graph<GlyphType, Edge>, Label > m_graph;

		LinkMap m_linkGlyphs;

		Label m_nextLabel;
	};
}

#endif //SYNGLYPHX_GLYPHGRAPHTEMPLATE_H
