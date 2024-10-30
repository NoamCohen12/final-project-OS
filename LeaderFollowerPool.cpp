#include "LeaderFollowerPool.hpp"

#include <chrono>
#include <iostream>

void LeaderFollowerPool::start() {
    {
        lock_guard<mutex> lock(mutexstop);  // Lock the mutex
        stopFlag_ = false;  
    }
}
// Update constructor to accept shared answer stream and mutex
LeaderFollowerPool::LeaderFollowerPool(int numThreads)
    : numThreads_(numThreads), stopFlag_(false) {
    for (int i = 0; i < numThreads_; ++i) {
        workers_.emplace_back(&LeaderFollowerPool::leaderRole, this);
       // threadQueue_.push(workers_.back().get_id());
    }
}

LeaderFollowerPool::~LeaderFollowerPool() {
    stop();
}


void LeaderFollowerPool::addEventHandler(function<void()> task) {
    {
    std::lock_guard<std::mutex> lock(mutexqueue);
    eventQueue_.push(task);
    }
    cv_.notify_one();  // Notify the leader thread to handle the event
}
void LeaderFollowerPool::leaderRole() {
    while (true) {
        function<void()> currentRequest;

        {
            std::unique_lock<std::mutex> lock(mutexqueue);
            cv_.wait(lock, [this] { return !eventQueue_.empty() || stopFlag_; });

            if (stopFlag_ && eventQueue_.empty()) {
                return;  // Shutdown check
            }

            if (!eventQueue_.empty()) {
                // Get the next task
                currentRequest = std::move(eventQueue_.front());
                eventQueue_.pop();
            } else {
                continue;  // Recheck if spurious wakeup
            }
        }

        // Execute the task outside the lock
        if (currentRequest) {
            currentRequest();
        }

        // Notify other threads that a new leader can be elected
        cv_.notify_one();
    }
}


// void LeaderFollowerPool::followerRole() {
//     {
//         std::lock_guard<std::mutex> lock(mutexqueue);
//         if (!threadQueue_.empty()) {
//             threadQueue_.pop();  // Remove current leader
//         }
//     }
//     cv_.notify_one();  // Wake up next thread to be leader
// }

// Method to gracefully stop the thread pool
void LeaderFollowerPool::stop() {
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



// void waitForCompletion() {
//         std::unique_lock<std::mutex> lock(mutexqueue);
//         cv_.wait(lock, [this] { return completed; });
// }