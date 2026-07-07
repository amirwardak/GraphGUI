#pragma once
#include <QGraphicsLineItem>

class VertexItem;

class EdgeItem : public QGraphicsLineItem
{
public:
    EdgeItem(VertexItem* v1, VertexItem* v2, QGraphicsItem* parent = nullptr);
    ~EdgeItem();

    void updateFromVertices();
    VertexItem* getV1() const { return v1; }
    VertexItem* getV2() const { return v2; }
    double weight() const { return cachedWeight; }

private:
    VertexItem* v1;
    VertexItem* v2;
    double cachedWeight{};
};
