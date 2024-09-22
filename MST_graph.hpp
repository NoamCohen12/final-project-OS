#ifndef MST_GRAPH_HPP
#define MST_GRAPH_HPP
#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <set>
#include <queue>
#include <utility>
#include <vector>
using namespace std;
 


 class MST_graph
 {
    vector<tuple<int, int, int, int>> edges;  // (from, to, weight, id)
    int numVertices;
    vector<vector<int>> alldistances;//all distances between vertices
   
    public:
    MST_graph(){};  // Default constructor
    MST_graph(int n) : numVertices(n) {}
    MST_graph(vector<tuple<int, int, int, int>> edges); 
    vector<tuple<int, int, int, int>> getEdges() const {
        return edges;
    }
    vector<vector<int>> getAllDistances() const {
        return alldistances;
    }

    

 };
 #endif // MST_GRAPH_HPP