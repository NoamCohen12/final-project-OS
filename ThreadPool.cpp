#include "ThreadPool.hpp"

#include <chrono>
#include <iostream>

// Update constructor to accept shared answer stream and mutex
ThreadPool::ThreadPool(int numThreads)
    : numThreads_(numThreads), stopFlag_(false) {
    for (int i = 0; i < numThreads_; ++i) {
        workers_.emplace_back(&ThreadPool::leaderRole, this);
        threadQueue_.push(workers_.back().get_id());
    }
}

ThreadPool::~ThreadPool() {
    stop();
    // for (auto& worker : workers_) {
    //     if (worker.joinable()) {
    //         worker.join();
    //     }
    // }
}



void ThreadPool::addEventHandler(function<void()> task) {
    std::lock_guard<std::mutex> lock(mutexqueue);
    eventQueue_.push(task);
    cv_.notify_one();  // Notify the leader thread to handle the event
}

void ThreadPool::leaderRole() {
    std::cout << "Entering leader role" << std::endl;

    while (true) {
        function<void()> currentRequest;  // Function to process
        std::unique_lock<std::mutex> lock(mutexqueue);
        std::cout << "Thread " << std::this_thread::get_id() << " waiting for event or stop signal." << std::endl;

        cv_.wait(lock, [this] { 
         lock_guard<mutex> stopLock(mutexstop); 
            return !eventQueue_.empty() || stopFlag_; });

        lock_guard<mutex> stopLock(mutexstop);
        if (stopFlag_ && eventQueue_.empty()) {
            std::cout << "Thread " << std::this_thread::get_id() << " stopping as stopFlag is set and eventQueue is empty." << std::endl;
            return;
        }

        // Process the next event in the queue
        if (!eventQueue_.empty()) {
            currentRequest = eventQueue_.front();
            eventQueue_.pop();
            lock.unlock();  // Unlock queue while processing

            // MST_graph& currentMST = currentRequest.clientMST;
            // std::string& currentAns = currentRequest.clientAns;

            std::thread::id currentThreadId = std::this_thread::get_id();
            std::cout << "Thread " << currentThreadId << " processing event." << std::endl;

            // Manage the thread queue
            if (!threadQueue_.empty()) {
                threadQueue_.pop();
                threadQueue_.push(currentThreadId);
            }

            //   lock.unlock();  // Unlock before processing

            // MST_stats mst_stats;
            // std::ostringstream localAns;
            // localAns << "Thread " << std::this_thread::get_id() << "\n";
            // localAns << " Longest path: " << mst_stats.getLongestDistance(currentMST) << "\n";
            // localAns << " Shortest path: " << mst_stats.getShortestDistance(currentMST) << "\n";
            // localAns << " Average path: " << mst_stats.getAverageDistance(currentMST) << "\n";
            // localAns << " Total weight: " << mst_stats.getTotalWeight(currentMST) << "\n";
            currentRequest();
            // Add a sleep
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // Append the local answer to the shared string in a thread-safe way
            {
                //   std::lock_guard<std::mutex> ansLock(mutexqueue);
                // std::lock_guard<std::mutex> ansLock(mutexqueue);

               // currentAns += localAns.str();
                //  std::cout << "Thread " << currentThreadId << " appended result to sharedAns: " << currentAns << std::endl;
            }

            followerRole();
            std::cout << "Thread " << currentThreadId << " finished processing event and notified followers." << std::endl;
        }
    }
}
void ThreadPool::followerRole() {
    // Notify another thread to take over as the leader
    cv_.notify_one();
}

// Method to gracefully stop the thread pool
void ThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lock(mutexqueue);

        stopFlag_ = true;
    }

    cv_.notify_all();  // Wake up all threads
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();  // Wait for all worker threads to finish
        }
    }
}
