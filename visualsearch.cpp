#include "visualsearch.h"

VisualBinarySearch::VisualBinarySearch(QGraphicsScene* scene, QSize areaSize)
    : m_scene(scene), m_areaSize(areaSize) {}

void VisualBinarySearch::clear() {
    m_nodes.clear();
}

void VisualBinarySearch::insertValue(int value) {
    const int spacing = 16;
    const int barWidth = 36;
    int x = (m_nodes.size()) * (barWidth + spacing) + (m_areaSize.width() - (barWidth + spacing) * qMax(1, m_nodes.size() + 1)) / 2;
    int maxHeight = m_areaSize.height() - 140;
    int barHeight = qMin(maxHeight, value * 4);
    int y = m_areaSize.height() - barHeight - 50;

    auto bar = m_scene->addRect(x, y, barWidth, barHeight, QPen(QColor("#E0E0E0"), 2), QBrush(QColor("#707070")));
    bar->setZValue(1);
    bar->setOpacity(0);
    bar->setScale(0.7);

    auto label = m_scene->addText(QString::number(value), QFont("Segoe UI", 12, QFont::Bold));
    label->setDefaultTextColor(QColor("#ABB2BF"));
    label->setZValue(2);
    label->setPos(x + barWidth / 2 - label->boundingRect().width() / 2, m_areaSize.height() - 35);
    label->setOpacity(0);

    m_nodes.append(new VisualNode{value, bar, label});

    auto anim1 = new QVariantAnimation();
    anim1->setStartValue(0.0);
    anim1->setEndValue(1.0);
    anim1->setDuration(400);
    anim1->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim1, &QVariantAnimation::valueChanged, [bar, label](const QVariant &v) {
        bar->setOpacity(v.toDouble());
        label->setOpacity(v.toDouble());
    });
    anim1->start(QAbstractAnimation::DeleteWhenStopped);

    auto anim2 = new QVariantAnimation();
    anim2->setStartValue(0.7);
    anim2->setEndValue(1.0);
    anim2->setDuration(400);
    anim2->setEasingCurve(QEasingCurve::OutBack);
    connect(anim2, &QVariantAnimation::valueChanged, [bar](const QVariant &v) {
        bar->setScale(v.toDouble());
    });
    anim2->start(QAbstractAnimation::DeleteWhenStopped);
}

void VisualBinarySearch::highlightRange(int left, int right) {
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (i >= left && i <= right) {
            m_nodes[i]->bar->setBrush(QColor("#282C34"));
        } else {
            m_nodes[i]->bar->setBrush(QColor("#707070"));
        }
    }
}

void VisualBinarySearch::highlightCheck(int left, int right, int mid) {
    highlightRange(left, right);
    animateHighlight(mid, QColor("#E5C07B"));
}

void VisualBinarySearch::highlightFound(int index) {
    auto node = m_nodes[index];
    node->bar->setBrush(QColor("#98C379"));
}

void VisualBinarySearch::animateHighlight(int index, const QColor& color) {
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
