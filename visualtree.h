#ifndef VISUALTREE_H
#define VISUALTREE_H

#include "binarytree.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPropertyAnimation>
#include <QMap>
#include <cmath>
#include <QDebug>


struct VisualNode {
    int value;
    QPointF pos;
    QGraphicsWidget *widget;
    QGraphicsEllipseItem *ellipse;
    QGraphicsTextItem *text;
    VisualNode *left, *right;
    QGraphicsLineItem *lineToParent;
    VisualNode *parent;
};


class VisualTree : public QObject {
    Q_OBJECT

public:
    VisualTree(QGraphicsScene *scene, QGraphicsView *view, QSize areaSize);

    void clear();
    void insertNodeDynamic(int value, TreeNode* root);
    void highlightNode(int value);
    void adjustScene();
    void resetView();
    VisualNode* findParentNode(int childValue);
    VisualNode* findVisualNode(int value);
    VisualNode* insertVisualNode(VisualNode*, TreeNode*, TreeNode*, QPointF);
    void rebuild(TreeNode* root);
    QSizeF computeSubtreeSize(TreeNode* node, int spacingX);
    VisualNode* layoutTreeRecursive(TreeNode* node, VisualNode* parentVisual,
                                                QPointF center, qreal xOffset,
                                                int spacingX, int spacingY, int depth);


private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    VisualNode* visualRoot;
    QMap<int, VisualNode*> nodeMap; // 二叉排序树不允许结点的值重复
    QSize areaSize;

};

#endif // VISUALTREE_H
