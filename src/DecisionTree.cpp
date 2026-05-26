#include "DecisionTree.h"

DecisionTree::DecisionTree(std::unique_ptr<Node> rootNode) : root(std::move(rootNode)) {}

// Copy constructor
DecisionTree::DecisionTree(const DecisionTree& other) {
    if (other.root) {
        root = other.root->clone();
    }
}

DecisionTree& DecisionTree::operator=(const DecisionTree& other) {
    if (this != &other && other.root) {
        root = other.root->clone();
    }

    return *this;
}

Phenotype DecisionTree::decode(const Phenotype& baseSolution) const {
    // TODO
}
