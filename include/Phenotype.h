#pragma once

#include "TaskGraph.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Phenotype {
public:
    // Creates base phenotype (fastest) with provided architecture.
    explicit Phenotype(const std::shared_ptr<TaskGraph> graph);

    // wylicza fitnessScore i wpisuje go
    void evaluate();

    [[nodiscard]] double getFitnessScore() const {
        return fitnessScore;
    }

    // Gets taskgraph procId based on internal procId
    size_t getTgProcId(size_t phenotypeProcId) const;
    // Gets taskId internall procesor
    size_t getPhenotypeProcId(size_t taskId) const;

    // Adds internally new proc based on TaskGraph processor id and returns internal mapping
    size_t addProc(size_t tgProcId);

    // Change processor assigned to task. (Takes care of processor usage)
    void changeTaskProc(size_t taskId, size_t phenotypeProcId);

    // Check all edge cases with end architecture.
    // - porcessor not conected to chanells allowing communication
    bool isValidArchitecture() const;

    std::shared_ptr<TaskGraph> getGraph() const;

private:
    std::shared_ptr<TaskGraph> graph;

    // --- Aktualny Stan (DNA) ---
    // taskToPhenotypeProcessor[taskId] = phenotypeProcId
    std::vector<size_t> taskToPhenotypeProcessor{};
    // Maps Internal phenotypeProcId to Task Graph Processor
    std::vector<size_t> phenotypeProcToTgProc{};

    // phenotypeProcToChannel[phenotypeProcId] = std::unordered_set<channelId>
    // All chanells conected to this processor
    std::vector<std::unordered_set<size_t>> phenotypeProcToPhenotypeChannel{};
    std::vector<size_t> phenotypeChannelToTgChannel{};

    // Stores how much time each phenotype proc uses
    std::vector<int32_t> phenotypeProcUsage{};

    // --- Wyniki ---
    int32_t time = -1;
    int32_t cost = -1;
    double fitnessScore = 0.0;

    // Czy przekracza hard time constrains
    // bool isValid();
};
