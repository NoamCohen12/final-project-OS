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
            cout << "Debug: Adding tree to queue." << endl;
            std::lock_guard<std::mutex> lock(mtx);
            trees.push(tree);
            std::cout << "Debug: Tree added to queue, queue size now: " << trees.size() << std::endl;
        }
        cv.notify_one();
    }

    // Get tree from the queue
    MST_graph* getTree() {
        std::lock_guard<std::mutex> lock(mtx);
        if (trees.empty()) {
            std::cout << "Debug: No tree in the queue." << std::endl;
            return nullptr;
        }
        MST_graph* tree = trees.front();
        trees.pop();
        std::cout << "Debug: Tree retrieved from queue, queue size now: " << trees.size() << std::endl;
        return tree;
    }

    // Stop the ActiveObject
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stopFlag = true;
            std::cout << "Debug: ActiveObject stop flag set to true." << std::endl;
        }
        cv.notify_all();  // Notify the worker thread
    }

    // Run tasks
    void runTasks_obj() {
        std::cout << "Debug: runTasks_obj started." << std::endl;
        while (true) {
            MST_graph* tree = nullptr;

            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !trees.empty() || stopFlag; });

            if (stopFlag && trees.empty()) {
                std::cout << "Debug: Exiting runTasks_obj as stopFlag is set and queue is empty." << std::endl;
                return;
            }

            if (!trees.empty()) {
                tree = trees.front();  // Get the next MST_graph
                trees.pop();
                std::cout << "Debug: Running task for tree, remaining trees in queue: " << trees.size() << std::endl;
            }

            if (tree && task_) {
                task_();  // Execute the task
                std::cout << "Debug: Task executed." << std::endl;
            }
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
        std::lock_guard<std::mutex> lock(mtx_);
        // Add new ActiveObject to vector using std::make_unique
        activeObjects.emplace_back(std::make_unique<ActiveObject>(std::move(task)));
        std::cout << "Debug: Task added to active object. Total active objects: " << activeObjects.size() << std::endl;
        cv_.notify_one();
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stopFlag = true;
            std::cout << "Debug: Pipeline stop flag set to true." << std::endl;
        }
        cv_.notify_all();  // Notify any waiting condition variable

        // Stop all active objects
        for (auto& obj : activeObjects) {
            obj->stop();  // Set the stop flag for each ActiveObject
            std::cout << "Debug: ActiveObject stopped." << std::endl;
        }
    }

    void runTasks() {
        activeObjects[0]->addTree(&mst_);  // Add the MST to the first ActiveObject
        // Iterate over all active objects and run tasks on the tree
        for (int i = 0; i < num_of_objects; ++i) {
            std::cout << "Debug: Running tasks for ActiveObject " << i << std::endl;
            activeObjects[i]->runTasks_obj();
            cout << "Debug: Tasks completed for ActiveObject " << i << std::endl;
            cout << "Debug: size active objects: " << activeObjects.size() << endl;
            cout << "i+1:" << i + 1 << endl;
            if (i + 1 < activeObjects.size()) {
                activeObjects[i + 1]->addTree(activeObjects[i]->getTree());  // Transfer tree to next object
                std::cout << "Debug: Transferred tree from ActiveObject " << i << " to ActiveObject " << i + 1 << std::endl;
            }
        }
    }
};
