#include "graphscene.h"
#include "vertexitem.h"
#include <QGraphicsSceneMouseEvent>

GraphScene::GraphScene(QObject* parent)
    : QGraphicsScene(parent)
{
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    while (item) {
        if (auto* vertex = dynamic_cast<VertexItem*>(item)) {
            emit vertexClicked(vertex->getId());
            break;
        }
        item = item->parentItem();
    }
    QGraphicsScene::mousePressEvent(event);
}
