#include "MST_stats.hpp"


int MST_stats::getTotalWeight(const Graph& g) {
    int totalWeight = 0;
    for (const auto& edge : g.getEdges()) {
        totalWeight += std::get<2>(edge);
    }
    return totalWeight/2;
}
