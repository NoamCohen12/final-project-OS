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
#include <string>

#include "MST_graph.hpp"
#include "MST_stats.hpp"



class LeaderFollowerPool {
public:


LeaderFollowerPool(int numThreads);

    //LeaderFollowerPool(int numThreads, const MST_graph& mst);
    ~LeaderFollowerPool();

    // Add an event to the queue
    void addEventHandler(std::function<void()> task);

    // Stop the thread pool
    void stop();
    void start();


private:
    void leaderRole();  // Function for the leader thread
    void followerRole();  // Function for the followers


    int numThreads_;  // Total number of threads in the pool
    std::queue<function<void()>>  eventQueue_;  // Queue of events to process
    std::vector<std::thread> workers_;  // List of thread objects
    std::mutex mutexstop;  // Mutex for synchronization
    std::mutex mutexqueue;  // Mutex for synchronization
    std::condition_variable cv_;  // Condition variable for waiting threads
    bool stopFlag_;  // Flag to stop the thread pool


};

#endif // THREAD_POOL_HPP