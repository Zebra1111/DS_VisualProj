#ifndef VISUALQUEUE_H
#define VISUALQUEUE_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

struct VisualQueueNode {
    int value;
    QGraphicsWidget* widget;
    QGraphicsProxyWidget* proxy;
    QLabel* label;
};

class VisualQueue : public QObject {
    Q_OBJECT

public:
    VisualQueue(QGraphicsScene* scene, QSize areaSize);
    void clear();
    void enqueueAnimated(int value);
    void dequeueAnimated();
    void highlightFront();

private:
    VisualQueueNode* createNode(int value, QPointF pos);
    void updateLayout();
    void animateProperty(QObject* obj, const QByteArray& property, QVariant start, QVariant end, int duration, QEasingCurve curve = QEasingCurve::OutCubic);

    QGraphicsScene* m_scene;
    QSize m_areaSize;
    QVector<VisualQueueNode*> m_nodes;

    const QSize nodeSize = {60, 40};
    const int nodeSpacing = 15;
};

#endif // VISUALQUEUE_H
