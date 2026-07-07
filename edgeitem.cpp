#include "edgeitem.h"
#include "vertexitem.h"
#include <QPen>
#include <QtMath>

EdgeItem::EdgeItem(VertexItem* v1, VertexItem* v2, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), v1(v1), v2(v2) {
    setPen(QPen(QColor("#00BCD4"), 2.5));
    setZValue(-1);
    updateFromVertices();
}

EdgeItem::~EdgeItem() {}

void EdgeItem::updateFromVertices()
{
    QPointF c1 = v1->scenePos();
    QPointF c2 = v2->scenePos();

    setLine(QLineF(c1, c2));

    double dx = c2.x() - c1.x();
    double dy = c2.y() - c1.y();
    cachedWeight = qSqrt(dx * dx + dy * dy);
}
