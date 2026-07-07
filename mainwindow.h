#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include "Graph.h"
#include <QGraphicsScene>
#include <QList>

class VertexItem;
class EdgeItem;
class QAction;
class QTimer;
class QGraphicsEllipseItem;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void highlightPath(const QVector<int>& path);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onOpenFile();
    void onSaveAs();
    void onVertexClicked(int id);
    void onRunDijkstra();
    void onReset();
    void onAnimateCar();
    void advanceCar();

private:
    Ui::MainWindow *ui;
    Graph graph;
    QGraphicsScene* scene;
    QList<VertexItem*> vertices;
    QList<EdgeItem*> edges;

    void loadGraph(const QString& path);
    void clearPathHighlight();

    int selectedStart = -1;
    int selectedEnd = -1;
    QVector<int> pathResult;
    bool hasPathResult = false;

    QAction* resetAction = nullptr;
    QAction* animateAction = nullptr;

    QGraphicsEllipseItem* carItem = nullptr;
    QTimer* carTimer = nullptr;
    int carEdgeIndex = 0;
    double carProgress = 0.0;
    double carStepSize = 0.0;
};

#endif // MAINWINDOW_H
