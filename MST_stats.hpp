#ifndef MST_STATS_HPP
#define MST_STATS_HPP
#include "MST_graph.hpp"
#include <climits> // Include this header for INT_MIN
#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>
#include <numeric>

using namespace std;

class MST_stats {
   public:
    int getTotalWeight(const MST_graph& mst);
    int getLongestDistance(const MST_graph& mst);
    double getAverageDistance(const MST_graph& mst);
    int getShortestDistance(const MST_graph& mst);
};
#endif // MST_STATS_HPP