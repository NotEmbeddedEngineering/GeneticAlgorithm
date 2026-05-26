#pragma once

#include "FunctionType.h"
#include "Phenotype.h"
#include <memory>
#include <optional>
#include <vector>

// Struktura Wierzcholka
struct Node {
    FunctionType type;
    // Parametry węzła, na którym robimy modyfikacje
    std::optional<int> targetTaskId;
    std::optional<int> targetProcessorId;
    std::optional<int> targetChannelId;

    std::vector<std::unique_ptr<Node>> children;

    explicit Node(const FunctionType t, const std::optional<int> taskId = std::nullopt,
                  const std::optional<int> processorId = std::nullopt,
                  const std::optional<int> channelId = std::nullopt)
        : type(t), targetTaskId(taskId), targetProcessorId(processorId),
          targetChannelId(channelId) {}

    // Klonowanie
    [[nodiscard]] std::unique_ptr<Node> clone() const;

    void process(Phenotype& currentState);
};
