#include "visualsinglylist.h"

VisualSinglyLinkedList::VisualSinglyLinkedList(QGraphicsScene* scene, QGraphicsView* view, QSize areaSize)
    : scene(scene), view(view), areaSize(areaSize) {}

void VisualSinglyLinkedList::clear() {
    nodes.clear();
}

void VisualSinglyLinkedList::insertAtHeadAnimated(int value) {
    const int nodeWidth = 60;
    const int nodeHeight = 40;
    const int y = 100;
    QPointF pos(100, y);

    auto* widget = new QGraphicsWidget();
    scene->addItem(widget);

    auto* rect = new QGraphicsRectItem(-nodeWidth / 2, -nodeHeight / 2, nodeWidth, nodeHeight, widget);
    rect->setPen(QPen(QColor("#ABB2BF"), 2));
    rect->setBrush(QColor("#707070"));

    auto* text = new QGraphicsTextItem(QString::number(value), widget);
    text->setFont(QFont("Segoe UI", 13, QFont::Bold));
    text->setDefaultTextColor(Qt::white);
    QRectF rectText = text->boundingRect();
    text->setPos(-rectText.width() / 2, -rectText.height() / 2);

    widget->setScale(0);

    auto* node = new VisualSinglyNode{value, pos, widget, rect, text, nullptr, nullptr};
    nodes.prepend(node);

    animateScale(widget, 0.0, 1.0, 500, QEasingCurve::OutBack);
    updateLayout();
    resetView();
}

void VisualSinglyLinkedList::insertAtTailAnimated(int value) {
    const int nodeWidth = 60;
    const int nodeHeight = 40;
    const int y = 100;
    int spacing = 120;
    QPointF pos(100 + nodes.size() * spacing, y);

    auto* widget = new QGraphicsWidget();
    scene->addItem(widget);

    auto* rect = new QGraphicsRectItem(-nodeWidth / 2, -nodeHeight / 2, nodeWidth, nodeHeight, widget);
    rect->setPen(QPen(QColor("#ABB2BF"), 2));
    rect->setBrush(QColor("#707070"));

    auto* text = new QGraphicsTextItem(QString::number(value), widget);
    text->setFont(QFont("Segoe UI", 13, QFont::Bold));
    text->setDefaultTextColor(Qt::white);
    QRectF rectText = text->boundingRect();
    text->setPos(-rectText.width() / 2, -rectText.height() / 2);

    widget->setScale(0);

    auto* node = new VisualSinglyNode{value, pos, widget, rect, text, nullptr, nullptr};
    nodes.append(node);

    animateScale(widget, 0.0, 1.0, 500, QEasingCurve::OutBack);
    updateLayout();
    resetView();
}

void VisualSinglyLinkedList::deleteValueAnimated(int value) {
    for (int i = 0; i < nodes.size(); ++i) {
        VisualSinglyNode* node = nodes[i];
        if (node->value == value) {
            removeConnection(node);
            animateScale(node->widget, 1.0, 0.0, 400, QEasingCurve::InBack, [=]() {
                scene->removeItem(node->widget);
                delete node;
                nodes.removeAt(i);
                updateLayout();
                resetView();
            });
            return;
        }
    }
}

void VisualSinglyLinkedList::rebuildFromList(const QList<int>& values) {
    clear();
    for (int i = values.size() - 1; i >= 0; --i) {
        insertAtHeadAnimated(values[i]);
    }
}

bool VisualSinglyLinkedList::findAndhighlight(int value) {
    if(nodes.size() != 0)
    {
        index = 0;
        VisualSinglyNode* node = nodes[index];
        QGraphicsItem* item = node->widget;
        animateScale(item, 1.0, 1.5, 200, QEasingCurve::OutBack, [=]() {
            animateScale(item, 1.5, 1.0, 200, QEasingCurve::InCubic);
        });
        if (node->value == value) {
            node->shape->setBrush(QColor("#98C379"));
            index = 0;
            return true;
        }
        if(index < nodes.size()) return true;
    }
    return false;
}

bool VisualSinglyLinkedList::nextAndhighlight(int value)
{
    index++;
    if(index < nodes.size())
    {
        VisualSinglyNode* node = nodes[index];
        QGraphicsItem* item = node->widget;
        animateScale(item, 1.0, 1.5, 200, QEasingCurve::OutBack, [=]() {
            animateScale(item, 1.5, 1.0, 200, QEasingCurve::InCubic);
        });
        if (node->value == value) {
            node->shape->setBrush(QColor("#98C379"));
            index = 0;
            return true;
        }
    }
    else
        return false;
    return true;
}

void VisualSinglyLinkedList::updateLayout() {
    const int spacing = 120;
    const int nodeWidth = 60;
    const int y = 100;
    QPointF base(100, y);

    for (int i = 0; i < nodes.size(); ++i) {
        VisualSinglyNode* node = nodes[i];
        QPointF pos = base + QPointF(i * spacing, 0);
        node->pos = pos;

        animateMove(node->widget, pos, 300);

        removeConnection(node);

        if (i == 0) {
            node->shape->setPen(QPen(QColor("#E06C75"), 2));
        } else {
            node->shape->setPen(QPen(QColor("#ABB2BF"), 2));
        }

        if (i < nodes.size() - 1) {
            QPointF start = pos + QPointF(nodeWidth / 2, 0);
            QPointF end = base + QPointF((i + 1) * spacing - nodeWidth / 2, 0);

            node->lineToNext = scene->addLine(QLineF(start, end), QPen(QColor("#ABB2BF"), 2));
            node->lineToNext->setZValue(0);

            QPointF direction = end - start;
            qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
            if (length > 0) {
                QPointF unit = direction / length;
                QPointF arrowTip = end;
                QPointF arrowLeft = end - unit * 10 + QPointF(-unit.y(), unit.x()) * 6;
                QPointF arrowRight = end - unit * 10 - QPointF(-unit.y(), unit.x()) * 6;

                QPolygonF arrowHead;
                arrowHead << arrowTip << arrowLeft << arrowRight;

                node->arrowToNext = scene->addPolygon(arrowHead, QPen(Qt::NoPen), QBrush(QColor("#ABB2BF")));
                node->arrowToNext->setZValue(0);
            }
        }
    }
    resetView();
}

void VisualSinglyLinkedList::resetView() {
    QRectF bounds = scene->itemsBoundingRect();
    if (bounds.isEmpty()) return;

    QRectF margin = bounds.adjusted(-30, -30, 30, 30);
    view->resetTransform();

    QSizeF viewSize = view->mapToScene(view->viewport()->rect()).boundingRect().size();
    if (margin.width() > viewSize.width() || margin.height() > viewSize.height()) {
        view->fitInView(margin, Qt::KeepAspectRatio);
    }
    view->centerOn(bounds.center());
}

void VisualSinglyLinkedList::removeConnection(VisualSinglyNode* node) {
    if (node->lineToNext) {
        scene->removeItem(node->lineToNext);
        delete node->lineToNext;
        node->lineToNext = nullptr;
    }
    if (node->arrowToNext) {
        scene->removeItem(node->arrowToNext);
        delete node->arrowToNext;
        node->arrowToNext = nullptr;
    }
}

void VisualSinglyLinkedList::animateMove(QGraphicsItem* item, QPointF end, int duration) {
    auto* anim = new QPropertyAnimation();
    anim->setTargetObject(static_cast<QGraphicsObject*>(item));
    anim->setPropertyName("pos");
    anim->setDuration(duration);
    anim->setEndValue(end);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void VisualSinglyLinkedList::animateScale(QGraphicsItem* item, qreal start, qreal end,
                                          int duration, QEasingCurve curve, std::function<void()> onFinish) {
    auto* anim = new QPropertyAnimation();
    anim->setTargetObject(static_cast<QGraphicsObject*>(item));
    anim->setPropertyName("scale");
    anim->setDuration(duration);
    anim->setStartValue(start);
    anim->setEndValue(end);
    anim->setEasingCurve(curve);

    if (onFinish) {
        QObject::connect(anim, &QPropertyAnimation::finished, onFinish);
    }

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
