#include "MST_stats.hpp"

    int MST_stats::getTotalWeight(const MST_graph& mst){
        vector<tuple<int, int, int, int>> edges = mst.getEdges();
        int totalWeight = 0;
        for (const auto& e : edges) {
            int u, v, w, id;
            tie(u, v, w, id) = e;
            totalWeight += w;
        }
        return totalWeight;
    }

int MST_stats::getLongestDistance(const MST_graph& mst) {
    vector<vector<int>> alldistances = mst.getAllDistances();
    int longestDistance = INT_MIN;
    for (int i = 0; i < alldistances.size(); i++) {
        for (int j = 0; j < alldistances[i].size(); j++) {
            if (alldistances[i][j] > longestDistance) {
                longestDistance = alldistances[i][j];
            }
        }
    }
    return longestDistance;
}

int MST_stats::getShortestDistance(const MST_graph& mst) {
    vector<vector<int>> alldistances = mst.getAllDistances();
    int shortestDistance = INT_MAX;
    for (int i = 0; i < alldistances.size(); i++) {
        for (int j = 0; j < alldistances[i].size(); j++) {
            if (alldistances[i][j] < shortestDistance) {
                shortestDistance = alldistances[i][j];
            }
        }
    }
    return shortestDistance;
}

double MST_stats::getAverageDistance(const MST_graph& mst) {
    vector<vector<int>> alldistances = mst.getAllDistances();
    int totalDistance = 0;
    int numDistances = 0;
    for (int i = 0; i < alldistances.size(); i++) {
        for (int j = 0; j < alldistances[i].size(); j++) {
            totalDistance += alldistances[i][j];
            numDistances++;
        }
    }
    return static_cast<double>(totalDistance) / numDistances;
}