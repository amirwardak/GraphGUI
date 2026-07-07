#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vertexitem.h"
#include "edgeitem.h"

#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QtMath>

#include <fstream>
#include <iomanip>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("GraphGUI - Graph Visualization");

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QMenu* fileMenu = menuBar()->addMenu("&File");
    QAction* openAction = fileMenu->addAction("&Open Graph...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFile);

    QAction* saveAction = fileMenu->addAction("&Save As...");
    saveAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveAs);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    if (!vertices.isEmpty())
        ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::onOpenFile()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Open Graph File", QString(),
        "Graph Files (*.txt);;All Files (*)");
    if (!path.isEmpty())
        loadGraph(path);
}

void MainWindow::onSaveAs()
{
    if (vertices.isEmpty()) {
        QMessageBox::information(this, "Save", "No graph loaded.");
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this, "Save Adjacency Matrix", QString(),
        "Text Files (*.txt);;All Files (*)");
    if (path.isEmpty()) return;

    std::ofstream fout(path.toStdString());
    if (!fout.is_open()) {
        QMessageBox::warning(this, "Error", "Failed to save file:\n" + path);
        return;
    }

    int n = vertices.size();
    fout << n << "\n";

    for (int i = 0; i < n; ++i) {
        QPointF pi = vertices[i]->scenePos();
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                fout << "inf";
            } else {
                bool adjacent = false;
                for (int neighbor : graph.getAdjList()[i]) {
                    if (neighbor == j) { adjacent = true; break; }
                }
                if (adjacent) {
                    QPointF pj = vertices[j]->scenePos();
                    double dx = pj.x() - pi.x();
                    double dy = pj.y() - pi.y();
                    double w = std::sqrt(dx * dx + dy * dy);
                    fout << std::fixed << std::setprecision(2) << w;
                } else {
                    fout << "inf";
                }
            }
            if (j < n - 1) fout << " ";
        }
        fout << "\n";
    }
}

void MainWindow::loadGraph(const QString& path)
{
    scene->clear();
    vertices.clear();

    if (!graph.loadFromFile(path.toStdString())) {
        QMessageBox::warning(this, "Error", "Failed to open file:\n" + path);
        return;
    }

    int n = graph.getSize();
    if (n == 0) return;

    double radius = 200.0;
    QPointF center(0, 0);

    for (int i = 0; i < n; ++i) {
        double angle = 2.0 * M_PI * i / n - M_PI / 2.0;
        double x = center.x() + radius * qCos(angle);
        double y = center.y() + radius * qSin(angle);
        auto* v = new VertexItem(i);
        v->setPos(x, y);
        scene->addItem(v);
        vertices.append(v);
    }

    for (const auto& [v1, v2] : graph.getEdges()) {
        auto* edge = new EdgeItem(vertices[v1], vertices[v2]);
        scene->addItem(edge);
        vertices[v1]->addEdge(edge);
        vertices[v2]->addEdge(edge);
    }

    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}
