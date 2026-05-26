#pragma once

#include "TaskGraph.h"

class Phenotype {
public:
    explicit Phenotype(const TaskGraph& graph);

    // wylicza fitnessScore i wpisuje go
    void evaluate();

    double getFitnessScore() const {
        return fitnessScore;
    }

    // --- Aktualny Stan (DNA) ---
    // taskToProcessor[taskId] = processorId
    std::vector<int> taskToProcessor;

    // processorToChannel[processorId] = channelId
    // -1 oznacza brak przypisanej szyny
    // TODO rozróżnienie w przypadku dwóch procesorów PP
    std::vector<int> processorToChannel;

private:
    // TODO CHANGE TO SHARED POINTER
    TaskGraph* graph;

    // --- Wyniki ---
    int time = -1;
    int cost = -1;

    // Czy przekracza hard time constrains
    bool isValid();

    double fitnessScore = 0.0;
};
