#include "visualbtree.h"


VisualBTree::VisualBTree(QGraphicsScene* scene, QSize areaSize, QGraphicsView* view)
    : m_scene(scene), m_areaSize(areaSize), m_rootVisual(nullptr), m_view(view) {
    m_logicTree = new BTree();
}

VisualBTree::~VisualBTree() {
   clear();
   delete m_logicTree;
}

void VisualBTree::clear() {
    clearVisualNode(m_rootVisual);
    m_rootVisual = nullptr;
    m_scene->clear();
    m_logicTree->clear();
}

void VisualBTree::clearVisualNode(VisualBTreeNode* node) {
    if (!node) return;
    for (auto child : node->children)
        clearVisualNode(child);
    m_scene->removeItem(node->rect);
    delete node->rect;
    for (auto text : node->keyTexts) {
        m_scene->removeItem(text);
        delete text;
    }
    delete node;
}

void VisualBTree::insertAnimated(int key) {
    m_logicTree->insert(key);
    visualizeTree();
    adjustViewScale();
}

void VisualBTree::removeAnimated(int key) {
    m_logicTree->remove(key);
    visualizeTree();
    adjustViewScale();
}

void VisualBTree::highlightNode(int key) {
    if (!m_rootVisual) return;
    VisualBTreeNode* target = findVisualNode(m_rootVisual, key);
    if (target) animateHighlight(target, key);
}

VisualBTreeNode* VisualBTree::findVisualNode(VisualBTreeNode* node, int key) {
    if (!node) return nullptr;
    for (int i = 0; i < node->logicalNode->numKeys; i++)
        if (node->logicalNode->keys[i] == key) return node;
    for (auto child : node->children) {
        VisualBTreeNode* found = findVisualNode(child, key);
        if (found) return found;
    }
    return nullptr;
}

void VisualBTree::animateHighlight(VisualBTreeNode* node, int key) {
    for (auto text : node->keyTexts) {
        if (text->toPlainText().toInt() == key) {
            QVariantAnimation* anim = new QVariantAnimation();
            anim->setDuration(500);
            anim->setStartValue(QColor("#E06C75"));
            anim->setEndValue(QColor("#FFFFFF"));
            anim->setEasingCurve(QEasingCurve::InOutQuad);

            connect(anim, &QVariantAnimation::valueChanged, [text](const QVariant &value) {
                text->setDefaultTextColor(value.value<QColor>());
            });

            anim->start(QAbstractAnimation::DeleteWhenStopped);
            break;
        }
    }
}

void VisualBTree::visualizeTree() {
    clearVisualNode(m_rootVisual);
    m_scene->clear();
    if (m_logicTree->getRoot())
        m_rootVisual = createVisualNode(m_logicTree->getRoot(), QPointF(m_areaSize.width() / 2, 60), 0);
    visualizeConnections(m_rootVisual);
}

VisualBTreeNode* VisualBTree::createVisualNode(BTreeNode* node, QPointF position, qreal level) {
    if (!node) return nullptr;

    const int rectWidth = 45 * (2 * DEGREE - 1);
    const int rectHeight = 50;
    VisualBTreeNode* vNode = new VisualBTreeNode{node, nullptr, {}, {}};

    QGraphicsRectItem* rect = m_scene->addRect(position.x() - rectWidth / 2, position.y(), rectWidth, rectHeight,
                                               QPen(QColor("#ABB2BF"), 2), QBrush(QColor("#707070")));
    rect->setBrush(QBrush(QColor("#707070")));
    rect->setPen(QPen(QColor("#E0E0E0"), 2));
    rect->setZValue(1);

    auto shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 150));
    shadow->setOffset(0, 4);
    rect->setGraphicsEffect(shadow);

    vNode->rect = rect;

    for (int i = 0; i < node->numKeys; i++) {
        auto text = m_scene->addText(QString::number(node->keys[i]), QFont("Segoe UI", 11, QFont::Bold));
        text->setDefaultTextColor(QColor("#FFFFFF"));
        text->setZValue(2);
        text->setPos(position.x() - rectWidth / 2 + i * 45 + 15, position.y() + 12);
        vNode->keyTexts.push_back(text);
    }

    qreal offset = rectWidth * 1.8;
    qreal childX = position.x() - offset * (node->numKeys / 2.0);
    qreal childY = position.y() + 100;

    for (int i = 0; i <= node->numKeys; i++) {
        if (node->children[i]) {
            auto child = createVisualNode(node->children[i], QPointF(childX + offset * i, childY), level + 1);
            vNode->children.push_back(child);
        }
    }
    return vNode;
}

void VisualBTree::visualizeConnections(VisualBTreeNode* node) {
    if (!node) return;
    QPointF parentCenter = node->rect->rect().center() + node->rect->pos() + QPointF(0, node->rect->rect().height() / 2);
    for (auto child : node->children) {
        QPointF childCenter = child->rect->rect().center() + child->rect->pos() - QPointF(0, child->rect->rect().height() / 2);
        auto line = m_scene->addLine(QLineF(parentCenter, childCenter), QPen(QColor("#E0E0E0"), 2));
        line->setZValue(0);
        visualizeConnections(child);
    }
}

void VisualBTree::adjustViewScale() {
    QRectF bounds = m_scene->itemsBoundingRect();
    QRectF marginBounds = bounds.adjusted(-50, -50, 50, 50);

    // 获取当前视图窗口在场景坐标中的大小
    QRectF viewSceneRect = m_view->mapToScene(m_view->viewport()->rect()).boundingRect(); // 当前视图对应的场景区域大小
    QSizeF sceneVisibleSize = viewSceneRect.size();                                       // 当前显示区域（未缩放前）

    // 计算是否超出当前窗口，需要缩放
    bool needScale = marginBounds.width() > sceneVisibleSize.width() ||
                     marginBounds.height() > sceneVisibleSize.height();

    if(needScale)
        m_view->fitInView(marginBounds, Qt::KeepAspectRatio);
}


