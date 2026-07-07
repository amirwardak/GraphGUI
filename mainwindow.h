#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include "Graph.h"
#include <QGraphicsScene>
#include <QList>

class VertexItem;

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

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onOpenFile();
    void onSaveAs();

private:
    Ui::MainWindow *ui;
    Graph graph;
    QGraphicsScene* scene;
    QList<VertexItem*> vertices;

    void loadGraph(const QString& path);
};
#endif // MAINWINDOW_H
