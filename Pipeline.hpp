#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <sstream>
#include "MST_graph.hpp"
#define LONGEST_PATH 1
#define SHORTEST_PATH 2
#define AVERAGE_PATH 3
#define TOTAL_WEIGHT 4

class Pipeline {
public:
    Pipeline(const MST_graph& mst, std::ostringstream& sharedAns, std::mutex& mtxAns);
    ~Pipeline();

    void addTask(std::function<void()> task);
    void stop();

private:
    void runTasks();
    
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::thread workerThread_;
    bool stopFlag;
    const MST_graph& mst_;
    std::ostringstream& sharedAns_;
    std::mutex& mtxAns_;
};

#endif // PIPELINE_HPP
