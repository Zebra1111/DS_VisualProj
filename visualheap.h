#ifndef VISUALHEAP_H
#define VISUALHEAP_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPropertyAnimation>
#include <QVector>
#include <QMap>
#include <QSize>
#include <QTimer>

struct VisualHeapNode {
    int value;
    int index;
    QGraphicsEllipseItem* circle;
    QGraphicsTextItem* label;
    QGraphicsLineItem* lineToParent = nullptr;

    VisualHeapNode(int val, int idx, QGraphicsEllipseItem* c, QGraphicsTextItem* l)
        : value(val), index(idx), circle(c), label(l), lineToParent(nullptr) {}
};

class VisualHeap : public QObject {
    Q_OBJECT

public:
    VisualHeap(QGraphicsScene* scene, QGraphicsView* view, QSize areaSize);
    ~VisualHeap();

    void clear();

    void insert(int value);
    void swap(int i, int j);
    void up(int from, int to);
    void down(int from, int to);
    void extractMax(int last);

    QVector<VisualHeapNode*> m_nodes;

private:
    QPointF getPosition(int index) const;
    void animateNode(VisualHeapNode* node, QPointF endPos, int duration = 300);
    void updateLines();

    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    QSize m_areaSize;
};

#endif // VISUALHEAP_H
