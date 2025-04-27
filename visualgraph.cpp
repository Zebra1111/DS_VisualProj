#include "visualgraph.h"

VisualGraph::VisualGraph(QGraphicsScene *scene, QSize areaSize)
    : m_scene(scene), m_areaSize(areaSize) {}

VisualGraph::~VisualGraph() {
    clear();
}

void VisualGraph::clear() {
    m_nodes.clear();
    m_edges.clear();
}

void VisualGraph::autoLayoutNodes(int nodeCount) {
    clear();
    if (nodeCount <= 0) return;

    const QPointF center(m_areaSize.width() / 2, m_areaSize.height() / 2);
    const double radius = qMin(m_areaSize.width(), m_areaSize.height()) / 2.8;
    const double angleStep = 2 * M_PI / nodeCount;

    for (int i = 0; i < nodeCount; ++i) {
        double angle = i * angleStep;
        double x = center.x() + radius * qCos(angle);
        double y = center.y() + radius * qSin(angle);

        addNode(i, QPointF(x, y));
    }
}

void VisualGraph::autoLayoutNodes_Fd(int nodeCount) {
    clear();
    if (nodeCount <= 0) return;

    const QPointF center(m_areaSize.width() / 2, m_areaSize.height() / 4); // 上半部分中心点
    const double radius = qMin(m_areaSize.width(), m_areaSize.height() / 2) / 2.5; // 半径适当减小
    const double angleStep = 2 * M_PI / nodeCount;

    for (int i = 0; i < nodeCount; ++i) {
        double angle = i * angleStep;
        double x = center.x() + radius * qCos(angle);
        double y = center.y() + radius * qSin(angle);
        addNode(i, QPointF(x, y));
    }
}


VisualGraphNode* VisualGraph::createNode(int id, QPointF position) {
    auto ellipse = m_scene->addEllipse(position.x() - 25, position.y() - 25, 50, 50,
                                       QPen(QColor("#E0E0E0"), 3), QBrush(QColor("#707070")));

    ellipse->setZValue(1);
    auto shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 150));
    shadow->setOffset(QPointF(0, 3));
    ellipse->setGraphicsEffect(shadow);


    auto label = m_scene->addText(QString::number(id), QFont("Segoe UI", 13, QFont::Bold));
    label->setDefaultTextColor(Qt::white);
    label->setZValue(2);
    label->setPos(position.x() - label->boundingRect().width() / 2,
                  position.y() - label->boundingRect().height() / 2);

    return new VisualGraphNode{id, position, ellipse, label};
}

void VisualGraph::addNode(int id, QPointF position) {
    if (!m_nodes.contains(id)) {
        m_nodes[id] = createNode(id, position);
    }
}

void VisualGraph::createEdge(int src, int dest) {
    auto srcNode = m_nodes[src];
    auto destNode = m_nodes[dest];
    auto line = m_scene->addLine(QLineF(srcNode->position, destNode->position), QPen(QColor("#ABB2BF"), 3));
    line->setZValue(0);
    m_edges.append(new VisualGraphEdge{src, dest, line});
}

void VisualGraph::addEdge(int src, int dest) {
    if (m_nodes.contains(src) && m_nodes.contains(dest)) {
        createEdge(src, dest);
    }
}

void VisualGraph::addEdge(int src, int dest, int weight) {
    if (m_nodes.contains(src) && m_nodes.contains(dest)) {
        createEdge(src, dest);

        // 添加权重文本
        QPointF mid = (m_nodes[src]->position + m_nodes[dest]->position) / 2;
        auto text = m_scene->addText(QString::number(weight), QFont("Segoe UI", 12, QFont::Bold));
        text->setDefaultTextColor(Qt::white);
        text->setZValue(3);
        text->setPos(mid.x() - 10, mid.y() - 10);
    }
}

void VisualGraph::highlightNode(int id) {
    //resetHighlights();
    if (m_nodes.contains(id)) {
        auto node = m_nodes[id];
        node->ellipse->setBrush(QBrush(QColor("#98C379")));

        auto anim = new QVariantAnimation();
        anim->setStartValue(0.7);
        anim->setEndValue(1.0);
        anim->setDuration(300);
        anim->setEasingCurve(QEasingCurve::OutBack);

        connect(anim, &QVariantAnimation::valueChanged, [ellipse=node->ellipse](const QVariant &value){
            ellipse->setOpacity(value.toDouble());
        });

        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void VisualGraph::highlightEdge(int src, int dest) {
    for(auto edge : m_edges) {
        if((edge->src == src && edge->dest == dest) || (edge->src == dest && edge->dest == src)) {
            edge->line->setPen(QPen(QColor("#E5C07B"), 7));
        }
    }
}

void VisualGraph::Dj_highlightEdge(int src, int dest) {
    for(auto edge : m_edges) {
        if((edge->src == src && edge->dest == dest) || (edge->src == dest && edge->dest == src)) {
            edge->line->setPen(QPen(QColor("#E06C75"), 7));
        }
    }
}

void VisualGraph::initDistanceMatrix(int vertices, QVector<QVector<int>> initialMatrix) {
    currentDistances = initialMatrix;
    drawDistanceMatrix();
}

void VisualGraph::drawDistanceMatrix() {
    const int cellSize = 50;

    // 邻接矩阵尺寸
    int matrixSize = currentDistances.size();

    // 计算起始位置 (下半部分居中)
    const int matrixWidth = cellSize * matrixSize;
    const QPointF start((m_areaSize.width() - matrixWidth) / 2 + cellSize - 50,
                        m_areaSize.height() / 2 + 40 + cellSize);

    // 背景矩形 (包括边界标签的额外空间)
    matrixBackground = m_scene->addRect(start.x(), start.y(),
                                        matrixWidth, matrixWidth,
                                        QPen(Qt::black), QBrush(QColor("#282C34")));

    distanceMatrixTexts.resize(matrixSize);
    for (int i = 0; i < matrixSize; ++i) {
        distanceMatrixTexts[i].resize(matrixSize);
        for (int j = 0; j < matrixSize; ++j) {
            QString text = currentDistances[i][j] == INF ? "∞" : QString::number(currentDistances[i][j]);
            QGraphicsTextItem* item = m_scene->addText(text, QFont("Segoe UI", 10, QFont::Bold));
            item->setDefaultTextColor(Qt::white);
            item->setPos(start.x() + j * cellSize + 15, start.y() + i * cellSize + 15);
            distanceMatrixTexts[i][j] = item;
        }
    }

    // 新增结点编号
    for (int i = 0; i < matrixSize; ++i) {
        // 顶部编号 (横向)
        QGraphicsTextItem* topLabel = m_scene->addText(QString("V%1").arg(i), QFont("Segoe UI", 10, QFont::Bold));
        topLabel->setDefaultTextColor(QColor("#61AFEF"));
        topLabel->setPos(start.x() + i * cellSize + 15, start.y() - cellSize + 15);

        // 左侧编号 (纵向)
        QGraphicsTextItem* leftLabel = m_scene->addText(QString("V%1").arg(i), QFont("Segoe UI", 10, QFont::Bold));
        leftLabel->setDefaultTextColor(QColor("#61AFEF"));
        leftLabel->setPos(start.x() - cellSize + 15, start.y() + i * cellSize + 15);
    }
}

void VisualGraph::updateDistanceMatrix(int src, int dest, int newDist) {
    // 更新邻接矩阵的数据
   currentDistances[src][dest] = newDist;
   currentDistances[dest][src] = newDist;

   QString text = newDist == INF ? "∞" : QString::number(newDist);

   // 更新矩阵显示文本
   distanceMatrixTexts[src][dest]->setPlainText(text);
   distanceMatrixTexts[dest][src]->setPlainText(text);

   // 设置高亮颜色
   distanceMatrixTexts[src][dest]->setDefaultTextColor(QColor("#E06C75"));
   distanceMatrixTexts[dest][src]->setDefaultTextColor(QColor("#E06C75"));

   // 动画效果：字体颜色从高亮渐变回白色
   QVariantAnimation* anim = new QVariantAnimation(this);
   anim->setDuration(1200);
   anim->setStartValue(QColor("#E06C75"));
   anim->setEndValue(QColor("#FFFFFF"));
   anim->setEasingCurve(QEasingCurve::InOutQuad);

   connect(anim, &QVariantAnimation::valueChanged, [=](const QVariant &value) {
       distanceMatrixTexts[src][dest]->setDefaultTextColor(value.value<QColor>());
       distanceMatrixTexts[dest][src]->setDefaultTextColor(value.value<QColor>());
   });

   anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void VisualGraph::resetHighlights() {
    for(auto node : m_nodes) {
        node->ellipse->setBrush(QBrush(QColor("#707070")));
        node->ellipse->setScale(1.0);
    }
    for(auto edge : m_edges) {
        edge->line->setPen(QPen(QColor("#ABB2BF"), 2));
    }
}
