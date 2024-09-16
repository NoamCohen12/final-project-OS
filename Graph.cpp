
class Graph {
private:
    std::vector<std::tuple<int, int, int, int>> edges;  // (from, to, weight, id)
    int numVertices;

public:
    Graph(int n) : numVertices(n) {}

    void addEdge(int from, int to, int weight, int id) {
        edges.emplace_back(from, to, weight, id);
    }
}