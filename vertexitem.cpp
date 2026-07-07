#include "vertexitem.h"
#include "edgeitem.h"
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>

VertexItem::VertexItem(int id, QGraphicsItem* parent)
    : QGraphicsEllipseItem(-20, -20, 40, 40, parent)
    , id(id)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setBrush(QBrush(Qt::lightGray));
    setPen(QPen(Qt::black, 2));

    auto* text = new QGraphicsTextItem(QString::number(id + 1), this);
    text->setDefaultTextColor(Qt::black);
    QRectF r = text->boundingRect();
    text->setPos(-r.width() / 2.0, -r.height() / 2.0);
}

QVariant VertexItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged) {
        for (auto* edge : edges)
            edge->updateFromVertices();
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}
