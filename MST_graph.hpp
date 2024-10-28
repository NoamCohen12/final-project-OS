#ifndef MST_GRAPH_HPP
#define MST_GRAPH_HPP
#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <tuple>
#include <utility>
#include <vector>
using namespace std;

class MST_graph {
    vector<tuple<int, int, int, int>> edges;  // (from, to, weight, id)
    int numVertices;
    vector<vector<int>> alldistances;  // all distances between vertices
    string strategy;

   public:
    MST_graph() {};  // Default constructor
    MST_graph(int n) : numVertices(n) {}
    MST_graph(vector<tuple<int, int, int, int>> edges,string strategy);
    vector<tuple<int, int, int, int>> getEdges() const {
        return edges;
    }
    vector<vector<int>> getAllDistances() const {
        return alldistances;
    }
    string to_string() const ;
    string getStrategy() const {
        return strategy;
    }
    };
#endif  // MST_GRAPH_HPP