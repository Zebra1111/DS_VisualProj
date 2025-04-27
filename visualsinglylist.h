#ifndef VISUALSINGLYLIST_H
#define VISUALSINGLYLIST_H

#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QList>
#include <QSize>
#include <QPropertyAnimation>
#include <functional>
#include <QtMath>

struct VisualSinglyNode {
    int value;
    QPointF pos;
    QGraphicsWidget* widget;
    QGraphicsRectItem* shape; // 改为矩形
    QGraphicsTextItem* text;
    QGraphicsLineItem* lineToNext;
    QGraphicsPolygonItem* arrowToNext;
};

class VisualSinglyLinkedList {
public:
    VisualSinglyLinkedList(QGraphicsScene* scene, QGraphicsView* view, QSize areaSize);

    void clear();
    void insertAtHeadAnimated(int value);
    void insertAtTailAnimated(int value);
    void deleteValueAnimated(int value);
    void rebuildFromList(const QList<int>& values);
    bool findAndhighlight(int value);
    bool nextAndhighlight(int value);

private:
    void updateLayout();
    void resetView();
    void removeConnection(VisualSinglyNode* node);
    void animateMove(QGraphicsItem* item, QPointF end, int duration);
    void animateScale(QGraphicsItem* item, qreal start, qreal end, int duration,
                      QEasingCurve curve, std::function<void()> onFinish = nullptr);

    QList<VisualSinglyNode*> nodes;
    QGraphicsScene* scene;
    QGraphicsView* view;
    QSize areaSize;
    int index; //遍历变量
};

#endif // VISUALSINGLYLIST_H
