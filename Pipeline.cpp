#include <iostream>
#include <memory>
#include <vector>
#include <sstream> // For std::ostringstream
#include "MST_graph.hpp"
#include "MST_stats.hpp"

// Pipeline Task interface
class PipelineTask {
public:
    virtual void execute(const MST_graph& mst, MST_stats& stats, std::ostringstream& output) = 0;
    virtual ~PipelineTask() = default;
};

// Concrete task implementations
class ComputeTotalWeight : public PipelineTask {
public:
    void execute(const MST_graph& mst, MST_stats& stats, std::ostringstream& output) override {
        int totalWeight = stats.getTotalWeight(mst);
        output << "Total Weight: " << totalWeight << "\n";
    }
};

class ComputeLongestDistance : public PipelineTask {
public:
    void execute(const MST_graph& mst, MST_stats& stats, std::ostringstream& output) override {
        int longestDistance = stats.getLongestDistance(mst);
        output << "Longest Distance: " << longestDistance << "\n";
    }
};

// Additional task classes (ComputeShortestDistance, ComputeAverageDistance)
class ComputeShortestDistance : public PipelineTask {
public:
    void execute(const MST_graph& mst, MST_stats& stats, std::ostringstream& output) override {
        int shortestDistance = stats.getShortestDistance(mst);
        output << "Shortest Distance: " << shortestDistance << "\n";
    }
};

class ComputeAverageDistance : public PipelineTask {
public:
    void execute(const MST_graph& mst, MST_stats& stats, std::ostringstream& output) override {
        double averageDistance = stats.getAverageDistance(mst);
        output << "Average Distance: " << averageDistance << "\n";
    }
};

// Pipeline manager
class Pipeline {
    std::ostringstream& sharedAns_;  // Reference to shared output stream
    std::vector<std::unique_ptr<PipelineTask>> stages;

public:
    Pipeline(std::ostringstream& sharedAns) : sharedAns_(sharedAns) {}

    void addStage(std::unique_ptr<PipelineTask> task) {
        stages.push_back(std::move(task));
    }

    void process(const MST_graph& mst, MST_stats& stats) {
        for (auto& stage : stages) {
            stage->execute(mst, stats, sharedAns_);
        }
    }
};
