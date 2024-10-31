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
using namespace std;

// ActiveObject class that processes a task
struct ActiveObject {
    std::thread* workerThread;
    std::queue<void*> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stopFlag;
    std::function<void(void*)> function;  // The function that the worker will execute
    queue<void*>* nextActiveObject;       // Queue for the next ActiveObject
    int id;
   
    ActiveObject(std::function<void(void*)> func, int id) : stopFlag(false), function(func), workerThread(nullptr), nextActiveObject(nullptr), id(id) {}





    ActiveObject() {};

    // Copy constructor
    ActiveObject(const ActiveObject& other) {
        stopFlag = other.stopFlag;
        function = other.function;
        nextActiveObject = other.nextActiveObject;
        id = other.id;
    }

    void addTask(void* task) {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(task);
        cv.notify_one();
        cout << "[DEBUG] Task added to ActiveObject. Queue size: " << tasks.size() << endl;
    }

    void runTasks(vector<ActiveObject>& activeObjects) {
        // print rthed id
        cout << "in runtask" << endl;
        std::cout << "Worker thread started. Thread ID: " << std::this_thread::get_id() << std::endl;

        while (true) {
            void* task;
            {
                std::cout << "Worker thread wait Thread ID: " << std::this_thread::get_id() << std::endl;
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this] { return !tasks.empty() || stopFlag; });
                cout << std::this_thread::get_id() << "after wait" << endl;

                if (stopFlag && tasks.empty()) {
                    cout << "[DEBUG] Stopping ActiveObject due to stopFlag and empty queue." << endl;
                    return;
                }
                if (!tasks.empty()) {
                    task = tasks.front();
                    tasks.pop();
                    cout << "[DEBUG] Task fetched from queue. Remaining tasks: " << tasks.size() << endl;
                }

                function(task);
                cout << "[DEBUG] Task executed." << endl;

                if (nextActiveObject) {
                    cout << "activeObject: " << id << "[DEBUG] Passing task to next ActiveObject." << endl;
                    nextActiveObject->push(task);
                    if (id + 1 < activeObjects.size()) {
                        activeObjects[id + 1].cv.notify_one();
                    }
                    cout << "[DEBUG] Task passed to next ActiveObject." << endl;
                } else {
                    cout << "[DEBUG] No next ActiveObject." << endl;
                }
            }
        }
    }
};

// Pipeline class that manages multiple ActiveObjects (workers)
class Pipeline {
    std::mutex mtx_;
    std::condition_variable cv_;
    std::vector<ActiveObject> activeObjects;
// Task 1: Longest Distance
    static void taskLongestDistance(void* task) {
        auto* taskTuple = static_cast<tuple<MST_graph*, string*>*>(task);
        MST_graph* mst_graph = std::get<0>(*taskTuple);
        string* clientAns = std::get<1>(*taskTuple);
        
        MST_stats stats;
        *clientAns += "Longest Distance: " + std::to_string(stats.getLongestDistance(*mst_graph)) + "\n";
        cout << "[DEBUG] Longest Distance calculated: " << *clientAns << endl;
    }

    // Task 2: Shortest Distance
    static void taskShortestDistance(void* task) {
        auto* taskTuple = static_cast<tuple<MST_graph*, string*>*>(task);
        MST_graph* mst_graph = std::get<0>(*taskTuple);
        string* clientAns = std::get<1>(*taskTuple);
        
        MST_stats stats;
        *clientAns += "Shortest Distance: " + std::to_string(stats.getShortestDistance(*mst_graph)) + "\n";
        cout << "[DEBUG] Shortest Distance calculated: " << *clientAns << endl;
    }

    // Task 3: Average Distance
    static void taskAverageDistance(void* task) {
        auto* taskTuple = static_cast<tuple<MST_graph*, string*>*>(task);
        MST_graph* mst_graph = std::get<0>(*taskTuple);
        string* clientAns = std::get<1>(*taskTuple);
        
        MST_stats stats;
        *clientAns += "Average Distance: " + std::to_string(stats.getAverageDistance(*mst_graph)) + "\n";
        cout << "[DEBUG] Average Distance calculated: " << *clientAns << endl;
    }

    // Task 4: Total Weight
    static void taskTotalWeight(void* task) {
        auto* taskTuple = static_cast<tuple<MST_graph*, string*>*>(task);
        MST_graph* mst_graph = std::get<0>(*taskTuple);
        string* clientAns = std::get<1>(*taskTuple);
        
        MST_stats stats;
        *clientAns += "Total Weight: " + std::to_string(stats.getTotalWeight(*mst_graph)) + "\n";
        cout << "[DEBUG] Total Weight calculated: " << *clientAns << endl;
    }

   public:
    Pipeline() {
        // Create ActiveObjects (workers) for each task
        activeObjects.push_back(ActiveObject(Pipeline::taskLongestDistance, 0));   // Task 1
        activeObjects.push_back(ActiveObject(Pipeline::taskShortestDistance, 1));  // Task 2
        activeObjects.push_back(ActiveObject(Pipeline::taskAverageDistance, 2));   // Task 3
        activeObjects.push_back(ActiveObject(Pipeline::taskTotalWeight, 3));       // Task 4

        for (int i = 0; i < activeObjects.size() - 1; ++i) {
            activeObjects[i].nextActiveObject = &activeObjects[i + 1].tasks;
        }
        cout << "[DEBUG] Pipeline initialized with " << activeObjects.size() << " ActiveObjects." << endl;
    }

    void addRequest(void* task) {
        activeObjects[0].addTask(task);
        activeObjects[0].cv.notify_one();
        cout << "[DEBUG] Task added to Pipeline. Initial ActiveObject notified." << endl;
    }

    void start() {
        cout << "[DEBUG] Pipeline starting all worker threads." << endl;
        for (auto& activeObject : activeObjects) {
            activeObject.workerThread = new std::thread(&ActiveObject::runTasks, &activeObject, std::ref(activeObjects));
            cout << "[DEBUG] Worker thread started for ActiveObject ID: " << activeObject.id << endl;

            cout << "[DEBUG] Worker thread started for ActiveObject." << endl;
        }
    }
    void stop() {
        cout << "[DEBUG] Pipeline stopping all worker threads." << endl;
        for (auto& activeObject : activeObjects) {
            activeObject.stopFlag = true;
            activeObject.cv.notify_one();
            activeObject.workerThread->join();
            cout << "[DEBUG] Worker thread stopped for ActiveObject." << endl;
        }
    }
};
