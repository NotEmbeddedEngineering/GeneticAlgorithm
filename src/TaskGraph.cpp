#include "TaskGraph.h"

#include <format>
#include <stdexcept>

constexpr bool Processor::isHC() const {
    return type == 0;
}

constexpr bool Processor::isPP() const {
    return type == 1;
}

bool TaskGraph::canExecute(size_t procId, size_t taskId) const {
    return getTime(procId, taskId) != -1 && getCost(procId, taskId) != -1;
}

int TaskGraph::getTime(size_t procId, size_t taskId) const {
    if (procId >= numProcessors || taskId >= numTasks) {
        throw std::logic_error(
            std::format("Resource out of range! Requested time for task {}/{} and proc {}/{}",
                        taskId, numTasks - 1, procId, numProcessors - 1));
    }
    return times[procId][taskId];
}

int TaskGraph::getCost(size_t procId, size_t taskId) const {
    if (procId >= numProcessors || taskId >= numTasks) {
        throw std::logic_error(
            std::format("Resource out of range! Requested cost for task {}/{} and proc {}/{}",
                        taskId, numTasks - 1, procId, numProcessors - 1));
    }
    return costs[procId][taskId];
}

int TaskGraph::getTime(size_t procId, size_t taskId) {
    if (procId >= numProcessors || taskId >= numTasks) {
        throw std::logic_error(
            std::format("Resource out of range! Requested time for task {}/{} and proc {}/{}",
                        taskId, numTasks - 1, procId, numProcessors - 1));
    }
    return times[procId][taskId];
}

int TaskGraph::getCost(size_t procId, size_t taskId) {
    if (procId >= numProcessors || taskId >= numTasks) {
        throw std::logic_error(
            std::format("Resource out of range! Requested cost for task {}/{} and proc {}/{}",
                        taskId, numTasks - 1, procId, numProcessors - 1));
    }
    return costs[procId][taskId];
}
