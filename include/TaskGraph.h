#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <format>

struct CommunicationEdge {
    int sourceTaskId;
    int targetTaskId;
    int bandwidth;
};

struct Processor {
    int cost;
    int type; // HC - 0, PP -1
    constexpr bool isHC() const {return type == 0;}
    constexpr bool isPP() const {return type == 1;}
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

    // Zwraca czy dany procesor może wykonać dane zadanie
    bool canExecute(size_t procID, size_t taskID) const{
        return getTime(procID,taskID) != -1 && getCost(procID,taskID) != -1; 
    }
    int getTime(size_t procID, size_t taskID)const{
        if(procID >= numProcessors || taskID >= numTasks) throw std::logic_error(std::format("Resource out of range! Requested time for task {}/{} and proc {}/{}",taskID,numTasks-1,procID,numProcessors-1));
        return times[procID][taskID];
    }
    int getCost(size_t procID, size_t taskID)const{
        if(procID >= numProcessors || taskID >= numTasks) throw std::logic_error(std::format("Resource out of range! Requested cost for task {}/{} and proc {}/{}",taskID,numTasks-1,procID,numProcessors-1));
        return costs[procID][taskID];
    }
    int& getTime(size_t procID, size_t taskID){
        if(procID >= numProcessors || taskID >= numTasks) throw std::logic_error(std::format("Resource out of range! Requested time for task {}/{} and proc {}/{}",taskID,numTasks-1,procID,numProcessors-1));
        return times[procID][taskID];
    }
    int& getCost(size_t procID, size_t taskID){
        if(procID >= numProcessors || taskID >= numTasks) throw std::logic_error(std::format("Resource out of range! Requested cost for task {}/{} and proc {}/{}",taskID,numTasks-1,procID,numProcessors-1));
        return costs[procID][taskID];
    }
};
