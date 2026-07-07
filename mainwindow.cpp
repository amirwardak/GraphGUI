#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vertexitem.h"
#include "edgeitem.h"
#include "graphscene.h"

#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QTimer>
#include <QGraphicsEllipseItem>
#include <QtMath>

#include <fstream>
#include <iomanip>
#include <limits>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("GraphGUI - Graph Visualization");

    auto* graphScene = new GraphScene(this);
    scene = graphScene;
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(graphScene, &GraphScene::vertexClicked, this, &MainWindow::onVertexClicked);

    QToolBar* toolBar = addToolBar("Actions");
    QAction* findAction = toolBar->addAction("Найти путь");
    connect(findAction, &QAction::triggered, this, &MainWindow::onRunDijkstra);

    resetAction = toolBar->addAction("Сброс");
    resetAction->setEnabled(false);
    connect(resetAction, &QAction::triggered, this, &MainWindow::onReset);

    animateAction = toolBar->addAction("Машинка");
    animateAction->setEnabled(false);
    connect(animateAction, &QAction::triggered, this, &MainWindow::onAnimateCar);

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
    if (carTimer) {
        carTimer->stop();
        delete carTimer;
        carTimer = nullptr;
    }
    if (carItem) {
        scene->removeItem(carItem);
        delete carItem;
        carItem = nullptr;
    }

    scene->clear();
    vertices.clear();
    edges.clear();

    selectedStart = -1;
    selectedEnd = -1;
    pathResult.clear();
    hasPathResult = false;
    resetAction->setEnabled(false);
    animateAction->setEnabled(false);

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
        edges.append(edge);
    }

    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::onVertexClicked(int id)
{
    if (vertices.isEmpty()) return;

    if (selectedStart == -1) {
        selectedStart = id;
        vertices[id]->setBrush(QBrush(Qt::yellow));
    } else if (selectedEnd == -1) {
        if (id != selectedStart) {
            selectedEnd = id;
            vertices[id]->setBrush(QBrush(Qt::yellow));
        }
    } else {
        clearPathHighlight();
        selectedStart = id;
        selectedEnd = -1;
        pathResult.clear();
        hasPathResult = false;
        resetAction->setEnabled(false);
        animateAction->setEnabled(false);
        vertices[id]->setBrush(QBrush(Qt::yellow));
    }
}

void MainWindow::onRunDijkstra()
{
    if (vertices.isEmpty()) {
        QMessageBox::information(this, "Dijkstra", "Сначала загрузите граф.");
        return;
    }

    if (selectedStart == -1 || selectedEnd == -1) {
        QMessageBox::information(this, "Dijkstra", "Выберите начальную и конечную вершины.");
        return;
    }

    if (selectedStart == selectedEnd) {
        QMessageBox::information(this, "Dijkstra", "Начальная и конечная вершины должны быть разными.");
        return;
    }

    int n = vertices.size();
    const double INF = std::numeric_limits<double>::max();
    QVector<QVector<double>> weights(n, QVector<double>(n, INF));

    for (auto* edge : edges) {
        int v1 = edge->getV1()->getId();
        int v2 = edge->getV2()->getId();
        double w = edge->weight();
        weights[v1][v2] = w;
        weights[v2][v1] = w;
    }

    pathResult = graph.dijkstra(selectedStart, selectedEnd, weights);

    if (pathResult.isEmpty()) {
        QMessageBox::information(this, "Dijkstra", "Вершины не соединены путём.");
        return;
    }

    hasPathResult = true;
    highlightPath(pathResult);
    resetAction->setEnabled(true);
    animateAction->setEnabled(true);
}

void MainWindow::onReset()
{
    clearPathHighlight();

    if (carTimer) {
        carTimer->stop();
        delete carTimer;
        carTimer = nullptr;
    }
    if (carItem) {
        scene->removeItem(carItem);
        delete carItem;
        carItem = nullptr;
    }

    selectedStart = -1;
    selectedEnd = -1;
    pathResult.clear();
    hasPathResult = false;
    resetAction->setEnabled(false);
    animateAction->setEnabled(false);
}

void MainWindow::onAnimateCar()
{
    if (!hasPathResult || pathResult.isEmpty()) {
        QMessageBox::information(this, "Анимация", "Путь не найден. Сначала примените алгоритм.");
        return;
    }

    if (carItem) {
        scene->removeItem(carItem);
        delete carItem;
        carItem = nullptr;
    }
    if (carTimer) {
        carTimer->stop();
        delete carTimer;
        carTimer = nullptr;
    }

    carItem = new QGraphicsEllipseItem(-7, -7, 14, 14);
    carItem->setBrush(QBrush(Qt::red));
    carItem->setPen(QPen(Qt::darkRed, 2));
    carItem->setZValue(100);

    QPointF startPos = vertices[pathResult[0]]->scenePos();
    carItem->setPos(startPos);
    scene->addItem(carItem);

    carEdgeIndex = 0;
    carProgress = 0.0;

    int v1 = pathResult[0];
    int v2 = pathResult[1];
    QPointF p1 = vertices[v1]->scenePos();
    QPointF p2 = vertices[v2]->scenePos();
    double edgeLen = qSqrt(QPointF::dotProduct(p2 - p1, p2 - p1));
    double speed = 150.0;
    carStepSize = (speed * 0.03) / edgeLen;

    carTimer = new QTimer(this);
    connect(carTimer, &QTimer::timeout, this, &MainWindow::advanceCar);
    carTimer->start(30);
}

void MainWindow::advanceCar()
{
    if (carEdgeIndex >= pathResult.size() - 1) {
        carTimer->stop();
        return;
    }

    carProgress += carStepSize;

    if (carProgress >= 1.0) {
        carEdgeIndex++;
        if (carEdgeIndex >= pathResult.size() - 1) {
            carTimer->stop();
            return;
        }
        carProgress = 0.0;
        int n1 = pathResult[carEdgeIndex];
        int n2 = pathResult[carEdgeIndex + 1];
        QPointF q1 = vertices[n1]->scenePos();
        QPointF q2 = vertices[n2]->scenePos();
        double edgeLen = qSqrt(QPointF::dotProduct(q2 - q1, q2 - q1));
        carStepSize = (100.0 * 0.03) / edgeLen;
    }

    int v1 = pathResult[carEdgeIndex];
    int v2 = pathResult[carEdgeIndex + 1];
    QPointF p1 = vertices[v1]->scenePos();
    QPointF p2 = vertices[v2]->scenePos();

    carItem->setPos(p1 + carProgress * (p2 - p1));
}

void MainWindow::clearPathHighlight()
{
    for (auto* vertex : vertices)
        vertex->setBrush(QBrush(Qt::lightGray));
    for (auto* edge : edges)
        edge->setPen(QPen(QColor("#00BCD4"), 2.5));
}

void MainWindow::highlightPath(const QVector<int>& path)
{
    for (int vIdx : path)
        vertices[vIdx]->setBrush(QBrush(Qt::green));

    for (int i = 0; i < path.size() - 1; ++i) {
        int v1 = path[i];
        int v2 = path[i + 1];
        for (auto* edge : edges) {
            int ev1 = edge->getV1()->getId();
            int ev2 = edge->getV2()->getId();
            if ((ev1 == v1 && ev2 == v2) || (ev1 == v2 && ev2 == v1)) {
                edge->setPen(QPen(Qt::green, 3));
                break;
            }
        }
    }
}
