#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct CommunicationEdge {
    int sourceTaskId;
    int targetTaskId;
    int bandwidth;
};

struct Processor {
    int cost;
    int type; // HC - 0, PP -1
    constexpr bool isHC() const;
    constexpr bool isPP() const;
};

struct Channel {
    int cost;
    int bandwidth;
    std::string name;
    std::vector<int> connected_processor;
};

struct TaskGraph {
    size_t numTasks;
    size_t numProcessors;
    size_t numChannels;
    size_t hardTime;

    // Wszystkie krawędzie komunikacyjne.
    // edgeId jest indeksem w tym wektorze.
    std::vector<CommunicationEdge> edges;

    // Lista sąsiedztwa: adjList[taskId] zawiera identyfikatory krawędzi edgeId
    // wychodzących z danego zadania.
    std::vector<std::vector<int>> adjList;

    std::vector<Processor> processors;
    std::vector<Channel> channels;

    // [procId][taskId]
    std::vector<std::vector<int>> times;
    // [procId][taskId]
    std::vector<std::vector<int>> costs;

    // Zwraca czy dany procesor może wykonać dane zadanie
    bool canExecute(size_t procId, size_t taskId) const;
    int getTime(size_t procId, size_t taskId) const;
    int getCost(size_t procId, size_t taskId) const;
    int getTime(size_t procId, size_t taskId);
    int getCost(size_t procId, size_t taskId);
};
