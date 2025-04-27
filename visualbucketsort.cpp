#include "visualbucketsort.h"

VisualBucketSort::VisualBucketSort(QGraphicsScene* scene, QSize areaSize)
    : m_scene(scene), m_areaSize(areaSize) {

    int baseY = m_areaSize.height() - 200;
    for (int i = 0; i < 6; ++i) {
        int bucketY = baseY + i * 40;
        auto label = m_scene->addText(QString("Bucket %1").arg(i), QFont("Segoe UI", 12, QFont::Bold));
        label->setDefaultTextColor(Qt::white);

        QRectF rect = label->boundingRect();
        label->setPos(40, bucketY + (30 - rect.height()) / 2);  // 垂直居中对齐元素
        m_bucketLabels[i] = label;
    }
}

void VisualBucketSort::clear() {
    m_nodes.clear();
    m_buckets.clear();
    m_bucketLabels.clear();
}

QPointF VisualBucketSort::getTopArrayPos(int index) const {
    return QPointF(60 + index * 50, 200);
}

QPointF VisualBucketSort::getBucketPos(int bucketIdx, int localIndex) const {
    int x = 150 + localIndex * 40;
    int y = m_areaSize.height() - 200 + bucketIdx * 40;
    return QPointF(x, y);
}

void VisualBucketSort::insertValue(int value) {
    int index = m_nodes.size();
    QPointF pos = getTopArrayPos(index);

    auto box = new QGraphicsRectItem(0, 0, 40, 40);
    box->setBrush(QColor("#707070"));
    box->setPen(QPen(QColor("#E0E0E0"), 2));
    box->setPos(pos);
    m_scene->addItem(box);

    auto label = new QGraphicsTextItem(QString::number(value));
    label->setFont(QFont("Segoe UI", 12, QFont::Bold));
    label->setDefaultTextColor(Qt::white);
    QRectF rect = label->boundingRect();
    label->setPos(pos.x() + (40 - rect.width()) / 2, pos.y() + (40 - rect.height()) / 2);
    m_scene->addItem(label);

    auto node = new VisualNode(value, box, label, pos);
    m_nodes.append(node);
}

void VisualBucketSort::assignToBucket(int value, int bucketIdx) {
    for (auto* node : m_nodes) {
        if (node->value == value && node->bucket == -1) {
            node->bucket = bucketIdx;
            int localIdx = m_buckets[bucketIdx].size();
            QPointF target = getBucketPos(bucketIdx, localIdx);
            animateMove(node, target, QSizeF(30, 30));  // 缩小为 30x30
            animateHighlight(node, QColor("#707070"));
            m_buckets[bucketIdx].append(node);
            break;
        }
    }
}

void VisualBucketSort::swapInBucket(int i, int j, int bucketId) {
    if (!m_buckets.contains(bucketId)) return;
    auto& vec = m_buckets[bucketId];
    if (i < 0 || j < 0 || i >= vec.size() || j >= vec.size() || i == j) return;

    auto nodeA = vec[i];
    auto nodeB = vec[j];

    QPointF posA = getBucketPos(bucketId, j);
    QPointF posB = getBucketPos(bucketId, i);

    QSizeF size(30, 30); // 缩小尺寸

    auto animA = new QVariantAnimation();
    animA->setStartValue(nodeA->box->pos());
    animA->setEndValue(posA);
    animA->setDuration(300);
    animA->setEasingCurve(QEasingCurve::OutCubic);
    connect(animA, &QVariantAnimation::valueChanged, [=](const QVariant& v) {
        QPointF pos = v.toPointF();
        nodeA->box->setPos(pos);
        nodeA->box->setRect(QRectF(0, 0, size.width(), size.height()));
        QRectF textRect = nodeA->label->boundingRect();
        nodeA->label->setPos(pos.x() + (size.width() - textRect.width()) / 2,
                             pos.y() + (size.height() - textRect.height()) / 2);
    });

    auto animB = new QVariantAnimation();
    animB->setStartValue(nodeB->box->pos());
    animB->setEndValue(posB);
    animB->setDuration(300);
    animB->setEasingCurve(QEasingCurve::OutCubic);
    connect(animB, &QVariantAnimation::valueChanged, [=](const QVariant& v) {
        QPointF pos = v.toPointF();
        nodeB->box->setPos(pos);
        nodeB->box->setRect(QRectF(0, 0, size.width(), size.height()));
        QRectF textRect = nodeB->label->boundingRect();
        nodeB->label->setPos(pos.x() + (size.width() - textRect.width()) / 2,
                             pos.y() + (size.height() - textRect.height()) / 2);
    });

    animA->start(QAbstractAnimation::DeleteWhenStopped);
    animB->start(QAbstractAnimation::DeleteWhenStopped);

    std::swap(vec[i], vec[j]); // 更新逻辑顺序
}

void VisualBucketSort::markFinal(int value) {
    static int insertIndex = 0;
    for (auto* node : m_nodes) {
        if (node->value == value) {
            QPointF pos = getTopArrayPos(insertIndex++);
            animateMove(node, pos, QSizeF(40, 40));  // 恢复大小
            animateHighlight(node, QColor("#98C379"));
            break;
        }
    }
}

void VisualBucketSort::animateMove(VisualNode* node, QPointF target, QSizeF targetSize) {
    auto anim = new QVariantAnimation();
    anim->setStartValue(node->box->pos());
    anim->setEndValue(target);
    anim->setDuration(400);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    QRectF targetRect(0, 0, targetSize.width(), targetSize.height());

    connect(anim, &QVariantAnimation::valueChanged, [=](const QVariant& v) {
        QPointF pos = v.toPointF();
        node->box->setPos(pos);
        node->box->setRect(targetRect);

        QRectF textRect = node->label->boundingRect();
        node->label->setPos(pos.x() + (targetSize.width() - textRect.width()) / 2,
                            pos.y() + (targetSize.height() - textRect.height()) / 2);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void VisualBucketSort::animateHighlight(VisualNode* node, const QColor& color) {
    node->box->setBrush(color);
    auto anim = new QVariantAnimation();
    anim->setStartValue(1.0);
    anim->setEndValue(1.1);
    anim->setDuration(300);
    anim->setEasingCurve(QEasingCurve::OutBack);

    connect(anim, &QVariantAnimation::valueChanged, [bar = node->box](const QVariant& v) {
        bar->setScale(v.toDouble());
    });
    connect(anim, &QVariantAnimation::finished, [bar = node->box]() {
        bar->setScale(1.0);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
