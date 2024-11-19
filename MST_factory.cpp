
#include "MST_factory.hpp"

MST_graph* MST_factory::createMST(const vector<tuple<int, int, int, int>>& edges, int n, string strategy) {
    MST_strategy mst;
    if (strategy == "prim") {
        return new MST_graph(mst.prim(edges, n).getEdges(), "prim");
    } else if (strategy == "kruskal") {
        return new MST_graph(mst.kruskal(edges, n).getEdges(), "kruskal");
    } else {
        return nullptr;
    }
}
