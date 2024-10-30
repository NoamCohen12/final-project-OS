#include "ThreadPool.hpp"

#include <chrono>
#include <iostream>

void ThreadPool::start() {
    {
        lock_guard<mutex> lock(mutexstop);  // Lock the mutex
        stopFlag_ = false;  
    }
}
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
}


void ThreadPool::addEventHandler(function<void()> task) {
    {
    std::lock_guard<std::mutex> lock(mutexqueue);
    eventQueue_.push(task);
    }
    cv_.notify_one();  // Notify the leader thread to handle the event
}
void ThreadPool::leaderRole() {
    while (true) {
        function<void()> currentRequest;
        thread::id nextLeader;
        
        {
            std::unique_lock<std::mutex> lock(mutexqueue);
            cv_.wait(lock, [this] { 
                return !eventQueue_.empty() || stopFlag_; 
            });

            if (stopFlag_ && eventQueue_.empty()) {
                return;
            }

            if (!eventQueue_.empty()) {
                // 1. Get the next task
                currentRequest = move(eventQueue_.front());
                eventQueue_.pop();
                
                // 2. Get next leader from front of queue
                if (!threadQueue_.empty()) {
                    nextLeader = threadQueue_.front();
                    threadQueue_.pop();
                }
            }
        }

        if (currentRequest) {
            // 3. Execute the task first
            currentRequest();
            
            // 4. After task is complete, put self back in queue
            {
                std::lock_guard<std::mutex> lock(mutexqueue);
                threadQueue_.push(std::this_thread::get_id());
            }
            
            // 5. Wake up next leader
            cv_.notify_one();
        }
    }
}

void ThreadPool::followerRole() {
    {
        std::lock_guard<std::mutex> lock(mutexqueue);
        if (!threadQueue_.empty()) {
            threadQueue_.pop();  // Remove current leader
        }
    }
    cv_.notify_one();  // Wake up next thread to be leader
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



// void waitForCompletion() {
//         std::unique_lock<std::mutex> lock(mutexqueue);
//         cv_.wait(lock, [this] { return completed; });
// }