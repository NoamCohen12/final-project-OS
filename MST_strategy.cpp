
#include "MST_strategy.hpp"

constexpr int INF = 0x3f3f3f3f;

struct Edge {
    int w = INF, to = -1, id;
    bool operator<(Edge const& other) const {
        return make_pair(w, to) < make_pair(other.w, other.to);
    }
    Edge() {
        w = INF;
        to = -1;
    }
    Edge(int _w, int _to, int _id) : w(_w), to(_to), id(_id) {}
};

vector<tuple<int, int, int, int>> MST_strategy::kruskal(const vector<tuple<int, int, int, int>>& graph_edges, int n) {
    UnionFind graph(n);
    vector<tuple<int, int, int, int>> edges;
    vector<tuple<int, int, int, int>> spanning_tree;

    edges = graph_edges;

    sort(edges.begin(), edges.end(), [&](const tuple<int, int, int, int>& a, const tuple<int, int, int, int>& b) {
        return get<2>(a) < get<2>(b);
    });

    for (const auto& edge : edges) {
        int from, to, cost, id;
        tie(from, to, cost, id) = edge;
        if (graph.unite(from, to)) {
            spanning_tree.emplace_back(from, to, cost, id);
        }
    }
    cout << "Kruskal's Algorithm\n";
    return spanning_tree;
}

vector<tuple<int, int, int, int>> _prim(const vector<vector<Edge>>& adj, int n) {
    vector<tuple<int, int, int, int>> spanning_tree;

    vector<Edge> min_e(n);
    min_e[0].w = 0;
    set<Edge> q;
    q.insert({0, 0, -1});

    vector<bool> selected(n, false);
    for (int i = 0; i < n; ++i) {
        int v = q.begin()->to;
        selected[v] = true;
        q.erase(q.begin());

        if (min_e[v].to != -1) {
            spanning_tree.emplace_back(min_e[v].to, v, min_e[v].w, min_e[v].id);
        }

        for (Edge e : adj[v]) {
            if (!selected[e.to] && e.w < min_e[e.to].w) {
                q.erase({min_e[e.to].w, e.to, e.id});
                min_e[e.to] = {e.w, v, e.id};
                q.insert({e.w, e.to, e.id});
            }
        }
    }
    return spanning_tree;
}

vector<tuple<int, int, int, int>> MST_strategy::prim(const vector<tuple<int, int, int, int>>& edges, int n) {
    // Create an adjacency list with 'n' nodes
    vector<vector<Edge>> adj(n);

    for (const auto& e : edges) {
        // Unpack the tuple into nodes (u, v), weight, and edge ID
        int u, v, weight, id;
        tie(u, v, weight, id) = e;  // Correct unpacking

        // Add the edges to the adjacency list
        adj[u].push_back(Edge(weight, v, id));  // Edge from u to v
        adj[v].push_back(Edge(weight, u, id));  // Edge from v to u (undirected graph)
    }
    // Now call your _prim function, assuming it takes adj and n
    vector<tuple<int, int, int, int>> spanning_tree = _prim(adj, n);

    cout << "Prim's Algorithm\n";

    // Return the MST result as a string
    return spanning_tree;
}

