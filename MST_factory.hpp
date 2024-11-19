#include "MST_graph.hpp"
#include "MST_strategy.hpp"


class MST_factory {
    public:
    MST_factory() {}
    MST_graph* createMST(const vector<tuple<int, int, int, int>>& edges, int n, string strategy);
       
};