#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "MST_graph.hpp"
#include "MST_stats.hpp"

// ActiveObject class definition
class ActiveObject {
   private:
    std::thread workerThread;
    std::queue<MST_graph*> trees;
    std::mutex mtx;
    std::condition_variable cv;
    std::function<void()> task_;  // Task to execute
    bool stopFlag;

   public:
    // Default constructor
    ActiveObject() : stopFlag(false) {}

    // Constructor to initialize with a task
    ActiveObject(std::function<void()> task) : task_(task), stopFlag(false) {
        workerThread = std::thread(&ActiveObject::runTasks_obj, this);
    }

    // Destructor
    ~ActiveObject() {
        stop();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    // Add tree to the queue
    void addTree(MST_graph* tree) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            trees.push(tree);
        }
        cv.notify_one();
    }

    // Get tree from the queue
    MST_graph* getTree() {
        std::lock_guard<std::mutex> lock(mtx);
        if (trees.empty()) {
            return nullptr;
        }
        MST_graph* tree = trees.front();
        trees.pop();
        return tree;
    }

    // Stop the ActiveObject
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stopFlag = true;
        }
        cv.notify_all();
    }

    // Run tasks
    void runTasks_obj() {
        while (true) {
            MST_graph* tree = nullptr;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this] { return !trees.empty() || stopFlag; });

                if (stopFlag && trees.empty()) {
                    return;
                }
                tree = trees.front();
                trees.pop();
            }

            // Execute the task
            task_();  // Execute the task
            delete tree;
        }
    }
};

// Pipeline class definition
class Pipeline {
   private:
    int num_of_objects;
    std::vector<std::unique_ptr<ActiveObject>> activeObjects;  // Use unique_ptr for ActiveObject
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stopFlag;
    MST_graph mst_;
    std::ostringstream& sharedAns_;  // Shared output
    std::mutex& mtxAns_;

   public:
    Pipeline(int num_of_objects, MST_graph& mst, std::ostringstream& sharedAns, std::mutex& mtxAns)
        : mst_(mst), sharedAns_(sharedAns), mtxAns_(mtxAns), stopFlag(false), num_of_objects(num_of_objects) {
        // Reserve space for unique_ptrs to active objects

        activeObjects.reserve(num_of_objects);
    }

void addTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        // Add new ActiveObject to vector using std::make_unique
        activeObjects.emplace_back(std::make_unique<ActiveObject>(std::move(task)));
        std::cout << "Debug: Task added to active object. Total active objects: " << activeObjects.size() << std::endl;
    }
    cv_.notify_one();
}


void stop() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        stopFlag = true;
    }
    cv_.notify_all();

    // Stop all active objects
    for (auto& obj : activeObjects) {
        obj->stop();
    }
}    void runTasks() {
        // Iterate over all active objects and run tasks on the tree
        for (int i = 0; i < num_of_objects; ++i) {
            activeObjects[i]->runTasks_obj();
            if (i + 1 < activeObjects.size()) {
                activeObjects[i + 1]->addTree(activeObjects[i]->getTree());  // Transfer tree to next object
            }
        }
    }
};

#endif  // PIPELINE_HPP
