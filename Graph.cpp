#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <tuple>  // Include the tuple header
#include <vector>
using namespace std;

class Graph {
   private:
    std::vector<std::tuple<int, int, int, int>> edges;  // (from, to, weight, id)
    int numVertices;

   public:
    Graph(){};  // Default constructor
    Graph(int n) : numVertices(n) {}
//return the next id
    int getSize() {
        return edges.size();
    }
    int getnumVertices() {
        return numVertices;
    }
   
    void setEdges(vector<std::tuple<int, int, int, int>>& e) {
        edges = e;
    }
    void setnumVertices(int n) {
        numVertices = n;
    }
    //return edges
    vector<tuple<int, int, int, int>> getEdges() {
        return edges;
    }
    tuple<int,int,int,int> getEdge(int i) {
        cout << "getEdge" << endl;
        //print all parmeters
        int u, v, w, id;
        tie(u, v, w, id) = edges[i];
        cout << "Edge " << i << ": " << u << " " << v << " " << w << endl;
        return edges[i];
    }
    //get the number of vertices
    int getNumVertices() {
        return numVertices;
    }

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


