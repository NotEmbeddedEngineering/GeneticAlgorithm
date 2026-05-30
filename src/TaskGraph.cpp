#include "TaskGraph.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

constexpr bool Processor::isHC() const {
    return type == PeType::HC;
}

constexpr bool Processor::isPP() const {
    return type == PeType::PP;
}

bool TaskGraph::canExecute(size_t procId, size_t taskId) const {
    return getTime(procId, taskId) != -1 && getCost(procId, taskId) != -1;
}

bool TaskGraph::isConnected(size_t channelId, size_t procId) const {
    return channels[channelId].isProcessorConnected[procId];
}

int32_t TaskGraph::getTime(size_t procId, size_t taskId) const {
    if (procId >= this->getProcessorsCount() || taskId >= this->getTaskCount()) {
        throw std::logic_error(
            std::format("Resource out of range! Requested time for task {}/{} and proc {}/{}",
                        taskId, this->getTaskCount() - 1, procId, this->getProcessorsCount() - 1));
    }
    return times[procId][taskId];
}

int32_t TaskGraph::getCost(size_t procId, size_t taskId) const {
    if (procId >= this->getProcessorsCount() || taskId >= this->getTaskCount()) {
        throw std::logic_error(
            std::format("Resource out of range! Requested cost for task {}/{} and proc {}/{}",
                        taskId, this->getTaskCount() - 1, procId, this->getProcessorsCount() - 1));
    }
    return costs[procId][taskId];
}

size_t TaskGraph::getTaskCount() const {
    return this->adjList.size();
}
size_t TaskGraph::getProcessorsCount() const {
    return this->processors.size();
}
size_t TaskGraph::getChannelsCount() const {
    return this->channels.size();
}

TaskGraph::TaskGraph(std::string filePath) {
    std::fstream fileStream;
    fileStream.open(filePath, std::ios::in);

    if (!fileStream) {
        throw std::runtime_error(
            std::format("Cannot create file stream from path :\"{}\"", filePath));
    }

    int32_t taskCount = -1;
    int32_t processorCount = -1;
    int32_t chanelCount = -1;

    std::string lineStart{};
    while (!fileStream.eof()) {
        fileStream >> lineStart;
        // FIX: Idk it still holds not empty stream at the end of file
        if (fileStream.eof()) {
            break;
        }
        if (lineStart.starts_with("@tasks")) {
            fileStream >> taskCount;
            this->adjList = this->parseTasks(fileStream, taskCount);
        } else if (lineStart.starts_with("@proc")) {
            fileStream >> processorCount;
            this->processors = this->parseProc(fileStream, processorCount);
        } else if (lineStart.starts_with("@times")) {
            this->times = this->parseTimes(fileStream, taskCount, processorCount);
        } else if (lineStart.starts_with("@cost")) {
            this->costs = this->parseCost(fileStream, taskCount, processorCount);
        } else if (lineStart.starts_with("@comm")) {
            fileStream >> chanelCount;
            this->channels = this->parseComms(fileStream, chanelCount, processorCount);
        }
    }
}

std::vector<std::vector<Edge>> TaskGraph::parseTasks(std::fstream& fileStream, uint32_t taskCount) {
    std::vector<std::vector<Edge>> adj(taskCount);

    for (uint32_t tId = 0; tId < taskCount; ++tId) {
        char skipC{};
        uint32_t readTaskId{};
        uint32_t successors{};
        fileStream >> skipC >> readTaskId >> successors;
        for (uint32_t sId = 0; sId < successors; ++sId) {
            uint32_t successorTask = 0;
            uint32_t successorTaskData = 0;
            fileStream >> successorTask >> skipC >> successorTaskData >> skipC;
            adj[readTaskId].emplace_back(successorTask, successorTaskData);
        }
    }
    return adj;
}

std::vector<Processor> TaskGraph::parseProc(std::fstream& fileStream, uint32_t processorCount) {
    std::vector<Processor> proc{};
    for (uint32_t pId = 0; pId < processorCount; ++pId) {
        int32_t cost{};
        int32_t other{};
        int32_t type{};
        fileStream >> cost >> other >> type;
        if (type == 0) {
            proc.emplace_back(cost, other, PeType::HC);
        } else {
            proc.emplace_back(cost, other, PeType::PP);
        }
    }
    return proc;
}

std::vector<std::vector<int32_t>>
TaskGraph::parseTimes(std::fstream& fileStream, uint32_t taskCount, uint32_t processorCount) {
    std::vector<std::vector<int32_t>> time(processorCount, std::vector<int32_t>(taskCount));
    for (uint32_t tId = 0; tId < taskCount; ++tId) {
        for (uint32_t pId = 0; pId < processorCount; ++pId) {
            fileStream >> time[pId][tId];
        }
    }
    return time;
}

std::vector<std::vector<int32_t>> TaskGraph::parseCost(std::fstream& fileStream, uint32_t taskCount,
                                                       uint32_t processorCount) {
    std::vector<std::vector<int32_t>> cost(processorCount, std::vector<int32_t>(taskCount));
    for (uint32_t tId = 0; tId < taskCount; ++tId) {
        for (uint32_t pId = 0; pId < processorCount; ++pId) {
            fileStream >> cost[pId][tId];
        }
    }
    return cost;
}

std::vector<Channel> TaskGraph::parseComms(std::fstream& fileStream, uint32_t chanelCount,
                                           uint32_t processorCount) {
    std::vector<Channel> chanels{};
    for (uint32_t chId = 0; chId < chanelCount; ++chId) {
        auto chan = Channel{{}, 0, 0, std::vector<bool>(processorCount)};
        fileStream >> chan.name >> chan.cost >> chan.bandwidth;
        for (uint32_t pId = 0; pId < processorCount; ++pId) {
            bool isConnected{};
            fileStream >> isConnected;
            chan.isProcessorConnected[pId] = isConnected;
        }
        chanels.push_back(chan);
    }
    return chanels;
}