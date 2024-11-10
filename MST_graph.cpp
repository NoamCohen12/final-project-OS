#include "MST_graph.hpp"

// Helper function to perform DFS and accumulate the path weight between two vertices
bool dfs(int current, int target, const vector<vector<tuple<int, int, int>>>& adj, vector<bool>& visited, int& path_weight) {
    if (current == target) return true;  // Found the target vertex

    visited[current] = true;

    for (const auto& edge : adj[current]) {
        int to, weight, id;
        tie(to, weight, id) = edge;

        if (!visited[to]) {
            path_weight += weight;  // Accumulate the weight

            // Recursively search for the target vertex
            if (dfs(to, target, adj, visited, path_weight)) {
                return true;  // Return true if target found
            }

            path_weight -= weight;  // Backtrack if the path does not lead to the target
        }
    }
    return false;  // Return false if target vertex not found
}

// Function to generate the matrix of total path weights between every pair of vertices
vector<vector<int>> calculateWeightMatrix(int n, const vector<tuple<int, int, int, int>>& edges) {
    vector<vector<tuple<int, int, int>>> adj(n);             // Using tuple directly for adjacency list
    vector<vector<int>> weightMatrix(n, vector<int>(n, 0));  // Initialize the weight matrix

    // Build the adjacency list for the tree from the edges
    for (const auto& e : edges) {
        int u, v, w, id;
        tie(u, v, w, id) = e;
        adj[u].emplace_back(v, w, id);
        adj[v].emplace_back(u, w, id);
    }

    // Calculate the weight between all pairs of vertices
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            vector<bool> visited(n, false);
            int path_weight = 0;
            if (dfs(i, j, adj, visited, path_weight)) {
                weightMatrix[i][j] = weightMatrix[j][i] = path_weight;  // Symmetric matrix
            }
        }
    }
    return weightMatrix;
}

MST_graph::MST_graph(vector<tuple<int, int, int, int>> edges, string strategy) {
    this->edges = edges;
    numVertices = edges.size() + 1;
    alldistances = calculateWeightMatrix(edges.size() + 1, edges);
    this->strategy = strategy;
}

// Function to convert the MST graph to a string
string MST_graph::toString() const{
    string ans;
    vector<tuple<int, int, int, int>> edges = this->getEdges();

    for (const auto &e : edges) {
        int u, v, w, id;
        tie(u, v, w, id) = e;
        ans += "Edge " + std::to_string(id) + ": " + std::to_string(u) + " " + std::to_string(v) + " " + std::to_string(w) + "\n";
    }

    return ans;
}
