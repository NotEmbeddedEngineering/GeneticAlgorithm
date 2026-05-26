#pragma once

#include "Node.h"
#include "Phenotype.h"

class DecisionTree {
public:
    DecisionTree() = delete;

    explicit DecisionTree(std::unique_ptr<Node>&& rootNode);
    DecisionTree(const DecisionTree& from);
    DecisionTree& operator=(const DecisionTree& from);

    DecisionTree(DecisionTree&&) noexcept = default;
    DecisionTree& operator=(DecisionTree&&) noexcept = default;

    // przetwarza i wykonuje funkcje wygenerowane wcześniej drzewo w PopulationGenerator
    [[nodiscard]] Phenotype decode(const Phenotype& baseSolution) const;

private:
    std::unique_ptr<Node> root;
};

struct EvaluatedTree {
    DecisionTree tree;
    Phenotype phenotype;
};
