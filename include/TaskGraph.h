#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Edge {
    int32_t targetTaskId;
    int32_t bandwidth;
};

enum class PeType {
    PP,
    HC,
};

struct Processor {
    int32_t cost;
    PeType type;
    constexpr bool isHC() const;
    constexpr bool isPP() const;
};

struct Channel {
    int cost;
    int bandwidth;
    std::string name;
    std::vector<int32_t> connected_processor;
};

class TaskGraph {
    size_t numTasks;
    size_t numProcessors;
    size_t numChannels;
    size_t hardTime;

    // Lista sąsiedztwa: adjList[taskId] zawiera krawędzie wychodzących z danego zadania.
    std::vector<std::vector<Edge>> adjList;

    std::vector<Processor> processors;
    std::vector<Channel> channels;

    // [procId][taskId]
    std::vector<std::vector<int32_t>> times;
    // [procId][taskId]
    std::vector<std::vector<int32_t>> costs;

public:
    // Zwraca czy dany procesor może wykonać dane zadanie
    bool canExecute(size_t procId, size_t taskId) const;
    bool isConnected(size_t chanelId, size_t procId) const;
    int32_t getTime(size_t procId, size_t taskId) const;
    int32_t getCost(size_t procId, size_t taskId) const;
    size_t getTaskCount() const;
    size_t getProcessorsCount() const;
    size_t getChannelsCount() const;
};
