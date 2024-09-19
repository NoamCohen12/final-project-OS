#include "Graph.cpp"



using namespace std;

class MST_stats {
   public:
    int getTotalWeight(const Graph& g);
    int getLongestDistance(const vector<Edge>& mst, const Graph& g);
    double getAverageDistance(const Graph& g);
    int getShortestDistance(const vector<Edge>& mst, const Graph& g);
}