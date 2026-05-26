#pragma once

#include "TaskGraph.h"

class Phenotype {
public:
    explicit Phenotype(const TaskGraph& graph);

    // wylicza fitnessScore i wpisuje go
    void evaluate();

private:
    // TODO CHANGE TO SHARED POINTER
    TaskGraph* graph;

    // --- Aktualny Stan (DNA) ---
    // taskToProcessor[taskId] = processorId
    std::vector<int> taskToProcessor;

    // processorToChannel[processorId] = channelId
    // -1 oznacza brak przypisanej szyny
    std::vector<int> processorToChannel;

    // --- Wyniki ---
    int time = -1;
    int cost = -1;

    // Czy przekracza hard time constrains
    bool isValid();

    double fitnessScore = 0.0;
};
