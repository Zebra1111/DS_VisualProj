#include "visualstack.h"

VisualStack::VisualStack(QGraphicsScene* scene, QSize areaSize)
    : m_scene(scene), m_areaSize(areaSize) {}

void VisualStack::clear() {
    m_nodes.clear();
}

VisualStackNode* VisualStack::createNode(int value, QPointF pos) {
    QGraphicsWidget* widget = new QGraphicsWidget();
    widget->setPos(pos);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 150));
    widget->setGraphicsEffect(shadow);

    QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(widget);
    QLabel* label = new QLabel(QString::number(value));
    label->setAlignment(Qt::AlignCenter);
    QFont font("Segoe UI", 13, QFont::Bold);
    label->setFont(font);
    label->setStyleSheet("background-color:#707070; color:#E0E0E0; border-radius:12px;");
    label->setFixedSize(nodeSize);
    proxy->setWidget(label);
    proxy->setPos(-nodeSize.width()/2, -nodeSize.height()/2);

    m_scene->addItem(widget);

    return new VisualStackNode{value, widget, proxy, label};
}

void VisualStack::pushAnimated(int value) {
    QPointF startPos(m_areaSize.width()/2, nodeSize.height());
    auto node = (createNode(value, startPos));
    node->widget->setOpacity(0);
    node->widget->setScale(0.7);

    m_nodes.push_back(node);
    updateLayout();

    animateProperty(node->widget, "opacity", 0, 1, 500);
    animateProperty(node->widget, "scale", 0.7, 1.0, 500, QEasingCurve::OutBack);
}

void VisualStack::popAnimated() {
    if (m_nodes.isEmpty()) return;

    auto node = m_nodes.takeLast();
    QPointF endPos = node->widget->pos() - QPointF(0, nodeSize.height());

    animateProperty(node->widget, "opacity", 1, 0, 400);
    animateProperty(node->widget, "pos", node->widget->pos(), endPos, 400);
    animateProperty(node->widget, "scale", 1.0, 0.7, 400);

    m_scene->removeItem(node->widget);
    node->widget->deleteLater();
    delete node;
    updateLayout();

//    延迟的话可能会出现bug
//    QTimer::singleShot(100, [this, node]() {
//        m_scene->removeItem(node->widget);
//        node->widget->deleteLater();
//        delete node;
//        updateLayout();
//    });
}

void VisualStack::highlightTop() {
    if (m_nodes.isEmpty()) return;
    auto node = m_nodes.last();

    animateProperty(node->widget, "scale", 1.0, 1.2, 200, QEasingCurve::OutCubic);

    node->label->setStyleSheet("background-color:#E06C75; color:#FFFFFF; border-radius:12px;");
    QFont highlightFont("Segoe UI", 13, QFont::Bold);
    node->label->setFont(highlightFont);

    QTimer::singleShot(400, [this, node]() {
        animateProperty(node->widget, "scale", 1.2, 1.0, 200, QEasingCurve::OutCubic);

        node->label->setStyleSheet("background-color:#707070; color:#E0E0E0; border-radius:12px;");
        QFont originalFont("Segoe UI", 13, QFont::Bold);
        node->label->setFont(originalFont);
    });
}


void VisualStack::updateLayout() {
    int startX = m_areaSize.width() / 2;
    int startY = m_areaSize.height() - nodeSize.height()*2;

    for (int i = 0; i < m_nodes.size(); ++i) {
        auto node = m_nodes[i];
        QPointF targetPos(startX, startY - i * (nodeSize.height() + nodeSpacing));
        animateProperty(node->widget, "pos", node->widget->pos(), targetPos, 500);
    }
}

void VisualStack::animateProperty(QObject* obj, const QByteArray& property, QVariant start, QVariant end, int duration, QEasingCurve curve) {
    QPropertyAnimation* anim = new QPropertyAnimation(obj, property);
    anim->setDuration(duration);
    anim->setStartValue(start);
    anim->setEndValue(end);
    anim->setEasingCurve(curve);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
