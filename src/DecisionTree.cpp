#include "DecisionTree.h"

DecisionTree::DecisionTree(std::unique_ptr<Node>&& rootNode) : root(std::move(rootNode)) {}

DecisionTree::DecisionTree(const DecisionTree& from) {
    this->root = std::make_unique<Node>(*from.root);
}

DecisionTree& DecisionTree::operator=(const DecisionTree& from) {
    if (this != &from) {
        this->root = std::make_unique<Node>(*from.root);
    }

    return *this;
}

Phenotype DecisionTree::decode(const Phenotype& baseSolution) const {
    Phenotype solution = baseSolution;
    root->process(solution);
    return solution;
}
