#include "graph.h"

Graph::Graph(int vertices)
    : m_vertices(vertices),
      m_adjMatrix(vertices, QVector<int>(vertices, 0)),
      m_weightMatrix(vertices, QVector<int>(vertices, 0))
{
}

void Graph::addEdge(int src, int dest)
{
    if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices) {
        // 无向图
        m_adjMatrix[src][dest] = 1;
        m_adjMatrix[dest][src] = 1;
    }
}

void Graph::addEdge(int src, int dest, int weight)
{
    if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices) {
        // 无向图
        m_adjMatrix[src][dest] = 1;
        m_adjMatrix[dest][src] = 1;
        m_weightMatrix[src][dest] = weight;
        m_weightMatrix[dest][src] = weight;
    }
}

int Graph::getWeight(int src, int dest) const {
    return m_weightMatrix[src][dest];
}

void Graph::clear()
{
    for (int i = 0; i < m_vertices; ++i) {
        for (int j = 0; j < m_vertices; ++j) {
            m_adjMatrix[i][j] = 0;
            m_weightMatrix[i][j] = 0;
        }
    }
}
