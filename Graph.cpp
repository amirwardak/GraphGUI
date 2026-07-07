#include "Graph.h"
#include <fstream>
#include <stdexcept>
#include <QStack>
#include <limits>

bool Graph::loadFromFile(const std::string& path) {
    std::ifstream fin(path);
    if (!fin.is_open())
        return false;

    int nEdges;
    fin >> size >> nEdges;

    adjList.resize(size);
    edgeList.clear();

    for (int i = 0; i < nEdges; ++i) {
        int v1, v2;
        fin >> v1 >> v2;
        --v1; --v2;

        if (v1 < 0 || v2 < 0 || v1 >= size || v2 >= size)
            throw std::logic_error("Vertex index out of bounds");

        adjList[v1].append(v2);
        adjList[v2].append(v1);
        edgeList.append(qMakePair(v1, v2));
    }

    return true;
}

QVector<int> Graph::dijkstra(int start, int finish, const QVector<QVector<double>>& weights) const
{
    const double INF = std::numeric_limits<double>::max();

    QVector<bool> fixed(size, false);
    QVector<double> dist(size, INF);
    QVector<int> prev(size, -1);

    dist[start] = 0;

    for (int i = 0; i < size; ++i) {
        int temp = -1;
        for (int v = 0; v < size; ++v) {
            if (!fixed[v] && (temp == -1 || dist[v] < dist[temp]))
                temp = v;
        }

        if (dist[temp] == INF) break;
        fixed[temp] = true;

        for (int v = 0; v < size; ++v) {
            if (weights[temp][v] != INF && !fixed[v]) {
                double minDist = dist[temp] + weights[temp][v];
                if (minDist < dist[v]) {
                    dist[v] = minDist;
                    prev[v] = temp;
                }
            }
        }
    }

    if (dist[finish] == INF)
        return {};

    QStack<int> stack;
    int current = finish;
    while (current != -1) {
        stack.push(current);
        current = prev[current];
    }

    QVector<int> result;
    while (!stack.isEmpty())
        result.append(stack.pop());

    return result;
}
