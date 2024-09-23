#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <functional>
#include <vector>
#include <sstream>
#include <iostream>
#include "MST_graph.hpp"
#include "MST_stats.hpp"


#define LONGEST_PATH 1
#define SHORTEST_PATH 2
#define AVERAGE_PATH 3
#define TOTAL_WEIGHT 4

class ThreadPool {
public:


ThreadPool(int numThreads, const MST_graph& mst, std::ostringstream& sharedAns, std::mutex& mtxAns);

    ThreadPool(int numThreads, const MST_graph& mst);
    ~ThreadPool();

    // Add an event to the queue
    void addEventHandler(int eventType);

    // Stop the thread pool
    void stop();

private:
    void leaderRole();  // Function for the leader thread
    void followerRole();  // Function for the followers

    int numThreads_;  // Total number of threads in the pool
    std::queue<std::thread::id> threadQueue_;  // Queue of thread IDs
    std::queue<int> eventQueue_;  // Queue of events to process
    std::vector<std::thread> workers_;  // List of thread objects
    std::mutex mtx_;  // Mutex for synchronization
    std::condition_variable cv_;  // Condition variable for waiting threads
    bool stopFlag_;  // Flag to stop the thread pool
    MST_graph mst_;  // MST graph object
    std::ostringstream& sharedAns_;  // Shared answer buffer
    std::mutex& mtxAns_;  // Mutex for the shared answer


};

#endif // THREAD_POOL_HPP