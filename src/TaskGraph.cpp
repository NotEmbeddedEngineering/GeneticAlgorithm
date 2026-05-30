#include "TaskGraph.h"

#include <cstddef>
#include <format>
#include <stdexcept>

constexpr bool Processor::isHC() const {
    return type == PeType::HC;
}

constexpr bool Processor::isPP() const {
    return type == PeType::PP;
}

bool TaskGraph::canExecute(size_t procId, size_t taskId) const {
    return getTime(procId, taskId) != -1 && getCost(procId, taskId) != -1;
}

bool TaskGraph::isConnected(size_t chanelId, size_t procId) const {
    // TODO add implementation
    return false;
}

int32_t TaskGraph::getTime(size_t procId, size_t taskId) const {
    if (procId >= numProcessors || taskId >= numTasks) {
        throw std::logic_error(
            std::format("Resource out of range! Requested time for task {}/{} and proc {}/{}",
                        taskId, numTasks - 1, procId, numProcessors - 1));
    }
    return times[procId][taskId];
}

int32_t TaskGraph::getCost(size_t procId, size_t taskId) const {
    if (procId >= numProcessors || taskId >= numTasks) {
        throw std::logic_error(
            std::format("Resource out of range! Requested cost for task {}/{} and proc {}/{}",
                        taskId, numTasks - 1, procId, numProcessors - 1));
    }
    return costs[procId][taskId];
}

size_t TaskGraph::getTaskCount() const {
    return this->numTasks;
}
size_t TaskGraph::getProcessorsCount() const {
    return this->numProcessors;
}
size_t TaskGraph::getChannelsCount() const {
    return this->numChannels;
}
