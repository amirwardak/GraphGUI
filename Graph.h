#pragma once
#include <vector>
#include <string>
#include <utility>

class Graph
{
private:
    int size{};
    std::vector<std::vector<int>> adjList;
    std::vector<std::pair<int, int>> edgeList;
public:
    Graph() = default;
    bool loadFromFile(const std::string& path);
    int getSize() const { return size; }
    const std::vector<std::vector<int>>& getAdjList() const { return adjList; }
    const std::vector<std::pair<int, int>>& getEdges() const { return edgeList; }
};
