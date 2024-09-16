#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <sstream>  // Include the necessary header for istringstream

#include "Graph.cpp"
#include "MST.hpp"
#include "union_find.hpp"
#include "GraphGUI.cpp"  // Include the GraphGUI header

#define PORT "9034"  // port we're listening on
using namespace std;

vector<tuple<int, int, int, int>> sharedGraph;  // Shared graph for all clients
mutex mtx;

vector<tuple<int, int, int, int>> Newgraph(istringstream& iss, int n, int m) {
    vector<tuple<int, int, int, int>> graph;
    int u, v, w;
    for (int i = 0; i < m; ++i) {
        iss >> u >> v >> w;
        graph.emplace_back(u, v, w, i);
    }
    return graph;
}

string primMST(const vector<tuple<int, int, int, int>>& graph) {
    // Implement Prim's MST algorithm
    return "Prim's MST result\n";
}

string kruskalMST(const vector<tuple<int, int, int, int>>& graph) {
    // Implement Kruskal's MST algorithm
    return "Kruskal's MST result\n";
}

void addEdge(int u, int v, vector<tuple<int, int, int, int>>& graph) {
    // Add edge to the graph
    graph.emplace_back(u, v, 0, graph.size());
}

void removeEdge(int u, int v, vector<tuple<int, int, int, int>>& graph) {
    // Remove edge from the graph
    graph.erase(remove_if(graph.begin(), graph.end(), [u, v](const tuple<int, int, int, int>& edge) {
        return get<0>(edge) == u && get<1>(edge) == v;
    }), graph.end());
}

string graph_user_commands(string input_user) {
    string ans;
    string command_of_user;
    istringstream iss(input_user);
    iss >> command_of_user;

    int n, m;

    if (command_of_user.empty()) {
        ans += "No command received.\n";
    } else if (command_of_user == "Newgraph") {
        iss >> n >> m;
        if (n <= 0 || m < 0) {
            ans += "Invalid graph parameters. \n";
        } else {
            sharedGraph = Newgraph(iss, n, m);
            ans += "Graph created:\n";
            for (int i = 0; i < n; i++) {
                ans += to_string(i + 1) + ": ";
                for (int neighbor : sharedGraph[i]) {
                    ans += to_string(neighbor + 1) + " ";
                }
                ans += "\n";
            }
        }
    } else if (command_of_user == "MST-P") {
        if (!sharedGraph.empty()) {
            ans += primMST(sharedGraph);
        }
    } else if (command_of_user == "MST-K") {
        if (!sharedGraph.empty()) {
            ans += kruskalMST(sharedGraph);
        }
    } else if (command_of_user == "Newedge") {
        int u, v;
        iss >> u >> v;
        if (!sharedGraph.empty()) {
            addEdge(u, v, sharedGraph);
            ans += "Edge added from " + to_string(u) + " to " + to_string(v) + "\n";
        } else {
            ans += "No graph found for adding edge.\n";
        }
    } else if (command_of_user == "Removeedge") {
        int u, v;
        iss >> u >> v;
        if (!sharedGraph.empty()) {
            removeEdge(u, v, sharedGraph);
            ans += "Edge removed from " + to_string(u) + " to " + to_string(v) + "\n";
        } else {
            ans += "No graph found for removing edge.\n";
        }
    } else {
        ans += "Unknown command.\n";
    }
    return ans;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

vector<tuple<int, int, int, int>>
build_random_connected_graph(int num_vertices, int num_edges, unsigned int seed) {
    srand(seed);
    assert(num_edges >= (num_vertices - 1));
    assert(1ll * num_edges <= (1ll * num_vertices * (num_vertices - 1)) / 2);

    vector<int> W(num_edges, 0);
    iota(W.begin(), W.end(), 0);

    random_device rd;
    mt19937 g(seed);
    shuffle(W.begin(), W.end(), g);

    vector<tuple<int, int, int, int>> random_graph;
    int nxt = 0;
    set<pair<int, int>> edges;

    for (int i = 0; i < num_vertices - 1; ++i) {
        random_graph.emplace_back(i, i + 1, W[nxt], nxt);
        ++nxt;
        edges.emplace(i, i + 1);
    }

    int remaining_edges = num_edges - (num_vertices - 1);
    for (int e = 0; e < remaining_edges; ++e) {
        int a, b;
        a = (rand() % num_vertices);
        b = (rand() % num_vertices);
        if (a > b) swap(a, b);
        while (b == a || edges.find(make_pair(a, b)) != edges.end()) {
            a = (rand() % num_vertices);
            b = (rand() % num_vertices);
            if (a > b) swap(a, b);
        }
        random_graph.emplace_back(a, b, W[nxt], nxt);
        ++nxt;
        edges.emplace(a, b);
    }
    assert(static_cast<int>(random_graph.size()) == num_edges);
    return random_graph;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <num_vertices> <num_edges> <seed>" << std::endl;
        return 1;
    }

    int num_vertices = std::stoi(argv[1]);
    int num_edges = std::stoi(argv[2]);
    unsigned int seed = std::stoul(argv[3]);

    MST mst;
    Graph graph(num_vertices);
    vector<tuple<int, int, int, int>> mst_result = mst.prim(graph);
    mst.printMST(mst_result);

    // Define vertices (for example, positions of points)
    std::vector<sf::Vector2f> vertices;
    for (int i = 0; i < num_vertices; ++i) {
        vertices.emplace_back(100.f + (i * 200.f), 100.f + ((i % 2) * 200.f));
    }

    // Create the Graph GUI and run it
    GraphGUI graphGUI(vertices, graph, mst_result);
    graphGUI.run();

    return 0;
}