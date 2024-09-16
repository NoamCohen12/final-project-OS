
#include "Graph.cpp"
#include <algorithm>
#include <iostream>
#include <tuple>
#include <utility>
#include "union_find.hpp"
using namespace std;



class MST {
    Graph g1;

   public:
   vector<tuple<int, int, int, int>> _prim(const vector<vector<Edge>>& adj, int n);
   vector<tuple<int, int, int, int>> prim(const vector<tuple<int, int, int, int>>& edges, int n);
   vector<tuple<int, int, int, int>> kruskal(const vector<tuple<int, int, int, int>>& graph_edges, int n);
}
