

#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>

#include "Graph.cpp"
#include "MST_graph.hpp"
#include "MST_stats.hpp"
#include "MST_strategy.hpp"

// ActiveObject class that processes a task
class ActiveObject {
    std::thread workerThread;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stopFlag;

   public:
    ActiveObject() : stopFlag(false) {
        workerThread = std::thread(&ActiveObject::runTasks, this);
    }

    ~ActiveObject() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stopFlag = true;
            cv.notify_one();
        }
        workerThread.join();
    }

    void addTask(std::function<void()> task) {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(task);
        cv.notify_one();
    }

    void runTasks() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this] { return !tasks.empty() || stopFlag; });

                if (stopFlag && tasks.empty())
                    return;

                task = tasks.front();
                tasks.pop();
            }
            task();  // Execute the task
        }
    }
};

// Pipeline class that manages multiple ActiveObjects (workers)
class Pipeline {
    std::mutex mtx_;
    std::condition_variable cv_;
    bool completed = false;
    std::vector<std::unique_ptr<ActiveObject>> activeObjects;
    std::ostringstream& sharedAns;  // Change the type to std::ostringstream
    // Task 1: Construct MST from graph
    MST_graph task1(Graph& graph) {
        MST_strategy MST_strategy;
        return MST_strategy.prim(graph.getEdges(), graph.getnumVertices());  // MST creation using Prim's algorithm
    }

    // Task 2-5: MST statistics
    std::string task2(const MST_graph& mst_graph) {
        MST_stats stats;
        return to_string(stats.getLongestDistance(mst_graph));
    }

    std::string task3(const MST_graph& mst_graph) {
        MST_stats stats;
        return to_string(stats.getShortestDistance(mst_graph));
    }

    std::string task4(const MST_graph& mst_graph) {
        MST_stats stats;
        return to_string(stats.getAverageDistance(mst_graph));
    }

    std::string task5(const MST_graph& mst_graph) {
        MST_stats stats;
        return to_string(stats.getTotalWeight(mst_graph));
    }

   public:
    Pipeline(std::ostringstream& sharedAns) : sharedAns(sharedAns) {
        // Create ActiveObjects (workers) for each task
        activeObjects.push_back(std::make_unique<ActiveObject>());  // Task 1
        activeObjects.push_back(std::make_unique<ActiveObject>());  // Task 2
        activeObjects.push_back(std::make_unique<ActiveObject>());  // Task 3
        activeObjects.push_back(std::make_unique<ActiveObject>());  // Task 4
        activeObjects.push_back(std::make_unique<ActiveObject>());  // Task 5
    }

    void processGraph(Graph& graph) {
        std::unique_lock<std::mutex> lock(mtx_);
    completed = false;  // Reset the completion flag

    // Start by creating an MST from the graph
    activeObjects[0]->addTask([this, &graph]() {
        MST_graph mst_graph = task1(graph);
        sharedAns << mst_graph.to_string() << std::endl;
        std::cout << "MST created" << std::endl;

        // Pass result to next stage (longest distance)
        activeObjects[1]->addTask([this, mst_graph]() {
            sharedAns << "Longest Distance: " << task2(mst_graph) << std::endl;

            // Pass result to next stage (shortest distance)
            activeObjects[2]->addTask([this, mst_graph]() {
                sharedAns << "Shortest Distance: " << task3(mst_graph) << std::endl;

                // Pass result to next stage (average distance)
                activeObjects[3]->addTask([this, mst_graph]() {
                    sharedAns << "Average Distance: " << task4(mst_graph) << std::endl;

                    // Pass result to next stage (total weight)
                    activeObjects[4]->addTask([this, mst_graph]() {
                        sharedAns << "Total Weight: " << task5(mst_graph) << std::endl;

                        // Notify the main thread that processing is complete
                        std::unique_lock<std::mutex> lock(mtx_);
                        completed = true;
                        cv_.notify_one();
                    });
                });
            });
        });
    });
}

std::string waitForCompletion() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return completed; });
    return sharedAns.str();
}
};
