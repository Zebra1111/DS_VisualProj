#ifndef VISUALSEARCH_H
#define VISUALSEARCH_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QVariantAnimation>
#include <QEasingCurve>
#include <QVector>
#include <QSize>
#include <QBrush>
#include <QPen>
#include <QString>

class VisualBinarySearch : public QObject {
    Q_OBJECT

public:
    VisualBinarySearch(QGraphicsScene* scene, QSize areaSize);

    void insertValue(int value);
    void highlightFound(int index);
    void highlightCheck(int left, int right, int mid);
    void highlightRange(int left, int right);
    void clear();

private:
    struct VisualNode {
        int value;
        QGraphicsRectItem* bar;
        QGraphicsTextItem* label;
    };

    QVector<VisualNode*> m_nodes;
    QGraphicsScene* m_scene;
    QSize m_areaSize;

    void animateHighlight(int index, const QColor& color);
};


#endif // VISUALSEARCH_H
