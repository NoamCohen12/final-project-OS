#include "ThreadPool.hpp"

#include <chrono>
#include <iostream>

// ThreadPool::ThreadPool(int numThreads) : numThreads_(numThreads), stopFlag_(false) {
//     for (int i = 0; i < numThreads_; ++i) {
//         // Create threads and add their IDs to the queue
//         workers_.emplace_back(&ThreadPool::leaderRole, this);
//         threadQueue_.push(workers_.back().get_id());
//     }
// }

// Update constructor to accept shared answer stream and mutex
ThreadPool::ThreadPool(int numThreads, const MST_graph& mst, std::ostringstream& sharedAns, std::mutex& mtxAns)
    : numThreads_(numThreads), stopFlag_(false), sharedAns_(sharedAns), mtxAns_(mtxAns) {
    mst_ = mst;
    for (int i = 0; i < numThreads_; ++i) {
        workers_.emplace_back(&ThreadPool::leaderRole, this);
        threadQueue_.push(workers_.back().get_id());
    }
}

ThreadPool::~ThreadPool() {
    stop();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::addEventHandler(int eventType) {
    std::lock_guard<std::mutex> lock(mtx_);
    eventQueue_.push(eventType);
    cv_.notify_one();  // Notify the leader thread to handle the event
}

void ThreadPool::leaderRole() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx_);
        std::cout << "Thread " << std::this_thread::get_id() << " waiting for event or stop signal." << std::endl;

        cv_.wait(lock, [this] { return !eventQueue_.empty() || stopFlag_; });

        if (stopFlag_ && eventQueue_.empty()) {
            std::cout << "Thread " << std::this_thread::get_id() << " stopping as stopFlag is set and eventQueue is empty." << std::endl;
            return;
        }

        // Process the next event in the queue
        if (!eventQueue_.empty()) {
            int eventType = eventQueue_.front();
            eventQueue_.pop();

            std::thread::id currentThreadId = std::this_thread::get_id();
            std::cout << "Thread " << currentThreadId << " processing event type: " << eventType << std::endl;

            // Manage the thread queue
            if (!threadQueue_.empty()) {
                threadQueue_.pop();
                threadQueue_.push(currentThreadId);
            }

            lock.unlock();  // Unlock before processing

            MST_stats mst_stats;
            std::ostringstream localAns;
            switch (eventType) {
                case LONGEST_PATH:
                    localAns << "Thread " << currentThreadId << " Longest path: " << mst_stats.getLongestDistance(mst_) << "\n";
                    break;
                case SHORTEST_PATH:
                    localAns << "Thread " << currentThreadId << " Shortest path: " << mst_stats.getShortestDistance(mst_) << "\n";
                    break;
                case AVERAGE_PATH:
                    localAns << "Thread " << currentThreadId << " Average path: " << mst_stats.getAverageDistance(mst_) << "\n";
                    break;
                case TOTAL_WEIGHT:
                    localAns << "Thread " << currentThreadId << " Total weight: " << mst_stats.getTotalWeight(mst_) << "\n";
                    break;
                default:
                    localAns << "Thread " << currentThreadId << " Unknown event type\n";
            }
            //add a sleep
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // Append the local answer to the shared string in a thread-safe way
            {
                std::lock_guard<std::mutex> ansLock(mtxAns_);
                sharedAns_ << localAns.str();
                std::cout << "Thread " << currentThreadId << " appended result to sharedAns." << std::endl;
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

// void ThreadPool::stop() {
//     std::lock_guard<std::mutex> lock(mtx_);
//     stopFlag_ = true;
//     cv_.notify_all();  // Wake up all threads so they can stop
// }
// Method to gracefully stop the thread pool
void ThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        stopFlag_ = true;
    }
    cv_.notify_all();  // Wake up all threads
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();  // Wait for all worker threads to finish
        }
    }
}
