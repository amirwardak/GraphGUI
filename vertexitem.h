#pragma once
#include <QGraphicsEllipseItem>
#include <QList>

class EdgeItem;

class VertexItem : public QGraphicsEllipseItem
{
public:
    VertexItem(int id, QGraphicsItem* parent = nullptr);

    int getId() const { return id; }
    void addEdge(EdgeItem* edge) { edges.append(edge); }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int id;
    QList<EdgeItem*> edges;
};
