#ifndef VISUALSTACK_H
#define VISUALSTACK_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QVector>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QTimer>
#include <QDebug>

struct VisualStackNode {
    int value;
    QGraphicsWidget* widget;
    QGraphicsProxyWidget* proxy;
    QLabel* label;
};

class VisualStack : public QObject {
    Q_OBJECT

public:
    VisualStack(QGraphicsScene* scene, QSize areaSize);
    void clear();
    void pushAnimated(int value);
    void popAnimated();
    void highlightTop();

private:
    VisualStackNode* createNode(int value, QPointF pos);
    void updateLayout();
    void animateProperty(QObject* obj, const QByteArray& property, QVariant start, QVariant end, int duration, QEasingCurve curve = QEasingCurve::OutCubic);

    QGraphicsScene* m_scene;
    QSize m_areaSize;
    QVector<VisualStackNode*> m_nodes;
    //QVector<QSharedPointer<VisualStackNode>> m_nodes;

    const QSize nodeSize = {130, 60};
    const int nodeSpacing = 15;
};

#endif // VISUALSTACK_H
