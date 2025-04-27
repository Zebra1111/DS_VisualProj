#include "visualbubblesort.h"

VisualBubbleSort::VisualBubbleSort(QGraphicsScene* scene, QSize areaSize)
    : m_scene(scene), m_areaSize(areaSize) {}

void VisualBubbleSort::clear() {
    m_nodes.clear();
}

QPointF VisualBubbleSort::getBarPosition(int index, int barHeight) const {
    const int spacing = 16;
    const int barWidth = 36;
    int x = index * (barWidth + spacing) + (m_areaSize.width() - (barWidth + spacing) * 10) / 2;
    int y = m_areaSize.height() - barHeight - 50;
    return QPointF(x, y);
}

void VisualBubbleSort::insertValue(int value) {
    const int barWidth = 36;
    int index = m_nodes.size();
    int maxHeight = m_areaSize.height() - 140;
    int barHeight = qMin(maxHeight, value * 4);

    QPointF pos = getBarPosition(index, barHeight);

    auto bar = new QGraphicsRectItem(0, 0, barWidth, barHeight);
    bar->setBrush(QColor("#707070"));
    bar->setPen(QPen(QColor("#E0E0E0"), 2));
    bar->setZValue(1);
    bar->setOpacity(0);
    bar->setScale(0.7);
    bar->setPos(pos);
    m_scene->addItem(bar);

    auto label = new QGraphicsTextItem(QString::number(value));
    label->setDefaultTextColor(QColor("#ABB2BF"));
    label->setFont(QFont("Segoe UI", 12, QFont::Bold));
    label->setZValue(2);
    label->setOpacity(0);
    label->setPos(pos.x() + barWidth / 2 - label->boundingRect().width() / 2, m_areaSize.height() - 35);
    m_scene->addItem(label);

    m_nodes.append(new VisualNode{value, bar, label});

    auto anim1 = new QVariantAnimation();
    anim1->setStartValue(0.0);
    anim1->setEndValue(1.0);
    anim1->setDuration(400);
    anim1->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim1, &QVariantAnimation::valueChanged, [bar, label](const QVariant& v) {
        bar->setOpacity(v.toDouble());
        label->setOpacity(v.toDouble());
    });
    anim1->start(QAbstractAnimation::DeleteWhenStopped);

    auto anim2 = new QVariantAnimation();
    anim2->setStartValue(0.7);
    anim2->setEndValue(1.0);
    anim2->setDuration(400);
    anim2->setEasingCurve(QEasingCurve::OutBack);
    connect(anim2, &QVariantAnimation::valueChanged, [bar](const QVariant& v) {
        bar->setScale(v.toDouble());
    });
    anim2->start(QAbstractAnimation::DeleteWhenStopped);
}

void VisualBubbleSort::compare(int i, int j) {
    QColor highlightColor("#E5C07B");
    animateTemporaryHighlight(i, highlightColor);
    animateTemporaryHighlight(j, highlightColor);
}

void VisualBubbleSort::animateTemporaryHighlight(int index, const QColor& highlightColor) {
    if (index < 0 || index >= m_nodes.size()) return;

    auto node = m_nodes[index];
    QColor originalColor("#707070");

    // 设置比较颜色
    node->bar->setBrush(QBrush(highlightColor));

    // 创建动画，持续300毫秒后恢复原色
    QTimer::singleShot(300, [node, originalColor]() {
        node->bar->setBrush(QBrush(originalColor));
    });

    // 添加一个轻微缩放动画增加视觉效果
    auto anim = new QVariantAnimation();
    anim->setStartValue(1.0);
    anim->setEndValue(1.1);
    anim->setDuration(150); // 缩短动画更明显
    anim->setEasingCurve(QEasingCurve::OutBack);

    connect(anim, &QVariantAnimation::valueChanged, [bar = node->bar](const QVariant& value) {
        bar->setScale(value.toDouble());
    });

    connect(anim, &QVariantAnimation::finished, [bar = node->bar]() {
        bar->setScale(1.0);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}


void VisualBubbleSort::swap(int i, int j) {
    if (i < 0 || j < 0 || i >= m_nodes.size() || j >= m_nodes.size() || i == j) return;

    auto nodeA = m_nodes[i];
    auto nodeB = m_nodes[j];

    int heightA = nodeA->bar->rect().height();
    int heightB = nodeB->bar->rect().height();

    QPointF targetPosA = getBarPosition(j, heightA);
    QPointF targetPosB = getBarPosition(i, heightB);

    // 创建同步动画组
    auto group = new QParallelAnimationGroup();

    auto animA = new QVariantAnimation();
    animA->setDuration(400);
    animA->setStartValue(nodeA->bar->pos());
    animA->setEndValue(targetPosA);
    animA->setEasingCurve(QEasingCurve::OutCubic);
    connect(animA, &QVariantAnimation::valueChanged, [=](const QVariant& value) {
        nodeA->bar->setPos(value.toPointF());
        nodeA->label->setPos(QPointF(value.toPointF().x() + 18 - nodeA->label->boundingRect().width() / 2, m_areaSize.height() - 35));
    });
    group->addAnimation(animA);

    auto animB = new QVariantAnimation();
    animB->setDuration(400);
    animB->setStartValue(nodeB->bar->pos());
    animB->setEndValue(targetPosB);
    animB->setEasingCurve(QEasingCurve::OutCubic);
    connect(animB, &QVariantAnimation::valueChanged, [=](const QVariant& value) {
        nodeB->bar->setPos(value.toPointF());
        nodeB->label->setPos(QPointF(value.toPointF().x() + 18 - nodeB->label->boundingRect().width() / 2, m_areaSize.height() - 35));
    });
    group->addAnimation(animB);

    connect(group, &QParallelAnimationGroup::finished, [=]() {
        std::swap(m_nodes[i], m_nodes[j]);
        group->deleteLater();
    });

    group->start();
}

void VisualBubbleSort::markSorted(int index) {
    animateHighlight(index, QColor("#98C379"));
}

void VisualBubbleSort::animateHighlight(int index, const QColor& color) {
    if (index < 0 || index >= m_nodes.size()) return;
    auto node = m_nodes[index];
    node->bar->setBrush(QBrush(color));

    auto anim = new QVariantAnimation();
    anim->setStartValue(1.0);
    anim->setEndValue(1.1);
    anim->setDuration(300);
    anim->setEasingCurve(QEasingCurve::OutBack);

    connect(anim, &QVariantAnimation::valueChanged, [bar = node->bar](const QVariant& value) {
        bar->setScale(value.toDouble());
    });
    connect(anim, &QVariantAnimation::finished, [bar = node->bar]() {
        bar->setScale(1.0);
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
