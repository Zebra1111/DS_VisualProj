#ifndef VISUALBUBBLESORT_H
#define VISUALBUBBLESORT_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QParallelAnimationGroup>
#include <QVariantAnimation>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>
#include <QSize>
#include <QVector>

class VisualBubbleSort : public QObject {
    Q_OBJECT

public:
    VisualBubbleSort(QGraphicsScene* scene, QSize areaSize);
    QPointF getBarPosition(int index, int barHeight) const;
    void insertValue(int value);
    void compare(int i, int j);
    void swap(int i, int j);
    void markSorted(int index);
    void clear();

private:
    struct VisualNode {
        int value;
        QGraphicsRectItem* bar;
        QGraphicsTextItem* label;
    };

    QVector<VisualNode*> m_nodes;
    QGraphicsScene* m_scene;
    QSize m_areaSize;

    void animateSwap(int i, int j);
    void animateHighlight(int index, const QColor& color);
    void animateTemporaryHighlight(int index, const QColor& highlightColor);
};

#endif // VISUALBUBBLESORT_H
