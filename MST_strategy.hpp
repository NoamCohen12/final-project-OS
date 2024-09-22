
#ifndef MST_STRATEGY_HPP
#define MST_STRATEGY_HPP
#include <algorithm>
#include <iostream>
#include <tuple>
#include <utility>
#include "union_find.hpp"
#include <sstream>  // Include the necessary header for ostringstream

#include "MST_graph.hpp"
#include <chrono>
#include <iostream>
#include <set>
#include <tuple>
#include <utility>
#include <vector>
using namespace std;



class MST_strategy {
   public:
   MST_graph prim(const vector<tuple<int, int, int, int>>& edges, int n);
   MST_graph kruskal(const vector<tuple<int, int, int, int>>& graph_edges, int n);
};

#endif // MST_STRATEGY_HPP