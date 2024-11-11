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
            if (alldistances[i][j] < shortestDistance && alldistances[i][j]!=0) {
                shortestDistance = alldistances[i][j];
            }
        }
    }
    return shortestDistance;
}

//over just on above the diagonal

// Function to get the average distance in the MST graph
double MST_stats::getAverageDistance(const MST_graph& mst) {
    vector<vector<int>> alldistances = mst.getAllDistances();
    int sum = 0;
    int count = 0;

    // cout << "Calculating average distance" << endl;
    // cout << "Number of vertices: " << alldistances.size() << endl;

    for (int i = 0; i < alldistances.size(); i++) {
        for (int j = i + 1; j < alldistances[i].size(); j++) {
            sum += alldistances[i][j];
            count++;
      //      cout << "Adding distance between vertex " << i << " and vertex " << j << ": " << alldistances[i][j] << endl;
        }
    }

    double averageDistance = (double)sum / count;
  //  cout << "Total sum of distances: " << sum << endl;
    // cout << "Total number of distances: " << count << endl;
    // cout << "Average distance: " << averageDistance << endl;

    return averageDistance;
}



