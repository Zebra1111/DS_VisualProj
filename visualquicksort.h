// VisualQuickSort.h
#ifndef VISUALQUICKSORT_H
#define VISUALQUICKSORT_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QParallelAnimationGroup>
#include <QVariantAnimation>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QSize>
#include <QSet>
#include <QVector>
#include <QTimer>
#include <QDebug>

class VisualQuickSort : public QObject {
    Q_OBJECT

public:
    VisualQuickSort(QGraphicsScene* scene, QSize areaSize);
    void insertValue(int value);      // 插入柱状条元素
    void setPivot(int index);         // 设置主元标记（紫色）
    void compareA(int i, int pivot_index);
    void compareB(int j, int pivot_index);
    void swap(int i, int j);          // 动画交换两个元素
    void markSorted(int index);       // 标记某个元素为已排序
    void clear();                     // 清除场景元素
    void animateSwap(int i, int j);
    void animateHighlight(int index);

private:
    struct VisualNode {
        int value;
        QGraphicsRectItem* bar;
        QGraphicsTextItem* label;
    };

    QSet<int> pivotIndices;             // 记录主元颜色
    QVector<VisualNode*> m_nodes;
    QGraphicsScene* m_scene;
    QSize m_areaSize;
    QPointF getBarPosition(int index, int barHeight) const;

};

#endif // VISUALQUICKSORT_H
