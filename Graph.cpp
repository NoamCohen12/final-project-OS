#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <tuple>  // Include the tuple header
#include <vector>
using namespace std;

class Graph {
   private:
    std::vector<std::tuple<int, int, int, int>> edges;  // (from, to, weight, id)
    int numVertices;

   public:
    Graph() {};  // Default constructor
    Graph(int n) : numVertices(n) {}
    // return the next id
    int getSize() {
        return edges.size();
    }
    int getnumVertices() {
        return numVertices;
    }

    void setEdges(vector<std::tuple<int, int, int, int>>& e) {
        edges = e;
    }
    void setnumVertices(int n) {
        numVertices = n;
    }
    // return edges
    vector<tuple<int, int, int, int>> getEdges() {
        return edges;
    }
    tuple<int, int, int, int> getEdge(int i) {
        // cout << "getEdge" << endl;
        // print all parmeters
        int u, v, w, id;
        tie(u, v, w, id) = edges[i];
        // cout << "Edge " << i << ": " << u << " " << v << " " << w << endl;
        return edges[i];
    }
    // get the number of vertices
    int getNumVertices() {
        return numVertices;
    }

    void addEdge(int from, int to, int weight, int id) {
        edges.emplace_back(from, to, weight, id - 2);
        edges.emplace_back(to, from, weight, -2);
    }

    // remove the edgefrom to and to from
    // erase the edge from to
    void removeEdge(int from, int to) {
        for (int i = 0; i < edges.size(); i++) {
            int u, v, w, id;
            tie(u, v, w, id) = edges[i];
            if (u == from && v == to) {
                edges.erase(edges.begin() + i);
                break;
            }
        }
        for (int i = 0; i < edges.size(); i++) {
            int u, v, w, id;
            tie(u, v, w, id) = edges[i];
            if (u == to && v == from) {
                edges.erase(edges.begin() + i);
                break;
            }
        }

        
    }

    void reduceEdges(int from,int to, int newWhight) {
        //change weight from to and to from 
        for (int i = 0; i < edges.size(); i++) {
            int u, v, w, id;
            tie(u, v, w, id) = edges[i];
            if (u == from && v == to || u == to && v == from) {
                edges[i] = make_tuple(u, v, newWhight, id);
                edges[i] = make_tuple(v, u, newWhight, id);
            }
        }



         
            
      
        for (size_t i = 0; i < edges.size(); i++) {
            int u, v, w, id;
            tie(u, v, w, id) = edges[i];
            cout << " edge : " << u << " v:  " << v << " w: " << w << " id:  " << id << endl;
        }
    }
    // print edge just one side to - from not from - to
    string toString() {
        string ans = "";
        cout << "toString called" << endl;
        for (int i = 0; i < edges.size(); i++) {
            int u, v, w, id;
            tie(u, v, w, id) = this->getEdge(i);
            cout << "Processing edge " << i << ": " << u << " " << v << " " << w << " " << id << endl;
            if (id == -2) {
                cout << "Skipping reverse edge " << i << endl;
                continue;
            }
            ans += "Edge " + to_string(id) + ": " + to_string(u) + " " + to_string(v) + " " + to_string(w) + "\n";
            cout << "Edge added to string: " << "Edge " + to_string(id) + ": " + to_string(u) + " " + to_string(v) + " " + to_string(w) << endl;
        }
        cout << "toString completed" << endl;
        return ans;
    }
};
#endif  // GRAPH_HPP
