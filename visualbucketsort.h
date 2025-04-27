#ifndef VISUALBUCKETSORT_H
#define VISUALBUCKETSORT_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSimpleTextItem>
#include <QVariantAnimation>
#include <QTimer>
#include <QVector>
#include <QMap>
#include <QSize>


class VisualBucketSort : public QObject {
    Q_OBJECT

public:
    VisualBucketSort(QGraphicsScene* scene, QSize areaSize);
    void clear();

    void insertValue(int value);                   // 上方插入数组元素
    void assignToBucket(int value, int bucketIdx); // 移动元素到底部桶
    void swapInBucket(int i, int j, int bucketId); // 桶内冒泡排序
    void markFinal(int value);                     // 回归数组位置并高亮

private:
    struct VisualNode {
        int value;
        QGraphicsRectItem* box;
        QGraphicsTextItem* label;
        int bucket = -1;       // -1 表示未分配桶
        QPointF originalPos;   // 上方原始位置

        VisualNode(int v, QGraphicsRectItem* b, QGraphicsTextItem* l, QPointF pos)
                : value(v), box(b), label(l), bucket(-1), originalPos(pos) {}
    };
    QGraphicsScene* m_scene;
    QSize m_areaSize;

    QVector<VisualNode*> m_nodes;
    QMap<int, QVector<VisualNode*>> m_buckets;
    QMap<int, QGraphicsTextItem*> m_bucketLabels;

    QPointF getTopArrayPos(int index) const;
    QPointF getBucketPos(int bucketIdx, int localIndex) const;

    void animateMove(VisualNode* node, QPointF target);
    void animateMove(VisualNode* node, QPointF target, QSizeF targetSize);
    void animateHighlight(VisualNode* node, const QColor& color);
};

#endif // VISUALBUCKETSORT_H
