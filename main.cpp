#include <arpa/inet.h>   // Include for inet_ntop
#include <netdb.h>       // Include for addrinfo
#include <netinet/in.h>  // Include for sockaddr_in and sockaddr_in6
#include <sys/socket.h>  // Include for sockaddr
#include <unistd.h>      // Include for close

#include <algorithm>
#include <cassert>
#include <csignal>  // For signal handling (e.g., SIGINT)
#include <cstring>  // Include for memset
#include <ctime>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <set>
#include <sstream>  // Include the necessary header for istringstream
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "Graph.cpp"
#include "GraphGUI.cpp"  // Include the GraphGUI header
#include "LeaderFollowerPool.hpp"
#include "MST_graph.hpp"
#include "MST_stats.hpp"
#include "MST_strategy.hpp"
#include "Pipeline.cpp"
#include "union_find.hpp"
#define PORT "9034"    // port we're listening on
#define NUM_THREADS 4  // Number of threads in the thread pool
using namespace std;
unordered_map<int, tuple<Graph, MST_graph, string>> map_clients;  // Each client gets its own graph
std::mutex lfMutex;                                               // Declare as static to ensure it's shared across function calls
LeaderFollowerPool threadPool(NUM_THREADS, lfMutex);              // Create a thread pool object
Pipeline pipeline;

MST_strategy mst;
atomic<bool> isMST{false};

// Declare the LeaderFollowerPool instance here
int listener;  // Global listener for shutdown handling

// Signal handler for graceful shutdown
void shutdown_handler(int signum) {
    // cout << "\nShutting down the server..." << endl;
    close(listener);  // Close the listener socket
    exit(0);          // Exit the program
}

// assume the input is in the form of "Newgraph n m u v w"
// and the input of edge u v with weight w but dont add the reverse edge with weight w'
vector<tuple<int, int, int, int>> Newgraph(istringstream &iss, int n, int num_of_Edge) {
    vector<tuple<int, int, int, int>> graph;
    // cout << "Newgraph" << endl;
    int u, v, w;  // vertex and weight
    for (int i = 0; i < num_of_Edge; ++i) {
        iss >> u >> v >> w;
        graph.emplace_back(u, v, w, i);
        graph.emplace_back(v, u, w, -2);  // Add the reverse edge for undirected graph
    }
    return graph;
}

string graph_user_commands(string input_user, Graph &clientGraph, MST_graph &clientMST, string &clientAns, int fdclient) {
    // Shared string to accumulate results
    string ans;
    string command_of_user;
    istringstream iss(input_user);
    iss >> command_of_user;

    int n, m;
    std::lock_guard<std::mutex> lock(lfMutex);

    if (command_of_user.empty()) {
        {
            // std::lock_guard<std::mutex> lock(lfMutex); // Lock before modifying ans
            ans += "No command received.\n";
        }
    } else if (command_of_user == "Newgraph") {
        // Get the number of vertices and edges
        iss >> n >> m;
        if (n <= 0 || m < 0) {
            {
                ans += "Invalid graph parameters. \n";
            }
        } else {
            auto newEdges = Newgraph(iss, n, m);  // Store the result in a local variable
            clientGraph.setEdges(newEdges);       // Pass the local variable to setEdges
            clientGraph.setnumVertices(n);

            {
                // std::lock_guard<std::mutex> lock(lfMutex); // Lock before modifying ans
                ans += "Graph created:\n";
                ans += clientGraph.toString();
            }
        }

    } else if (command_of_user == "prim") {
        clientMST = mst.prim(clientGraph.getEdges(), clientGraph.getnumVertices());
        clientMST.setStrategy("prim");
        isMST = true;
        ans += "MST created using Prim's Algorithm.\n";
        ans += clientMST.toString();

    } else if (command_of_user == "kruskal") {
        clientMST = mst.kruskal(clientGraph.getEdges(), clientGraph.getnumVertices());
        clientMST.setStrategy("kruskal");
        isMST = true;
        ans += "MST created using Kruskal's Algorithm.\n";
        ans += clientMST.toString();

    } else if (command_of_user == "Newedge") {
        int from, to, weight;
        iss >> from >> to >> weight;
        // cout << "Newedge n: " << n << endl;
        if (clientGraph.getSize() != 0) {
            clientGraph.addEdge(from, to, weight, clientGraph.getSize());

            ans += "Edge added from " + to_string(from) + " to " + to_string(to) + "\n";
            ans+=clientGraph.toString();
        } else {
            {
                ans += "No graph found for adding edge.\n";
            }
        }
    } else if (command_of_user == "Removeedge") {
        int from, to;
        iss >> from >> to;
        // cout << "Removeedge n: " << n << endl;
        if (clientGraph.getSize() != 0) {
            clientGraph.removeEdge(from, to);

            ans += "Edge removed from " + to_string(from) + " to " + to_string(to) + "\n";
            ans+=clientGraph.toString();

        } else {
            {
                ans += "No graph found for removing edge.\n";
            }
        }
    } else if (command_of_user == "Reduceedge") {
        int id, newWeight;
        iss >> id >> newWeight;
        // cout << "Reduceedge n: " << n << endl;
        if (clientGraph.getSize() != 0) {
            clientGraph.reduceEdges(id, newWeight);

            ans += "Edge reduced with id " + to_string(id) + " to weight " + to_string(newWeight) + "\n";
            ans+=clientGraph.toString();

        } else {
            {
                ans += "No graph found for reducing edge.\n";
            }
        }
    } else if (command_of_user == "Leader_Follower") {
        if (!isMST) {
            {
                ans += "No MST found.\n";
            }
        } else {
            // auto clientTask = std::make_tuple(&clientMST, &clientAns,fdclient);
            auto *clientTask = new std::tuple<MST_graph *, std::string *, int>(&clientMST, &clientAns, fdclient);

            // Pass the pointer to the tuple to the threadPool
            threadPool.addEventHandler(clientTask);
            // Add a small delay to allow Leader_Follower to process
            {
                ans += "done";
            }
        }
    } else if (command_of_user == "Pipeline") {
        if (!isMST) {
            {
                ans += "No MST found.\n";
            }
        } else {
            auto *clientTask = new std::tuple<MST_graph *, std::string *, int>(&clientMST, &clientAns, fdclient);
            // Pass the pointer to the tuple to the pipeline
            pipeline.addRequest(clientTask);

            {
                ans += "done";
            }
        }
    } else {
        {
            ans += "Unknown command.\n";
        }
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

// vector<tuple<int, int, int, int>> build_random_connected_graph(int n, int m, unsigned int seed) {
//     srand(seed);
//     assert(m >= (n - 1));
//     assert(1ll * m <= (1ll * n * (n - 1)) / 2);

//     vector<int> W(m, 0);
//     iota(W.begin(), W.end(), 0);

//     random_device rd;
//     mt19937 g(seed);
//     shuffle(W.begin(), W.end(), g);

//     vector<tuple<int, int, int, int>> random_graph;
//     int nxt = 0;
//     set<pair<int, int>> edges;

//     for (int i = 0; i < n - 1; ++i) {
//         random_graph.emplace_back(i, i + 1, W[nxt], nxt);
//         ++nxt;
//         edges.emplace(i, i + 1);
//     }

//     int remaining_edges = m - (n - 1);
//     for (int e = 0; e < remaining_edges; ++e) {
//         int a, b;
//         a = (rand() % n);
//         b = (rand() % n);
//         if (a > b) swap(a, b);
//         while (b == a || edges.find(make_pair(a, b)) != edges.end()) {
//             a = (rand() % n);
//             b = (rand() % n);
//             if (a > b) swap(a, b);
//         }
//         random_graph.emplace_back(a, b, W[nxt], nxt);
//         ++nxt;
//         edges.emplace(a, b);
//     }
//     assert(static_cast<int>(random_graph.size()) == m);
//     return random_graph;
// }
int main() {
    mutex mtx;

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    // Initialize the LeaderFollowerPool once
    threadPool.start();   // Start the LeaderFollowerPool once during server initialization
    pipeline.start(mtx);  // Start the pipeline once during server initialization

    int newfd;                              // newly accept()ed socket descriptor
    struct sockaddr_storage clientAddress;  // client address
    socklen_t addrlen;

    char buf[4096];  // buffer for client data (increased size)
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];
    int yes = 1;  // for setsockopt() SO_REUSEADDR, below
    int i, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);  // clear the master and temp sets
    FD_ZERO(&read_fds);

    // Set up shutdown signal handler
    signal(SIGINT, shutdown_handler);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        cerr << "selectserver: " << gai_strerror(rv) << endl;
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }

    if (p == NULL) {
        cerr << "selectserver: failed to bind" << endl;
        exit(2);
    }

    freeaddrinfo(ai);  // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener;  // so far, it's this one

    // cout << "Server is listening on port " << PORT << endl;

    // main loop
    for (;;) {
        read_fds = master;  // copy it
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {  // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof clientAddress;
                    newfd = accept(listener, (struct sockaddr *)&clientAddress, &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master);  // add to master set
                        if (newfd > fdmax) {     // keep track of the max
                            fdmax = newfd;
                        }
                        // cout << "selectserver: new connection from "
                        //      << inet_ntop(clientAddress.ss_family,
                        //                   get_in_addr((struct sockaddr *)&clientAddress),
                        //                   remoteIP, INET6_ADDRSTRLEN)
                        //      << " on socket " << newfd << endl;

                        // Create a new graph for this client
                        lock_guard<mutex> lock(mtx);
                        std::get<0>(map_clients[newfd]) = Graph();  // Create an empty graph for the new client
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            // cout << "selectserver: socket " << i << " hung up" << endl;
                        } else {
                            perror("recv");
                        }
                        close(i);            // bye!
                        FD_CLR(i, &master);  // remove from master set
                        lock_guard<mutex> lock(mtx);
                        map_clients.erase(i);  // Remove the client's graph
                    } else {
                        // we got some data from a client
                        buf[nbytes] = '\0';  // null-terminate the buffer

                        // Process the command for this client

                        lock_guard<mutex> lock(mtx);
                        string response = graph_user_commands(string(buf), get<0>(map_clients[i]), get<1>(map_clients[i]), get<2>(map_clients[i]), i);

                        // Send the response back to the client
                        if (send(i, response.c_str(), response.length(), 0) == -1) {
                            perror("send");
                        }
                    }
                }
            }
        }
    }
    return 0;
}