#pragma once
#include <QVector>
#include <QPair>
#include <string>

class Graph
{
private:
    int size{};
    QVector<QVector<int>> adjList;
    QVector<QPair<int, int>> edgeList;
public:
    Graph() = default;
    bool loadFromFile(const std::string& path);
    int getSize() const { return size; }
    const QVector<QVector<int>>& getAdjList() const { return adjList; }
    const QVector<QPair<int, int>>& getEdges() const { return edgeList; }
    QVector<int> dijkstra(int start, int finish, const QVector<QVector<double>>& weights) const;
};
