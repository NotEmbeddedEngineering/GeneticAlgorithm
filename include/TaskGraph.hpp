#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_set>
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
    int32_t other;
    PeType type;
    bool isHC() const;
    bool isPP() const;
};

struct Channel {
    std::string name;
    int32_t cost;
    int32_t bandwidth;
    // Stores if proc with given id is connected
    std::unordered_set<size_t> connectedProcessors;
};

class TaskGraph {
private:
    // Lista sąsiedztwa: adjList[taskId] zawiera krawędzie wychodzących z danego zadania.
    std::vector<std::vector<Edge>> adjList;

    std::vector<Processor> processors;
    std::vector<Channel> channels;
    // [procId][taskId]
    std::vector<std::vector<int32_t>> times;
    // [procId][taskId]
    std::vector<std::vector<int32_t>> costs;

public:
    TaskGraph(std::string filePath);
    // Zwraca czy dany procesor może wykonać dane zadanie
    bool canExecute(size_t procId, size_t taskId) const;
    bool isConnected(size_t channelId, size_t procId) const;
    int32_t getTime(size_t procId, size_t taskId) const;
    int32_t getCost(size_t procId, size_t taskId) const;
    size_t getTaskCount() const;
    size_t getProcessorsCount() const;
    size_t getChannelsCount() const;

    const std::vector<std::vector<Edge>>& getAdj() const;
    const Processor& getProc(size_t procId) const;
    const Channel& getChan(size_t chanId) const;

    // Finds fastest time between two procesors, and fastest chan for sam type of HC
    int32_t findFastestChanel(size_t proc1Id, size_t proc2Id) const;

private:
    // =================
    // PARSING
    // =================
    static std::vector<std::vector<Edge>> parseTasks(std::fstream& fileStream, uint32_t taskCount);
    static std::vector<Processor> parseProc(std::fstream& fileStream, uint32_t pe);

    static std::vector<std::vector<int32_t>>
    parseTimes(std::fstream& fileStream, uint32_t taskCount, uint32_t processorCount);

    static std::vector<std::vector<int32_t>> parseCost(std::fstream& fileStream, uint32_t taskCount,
                                                       uint32_t processorCount);

    static std::vector<Channel> parseComms(std::fstream& fileStream, uint32_t chanelCount,
                                           uint32_t processorCount);
};
