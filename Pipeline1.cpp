#include "Pipeline.hpp"

Pipeline::Pipeline(const MST_graph& mst, std::ostringstream& sharedAns, std::mutex& mtxAns)
    : stopFlag(false), mst_(mst), sharedAns_(sharedAns), mtxAns_(mtxAns) {
    // Launch worker thread that will run tasks
    workerThread_ = std::thread(&Pipeline::runTasks, this);
}

Pipeline::~Pipeline() {
    stop();  // Stop the pipeline when it's destroyed
    if (workerThread_.joinable()) {
        workerThread_.join();  // Ensure the thread finishes
    }
}

void Pipeline::addTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        tasks_.push(task);
        std::cout << "Debug: Task added to queue. Queue size: " << tasks_.size() << std::endl;
    }
    cv_.notify_one();
}

void Pipeline::runTasks() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return !tasks_.empty() || stopFlag; });

            if (stopFlag && tasks_.empty()) {
                return; // Exit the thread
            }

            task = tasks_.front();
            tasks_.pop();
        }

        task();  // Execute the task

        std::cout << "Debug: Task executed." << std::endl;
    }
}

void Pipeline::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        stopFlag = true;
        std::cout << "Debug: Stop flag set." << std::endl;
    }
    cv_.notify_all();
}
