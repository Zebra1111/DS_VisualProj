#include "visualheap.h"
#include <QtMath>
#include <QFont>
#include <QVariantAnimation>

VisualHeap::VisualHeap(QGraphicsScene* scene, QGraphicsView* view, QSize areaSize)
    : m_scene(scene), m_view(view), m_areaSize(areaSize) {}

VisualHeap::~VisualHeap() {
    clear();
}

void VisualHeap::clear() {
    m_nodes.clear();
}

QPointF VisualHeap::getPosition(int index) const {
    int depth = static_cast<int>(qFloor(qLn(index + 1) / qLn(2)));
    int maxNodes = 1 << depth;
    int posInLevel = index - ((1 << depth) - 1);
    int spacingX = 60 * (1 << (3 - depth));
    int baseX = (m_areaSize.width() - (maxNodes - 1) * spacingX) / 2;
    int x = baseX + posInLevel * spacingX;
    int y = depth * 100 + 60;
    return QPointF(x, y);
}

void VisualHeap::insert(int value) {
    int index = m_nodes.size();
    QPointF pos = getPosition(index);

    auto* circle = m_scene->addEllipse(-25, -25, 50, 50,
                                       QPen(QColor("#ABB2BF"), 2), QBrush(QColor("#707070")));
    circle->setZValue(1);
    circle->setPos(pos);

    QFont font("Segoe UI", 12, QFont::Bold);
    auto* label = m_scene->addText(QString::number(value), font);
    label->setDefaultTextColor(Qt::white);
    label->setZValue(2);
    label->setPos(pos.x() - label->boundingRect().width() / 2,
                  pos.y() - label->boundingRect().height() / 2);

    auto* node = new VisualHeapNode{value, index, circle, label};
    m_nodes.append(node);

    if (index > 0) {
        int parent = (index - 1) / 2;
        QPointF p = getPosition(parent);
        node->lineToParent = m_scene->addLine(QLineF(p, pos), QPen(QColor("#ABB2BF"), 2));
        node->lineToParent->setZValue(0);
    }
}

void VisualHeap::swap(int i, int j) {
    if (i < 0 || j < 0 || i >= m_nodes.size() || j >= m_nodes.size()) return;

    auto* nodeA = m_nodes[i];
    auto* nodeB = m_nodes[j];
    nodeA->circle->setBrush(QColor("#E06C75"));
    nodeB->circle->setBrush(QColor("#E06C75"));

    auto animateScale = [](QGraphicsItem* item) {
        auto* scaleAnim = new QVariantAnimation();
        scaleAnim->setStartValue(1.0);
        scaleAnim->setKeyValueAt(0.5, 1.25);
        scaleAnim->setEndValue(1.0);
        scaleAnim->setDuration(600);
        scaleAnim->setEasingCurve(QEasingCurve::OutBack);

        QObject::connect(scaleAnim, &QVariantAnimation::valueChanged, [item](const QVariant &value) {
            item->setScale(value.toReal());
        });

        scaleAnim->start(QAbstractAnimation::DeleteWhenStopped);
     };

     animateScale(nodeA->circle);
     animateScale(nodeB->circle);

     // 延迟恢复原色
     QTimer::singleShot(600, [=]() {
        nodeA->circle->setBrush(QColor("#707070"));
        nodeB->circle->setBrush(QColor("#707070"));
     });

     // 值交换 + 更新 label
     std::swap(nodeA->value, nodeB->value);
     nodeA->label->setPlainText(QString::number(nodeA->value));
     nodeB->label->setPlainText(QString::number(nodeB->value));
}

void VisualHeap::up(int from, int to) {
    swap(from, to);
}

void VisualHeap::down(int from, int to) {
    swap(from, to);
}

void VisualHeap::extractMax(int last) {
    if (m_nodes.isEmpty()) return;

    auto* nodeA = m_nodes[0];
    auto* nodeB = m_nodes[last];

    nodeA->circle->setBrush(QColor("#E06C75"));
    nodeB->circle->setBrush(QColor("#E06C75"));

    auto animateScale = [](QGraphicsItem* item) {
        auto* scaleAnim = new QVariantAnimation();
        scaleAnim->setStartValue(1.0);
        scaleAnim->setKeyValueAt(0.5, 1.25);
        scaleAnim->setEndValue(1.0);
        scaleAnim->setDuration(600);
        scaleAnim->setEasingCurve(QEasingCurve::OutBack);

        QObject::connect(scaleAnim, &QVariantAnimation::valueChanged, [item](const QVariant &value) {
            item->setScale(value.toReal());
        });

        scaleAnim->start(QAbstractAnimation::DeleteWhenStopped);
     };

     animateScale(nodeA->circle);
     animateScale(nodeB->circle);

     // 延迟恢复原色
     QTimer::singleShot(600, [=]() {
        nodeA->circle->setBrush(QColor("#707070"));
        nodeB->circle->setBrush(QColor("#707070"));
     });

     // 值交换 + 更新 label
     std::swap(nodeA->value, nodeB->value);
     nodeA->label->setPlainText(QString::number(nodeA->value));
     nodeB->label->setPlainText(QString::number(nodeB->value));
}

void VisualHeap::animateNode(VisualHeapNode* node, QPointF endPos, int duration) {
    QPointF startPos = node->circle->pos();

    auto* anim = new QVariantAnimation();
    anim->setStartValue(startPos);
    anim->setEndValue(endPos);
    anim->setDuration(duration);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    connect(anim, &QVariantAnimation::valueChanged, this, [=](const QVariant &value) {
        QPointF pos = value.toPointF();
        node->circle->setPos(pos);
        node->label->setPos(pos.x() - node->label->boundingRect().width() / 2,
                            pos.y() - node->label->boundingRect().height() / 2);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
