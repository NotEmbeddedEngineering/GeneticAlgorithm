#pragma once

#include <string>
#include <vector>

struct CommunicationEdge {
    int sourceTaskId;
    int targetTaskId;
    int bandwidth;
};

struct Processor {
    int cost;
    int type;
};

struct Channel {
    int cost;
    int bandwidth;
    std::string name;
    std::vector<int> connected_processor;
};

struct TaskGraph {
    int numTasks;
    int numProcessors;
    int numChannels;
    int hardTime;

    // Wszystkie krawędzie komunikacyjne.
    // edgeId jest indeksem w tym wektorze.
    std::vector<CommunicationEdge> edges;

    // Lista sąsiedztwa: adjList[taskId] zawiera identyfikatory krawędzi edgeId
    // wychodzących z danego zadania.
    std::vector<std::vector<int>> adjList;

    std::vector<Processor> processors;
    std::vector<Channel> channels;

    std::vector<std::vector<int>> times;
    std::vector<std::vector<int>> costs;
};
