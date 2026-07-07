#pragma once
#include <QGraphicsScene>

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphScene(QObject* parent = nullptr);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
signals:
    void vertexClicked(int id);
};
