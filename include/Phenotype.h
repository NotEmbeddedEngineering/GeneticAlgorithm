#pragma once

#include "TaskGraph.h"

#include <memory>

class Phenotype {
public:
    explicit Phenotype(const std::shared_ptr<TaskGraph>& graph);

    // wylicza fitnessScore i wpisuje go
    void evaluate();

    [[nodiscard]] double getFitnessScore() const {
        return fitnessScore;
    }

    // --- Aktualny Stan (DNA) ---
    // taskToProcessor[taskId] = processorId
    std::vector<size_t> taskToProcessor;

    // processorToChannel[processorId] = channelId
    // -1 oznacza brak przypisanej szyny
    // TODO rozróżnienie w przypadku dwóch procesorów PP
    std::vector<size_t> processorToChannel;

    std::shared_ptr<TaskGraph> graph;

private:
    // --- Wyniki ---
    int time = -1;
    int cost = -1;

    // Czy przekracza hard time constrains
    bool isValid();

    double fitnessScore = 0.0;
};
