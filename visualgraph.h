#ifndef VISUALGRAPH_H
#define VISUALGRAPH_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QAbstractAnimation>
#include <QVariantAnimation>
#include <QVariant>
#include <QVector>
#include <QMap>
#include <QtMath>

const int INF = 99999; //自定义无穷大

struct VisualGraphNode {
    int id;
    QPointF position;
    QGraphicsEllipseItem *ellipse;
    QGraphicsTextItem *label;
};

struct VisualGraphEdge {
    int src;
    int dest;
    QGraphicsLineItem *line;
};

class VisualGraph : public QObject {
    Q_OBJECT

public:
    VisualGraph(QGraphicsScene *scene, QSize areaSize);
    ~VisualGraph();

    void clear();
    void addNode(int id, QPointF position);
    void addEdge(int src, int dest);
    void addEdge(int src, int dest, int weight); // 带权重边
    void highlightNode(int id);
    void highlightEdge(int src, int dest);
    void Dj_highlightEdge(int src, int dest);
    void resetHighlights();
    void autoLayoutNodes(int nodeCount);
    void autoLayoutNodes_Fd(int nodeCount);

    // 新增邻接矩阵显示和更新功能
    void initDistanceMatrix(int vertices, QVector<QVector<int>> initialMatrix);
    void updateDistanceMatrix(int src, int dest, int newDist);


private:
    QGraphicsScene *m_scene;
    QSize m_areaSize;
    QMap<int, VisualGraphNode*> m_nodes;
    QVector<VisualGraphEdge*> m_edges;

    VisualGraphNode* createNode(int id, QPointF position);
    void createEdge(int src, int dest);

    QVector<QVector<QGraphicsTextItem*>> distanceMatrixTexts;
    QGraphicsRectItem* matrixBackground;
    void drawDistanceMatrix();
    QVector<QVector<int>> currentDistances;

};

#endif // VISUALGRAPH_H
