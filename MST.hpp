
#include <algorithm>
#include <iostream>
#include <tuple>
#include <utility>
#include "union_find.hpp"
#include <sstream>  // Include the necessary header for ostringstream

#include <chrono>
#include <iostream>
#include <set>
#include <tuple>
#include <utility>
#include <vector>
using namespace std;



class MST {
   public:
   vector<tuple<int, int, int, int>> prim(const vector<tuple<int, int, int, int>>& edges, int n);
   vector<tuple<int, int, int, int>> kruskal(const vector<tuple<int, int, int, int>>& graph_edges, int n);
};
