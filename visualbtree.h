#ifndef VISUALBTREE_H
#define VISUALBTREE_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QVariantAnimation>
#include <QGraphicsView>
#include <QEasingCurve>
#include "btree.h"

struct VisualBTreeNode {
    BTreeNode* logicalNode;                      // 对应的逻辑树节点
    QGraphicsRectItem* rect;                     // 可视化的矩形框
    QVector<QGraphicsTextItem*> keyTexts;        // 关键字文本
    QVector<VisualBTreeNode*> children;          // 子节点
};

class VisualBTree : public QObject {
    Q_OBJECT

public:
    VisualBTree(QGraphicsScene* scene, QSize areaSize, QGraphicsView* view);
    ~VisualBTree();

    void clear();                               // 清空树结构
    void insertAnimated(int key);               // 插入节点并动画更新
    void removeAnimated(int key);               // 删除节点并动画更新
    void highlightNode(int key);                // 查找节点并字体高亮动画

private:
    QGraphicsScene* m_scene;                    // 场景对象
    QSize m_areaSize;                           // 绘图区域尺寸
    QGraphicsView* m_view;                      // 视图对象用于自动缩放
    VisualBTreeNode* m_rootVisual;              // 根节点（可视化）
    BTree* m_logicTree;                         // B树逻辑结构

    void visualizeTree();                       // 可视化整棵树
    VisualBTreeNode* createVisualNode(BTreeNode* node, QPointF position, qreal level);
    void visualizeConnections(VisualBTreeNode* node);
    VisualBTreeNode* findVisualNode(VisualBTreeNode* node, int key);
    void animateHighlight(VisualBTreeNode* node, int key);
    void clearVisualNode(VisualBTreeNode* node);

    void adjustViewScale();                     // 自动调整视图缩放
};

#endif // VISUALBTREE_H
