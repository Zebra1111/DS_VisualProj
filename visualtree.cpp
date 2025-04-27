#include "visualtree.h"

VisualTree::VisualTree(QGraphicsScene *scene, QGraphicsView *view, QSize visualAreaSize)
    : scene(scene), view(view), visualRoot(nullptr), areaSize(visualAreaSize) {
    scene->setSceneRect(0, 0, areaSize.width(), areaSize.height());
}


void VisualTree::clear() {
    visualRoot = nullptr;
    nodeMap.clear();
}


void VisualTree::insertNodeDynamic(int value, TreeNode* root) {
    if (!visualRoot) {
        const int startX = areaSize.width() / 2; // 不是功能性限制,而是语义性说明
        const int startY = 50;
        visualRoot = insertVisualNode(nullptr, nullptr, root, QPointF(startX, startY));
        resetView();
        return;
    }

    TreeNode* parentTreeNode = nullptr;
    TreeNode* current = root;

    // 在逻辑树中找到要插入节点的父节点
    while (current && current->value != value) {
        parentTreeNode = current;
        if (value < current->value) current = current->left;
        else current = current->right;
    }

    if (!parentTreeNode) return;

    VisualNode* parentVisualNode = findVisualNode(parentTreeNode->value);
    if (!parentVisualNode) return;

    int depth = (parentVisualNode->pos.y() - 50) / 80 + 1; // 估算深度：(parentY - 50) / 80 + 1
    int xOffset = 200 / pow(2, depth - 1); // 控制左右偏移量（根节点的左右子节点分布在两侧200距离处）

    // 孩子结点 = 父结点 + 偏移（根据值的大小觉得左/右偏移，下偏移固定为80）
    QPointF childPos = parentVisualNode->pos + QPointF((value < parentTreeNode->value ? -xOffset : xOffset), 80);
    insertVisualNode(parentVisualNode, parentTreeNode, current, childPos);
    resetView();
}

VisualNode* VisualTree::insertVisualNode(VisualNode* parentVisualNode, TreeNode* parentTreeNode,
                                         TreeNode* currentNode, QPointF currentPos) {
    if (!currentNode) return nullptr;

    // 创建图形部件包裹节点图形
    auto* widget = new QGraphicsWidget();
    scene->addItem(widget);
    widget->setTransformOriginPoint(0, 0);

    // 创建圆形节点
    auto* ellipse = new QGraphicsEllipseItem(-22, -22, 44, 44, widget);
    ellipse->setPen(QPen(QColor("#E0E0E0"), 2));
    ellipse->setBrush(QBrush(QColor("#707070")));

    // 创建文字节点
    auto* text = new QGraphicsTextItem(QString::number(currentNode->value), widget);
    QFont font("Segoe UI", 12, QFont::Bold);
    text->setFont(font);
    text->setDefaultTextColor(Qt::white);
    QRectF textRect = text->boundingRect();
    text->setPos(-textRect.width() / 2, -textRect.height() / 2 - 2); // 居中文字

    // 动画缩放效果
    auto* scaleAnim = new QPropertyAnimation(widget, "scale");
    scaleAnim->setDuration(600);
    scaleAnim->setStartValue(0);
    scaleAnim->setEndValue(1);
    scaleAnim->setEasingCurve(QEasingCurve::OutBack);
    scaleAnim->start(QAbstractAnimation::DeleteWhenStopped);

    // 构造 VisualNode 并赋值
    auto* visualNode = new VisualNode{
        currentNode->value,
        currentPos,
        widget,
        ellipse,
        text,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    };

    widget->setPos(currentPos);
    visualNode->parent = parentVisualNode;

    // 如果有父节点，添加连线
    if (parentVisualNode) {
        if (currentNode->value < parentTreeNode->value)
            parentVisualNode->left = visualNode;
        else
            parentVisualNode->right = visualNode;

        auto* line = scene->addLine(QLineF(parentVisualNode->pos, currentPos), QPen(QColor("#ABB2BF"), 2));
        line->setZValue(-1); // 放在节点图形下方
        visualNode->lineToParent = line;
    }

    nodeMap[currentNode->value] = visualNode;
    return visualNode;
}

void VisualTree::rebuild(TreeNode* root) {

    scene->clear();
    visualRoot = nullptr;
    nodeMap.clear();

    if (!root) return;

    const int spacingX = 60;
    const int spacingY = 80;
    QPointF rootPos(areaSize.width() / 2, 50);

    visualRoot = layoutTreeRecursive(root, nullptr, rootPos, areaSize.width() / 4, spacingX, spacingY, 0);

    resetView();
}

VisualNode* VisualTree::layoutTreeRecursive(TreeNode* node, VisualNode* parentVisual,
                                            QPointF center, qreal xOffset,
                                            int spacingX, int spacingY, int depth) {
    if (!node) return nullptr;

    QPointF currentPos = center + QPointF(0, depth * spacingY);

    auto* widget = new QGraphicsWidget();
    scene->addItem(widget);
    widget->setTransformOriginPoint(0, 0);

    auto* ellipse = new QGraphicsEllipseItem(-22, -22, 44, 44, widget);
    ellipse->setPen(QPen(QColor("#E0E0E0"), 2));
    ellipse->setBrush(QBrush(QColor("#707070")));

    auto* text = new QGraphicsTextItem(QString::number(node->value), widget);
    QFont font("Segoe UI", 12, QFont::Bold);
    text->setFont(font);
    text->setDefaultTextColor(Qt::white);
    QRectF textRect = text->boundingRect();
    text->setPos(-textRect.width() / 2, -textRect.height() / 2 - 2);

    widget->setPos(currentPos);

    auto* visualNode = new VisualNode{
        node->value,
        currentPos,
        widget,
        ellipse,
        text,
        nullptr,
        nullptr,
        nullptr,
        parentVisual
    };

    if (parentVisual) {
        auto* line = scene->addLine(QLineF(parentVisual->pos, currentPos), QPen(QColor("#ABB2BF"), 3));
        line->setZValue(-1);
        visualNode->lineToParent = line;

        if (node->value < parentVisual->value) {
            parentVisual->left = visualNode;
        } else {
            parentVisual->right = visualNode;
        }
    }

    nodeMap[node->value] = visualNode;

    // 递归构建左右子树
    visualNode->left = layoutTreeRecursive(node->left, visualNode,
                                           currentPos + QPointF(-xOffset, 0), xOffset / 2, spacingX, spacingY, depth + 1);
    visualNode->right = layoutTreeRecursive(node->right, visualNode,
                                            currentPos + QPointF(xOffset, 0), xOffset / 2, spacingX, spacingY, depth + 1);

    return visualNode;
}

QSizeF VisualTree::computeSubtreeSize(TreeNode* node, int spacingX) {
    if (!node) return QSizeF(0, 0);

    QSizeF leftSize = computeSubtreeSize(node->left, spacingX);
    QSizeF rightSize = computeSubtreeSize(node->right, spacingX);

    qreal totalWidth = qMax(spacingX * 1.0, leftSize.width() + rightSize.width());
    return QSizeF(totalWidth, 0);
}

void VisualTree::highlightNode(int value) {
    if (!nodeMap.contains(value)) return;
    auto* visualNode = nodeMap[value];

    visualNode->ellipse->setBrush(QBrush(QColor("#E06C75")));

    // 放大动画用于突出显示
    auto* pulseAnim = new QPropertyAnimation(visualNode->widget, "scale");
    pulseAnim->setDuration(500);
    pulseAnim->setStartValue(1.0);
    pulseAnim->setEndValue(1.5);
    pulseAnim->setEasingCurve(QEasingCurve::OutBounce);

    // 动画完成后更新颜色，并缩回原始大小
    connect(pulseAnim, &QPropertyAnimation::finished, [widget = visualNode->widget, ellipse = visualNode->ellipse]() {
        ellipse->setBrush(QBrush(QColor("#98C379")));

        auto* reverseAnim = new QPropertyAnimation(widget, "scale");
        reverseAnim->setDuration(500);
        reverseAnim->setStartValue(1.5);
        reverseAnim->setEndValue(1.0);
        reverseAnim->setEasingCurve(QEasingCurve::OutBounce);
        reverseAnim->start(QAbstractAnimation::DeleteWhenStopped);
    });

    pulseAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

VisualNode* VisualTree::findVisualNode(int value) {
    return nodeMap.value(value, nullptr);
}

VisualNode* VisualTree::findParentNode(int childValue) {
    for (auto& node : nodeMap) {
        if ((node->left && node->left->value == childValue) ||
            (node->right && node->right->value == childValue)) {
            return node;
        }
    }
    return nullptr;
}

void VisualTree::resetView() {
    QRectF bounds = scene->itemsBoundingRect();
    if (bounds.isEmpty()) return;

    QRectF marginBounds = bounds.adjusted(-50, -50, 50, 50);

    view->resetTransform(); // 重置缩放和移动

    // 获取当前视图窗口在场景坐标中的大小
    QRectF viewSceneRect = view->mapToScene(view->viewport()->rect()).boundingRect(); // 当前视图对应的场景区域大小
    QSizeF sceneVisibleSize = viewSceneRect.size();                                   // 当前显示区域（未缩放前）

    // 计算是否超出当前窗口，需要缩放
    bool needScale = marginBounds.width() > sceneVisibleSize.width() ||
                     marginBounds.height() > sceneVisibleSize.height();

    if (needScale) {
        view->fitInView(marginBounds, Qt::KeepAspectRatio);
    }
}












