#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QVector>
#include <QSet>

class Graph
{
public:
    explicit Graph(int vertices);

    void addEdge(int src, int dest);
    void addEdge(int src, int dest, int weight);
    int getWeight(int src, int dest) const;
    void clear();

private:

    int m_vertices;
    QVector<QVector<int>> m_adjMatrix;
    QVector<QVector<int>> m_weightMatrix;
};

#endif // GRAPH_H
