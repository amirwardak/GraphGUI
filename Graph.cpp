#include "Graph.h"
#include <fstream>
#include <stdexcept>

bool Graph::loadFromFile(const std::string& path) {
    std::ifstream fin(path);
    if (!fin.is_open())
        return false;

    int nEdges;
    fin >> size >> nEdges;

    adjList.assign(size, std::vector<int>());
    edgeList.clear();

    for (int i = 0; i < nEdges; ++i) {
        int v1, v2;
        fin >> v1 >> v2;
        --v1; --v2;

        if (v1 < 0 || v2 < 0 || v1 >= size || v2 >= size)
            throw std::logic_error("Vertex index out of bounds");

        adjList[v1].push_back(v2);
        adjList[v2].push_back(v1);
        edgeList.emplace_back(v1, v2);
    }

    return true;
}
