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