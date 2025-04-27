#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing);

    // 禁用滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 缩放锚点设置为鼠标位置
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // 鼠标拖动画布功能
    setDragMode(QGraphicsView::NoDrag);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0)
            scale(scaleFactor, scaleFactor);  // 放大
        else
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);  // 缩小
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isPanning = true;
        panStart = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (isPanning) {
        QPointF delta = mapToScene(panStart) - mapToScene(event->pos());
        translate(delta.x(), delta.y());
        panStart = event->pos();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isPanning) {
        isPanning = false;
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}
