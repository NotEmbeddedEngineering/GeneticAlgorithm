#pragma once

#include "Phenotype.h"
#include "Node.h"

#include <memory>

class DecisionTree {
private:
    std::unique_ptr<Node> root;

public:
    DecisionTree() = delete;

    explicit DecisionTree(std::unique_ptr<Node> rootNode);
    DecisionTree(const DecisionTree& other);
    DecisionTree& operator=(const DecisionTree& other);

    DecisionTree(DecisionTree&&) noexcept = default;
    DecisionTree& operator=(DecisionTree&&) noexcept = default;

    // przetwarza i wykonuje funkcje wygenerowane wcześniej drzewo w PopulationGenerator
    [[nodiscard]] Phenotype decode(const Phenotype& baseSolution) const;
};
