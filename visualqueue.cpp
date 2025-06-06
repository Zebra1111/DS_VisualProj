#include "visualqueue.h"


VisualQueue::VisualQueue(QGraphicsScene* scene, QSize areaSize)
    : m_scene(scene), m_areaSize(areaSize) {}

void VisualQueue::clear() {
    m_nodes.clear();
}

VisualQueueNode* VisualQueue::createNode(int value, QPointF pos) {
    QGraphicsWidget* widget = new QGraphicsWidget();
    widget->setPos(pos);

    QLabel* label = new QLabel(QString::number(value));
    label->setAlignment(Qt::AlignCenter);
    QFont font("Segoe UI", 12, QFont::Bold);
    label->setFont(font);
    label->setStyleSheet("background-color:#707070; color:#E0E0E0; border-radius:12px;");
    label->setFixedSize(nodeSize);

    QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(widget);
    proxy->setWidget(label);
    proxy->setPos(-nodeSize.width() / 2, -nodeSize.height() / 2);

    m_scene->addItem(widget);

    return new VisualQueueNode{value, widget, proxy, label};
}

void VisualQueue::enqueueAnimated(int value) {
    QPointF startPos(m_areaSize.width() + nodeSize.width(), m_areaSize.height() / 2);
    VisualQueueNode* node = createNode(value, startPos);
    node->widget->setOpacity(0);
    node->widget->setScale(0.7);

    m_nodes.push_back(node);
    updateLayout();

    animateProperty(node->widget, "opacity", 0, 1, 500);
    animateProperty(node->widget, "scale", 0.7, 1.0, 500, QEasingCurve::OutBack);
}

void VisualQueue::dequeueAnimated() {
    if (m_nodes.isEmpty()) return;

    VisualQueueNode* node = m_nodes.takeFirst();
    // 出队时结点向左移动逐渐消失
    QPointF endPos = node->widget->pos() - QPointF(nodeSize.width() * 1.5, 0);

    animateProperty(node->widget, "opacity", 1, 0, 400);
    animateProperty(node->widget, "pos", node->widget->pos(), endPos, 400);
    animateProperty(node->widget, "scale", 1.0, 0.7, 400);

    QTimer::singleShot(400, [this, node]() {
        m_scene->removeItem(node->widget);
        node->widget->deleteLater();
        delete node;
        updateLayout();
    });
}

void VisualQueue::updateLayout() {
    // 队列从画面中心偏左布局
    int startX = m_areaSize.width() * 0.4;
    int startY = m_areaSize.height() / 2;

    for (int i = 0; i < m_nodes.size(); ++i) {
        VisualQueueNode* node = m_nodes[i];
        QPointF targetPos(startX + i * (nodeSize.width() + nodeSpacing), startY);
        animateProperty(node->widget, "pos", node->widget->pos(), targetPos, 500);
    }
}

void VisualQueue::highlightFront() {
    if (m_nodes.isEmpty()) return;
    VisualQueueNode* node = m_nodes.first();

    animateProperty(node->widget, "scale", 1.0, 1.2, 200, QEasingCurve::OutCubic);

    node->label->setStyleSheet("background-color:#E06C75; color:#FFFFFF; border-radius:12px;");
    QFont highlightFont("Segoe UI", 12, QFont::Bold);
    node->label->setFont(highlightFont);

    QTimer::singleShot(400, [this, node]() {
        animateProperty(node->widget, "scale", 1.2, 1.0, 200, QEasingCurve::OutCubic);

        node->label->setStyleSheet("background-color:#707070; color:#E0E0E0; border-radius:12px;");
        QFont originalFont("Segoe UI", 12, QFont::Bold);
        node->label->setFont(originalFont);
    });
}

void VisualQueue::animateProperty(QObject* obj, const QByteArray& property, QVariant start, QVariant end, int duration, QEasingCurve curve) {
    QPropertyAnimation* anim = new QPropertyAnimation(obj, property);
    anim->setDuration(duration);
    anim->setStartValue(start);
    anim->setEndValue(end);
    anim->setEasingCurve(curve);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
