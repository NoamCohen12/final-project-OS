#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <tuple>  // Include the tuple header
#include <vector>

class Graph {
   private:
    std::vector<std::tuple<int, int, int, int>> edges;  // (from, to, weight, id)
    int numVertices;

   public:
    Graph(int n) : numVertices(n) {}

    void addEdge(int from, int to, int weight, int id) {
        edges.emplace_back(from, to, weight, id);
    }
    void removeEdge(int from, int to) {
        edges.erase(std::remove_if(edges.begin(), edges.end(), [from, to](const std::tuple<int, int, int, int>& edge) {
            return std::get<0>(edge) == from && std::get<1>(edge) == to;
        }), edges.end());
    }
    void reduceEdges(int id ,int newWhight) {
        for (auto& edge : edges) {
            if (std::get<3>(edge) == id) {
                std::get<2>(edge) = newWhight;
            }
        }
    }

};