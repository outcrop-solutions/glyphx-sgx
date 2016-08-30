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
#pragma once
#ifndef SYNGLYPHX_GRAPHCOPYVISITOR_H
#define SYNGLYPHX_GRAPHCOPYVISITOR_H

#include <boost/graph/depth_first_search.hpp>
#include <stack>
#include <memory>

namespace SynGlyphX {
	/*
	template <typename DestinationGraph>
	class GraphCopyVisitor : public boost::default_dfs_visitor
	{
	public:
		GraphCopyVisitor() :
			boost::default_dfs_visitor(),
			m_newGraph(std::make_shared<DestinationGraph>()) {

		}

		virtual ~GraphCopyVisitor() {


		}

		template <typename Vertex, typename Graph>
		virtual void discover_vertex(Vertex u, const Graph& g) const {

			DestinationGraph::vertex_descriptor newVertex = AddVertex(u, g);
			if (!m_previousVertices.empty()) {

				m_newGraph->add_edge(m_previousVertices.top(), newVertex);
			}
			m_previousVertices.push(newVertex);
		}

		virtual void finish_vertex(Vertex u, const Graph& g) const {

			m_previousVertices.pop();
		}

		std::shared_ptr<DestinationGraph> GetNewGraph() const {

			return m_newGraph;
		}

	protected:
		template <typename Vertex, typename Graph>
		virtual DestinationGraph::vertex_descriptor AddVertex(Vertex u, const Graph& g) const {

			return m_newGraph->add_vertex(DestinationGraph::VertexProperty(g[u]));
		}

		std::shared_ptr<DestinationGraph> m_newGraph;
		std::stack<DestinationGraph::vertex_descriptor> m_previousVertices;
	};
	*/
} //namespace SynGlyphX

#endif //SYNGLYPHX_GRAPHCOPYVISITOR_H