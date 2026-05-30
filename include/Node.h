#pragma once

#include "Phenotype.h"
#include <memory>
#include <random>
#include <vector>

/**
 * Empty node
 */
class Node {
public:
    Node() = default;
    Node(const Node& from);
    Node& operator=(const Node& from);
    Node(Node&& from) = default;
    Node& operator=(Node&& from) = default;
    virtual ~Node() = default;

    [[nodiscard]] virtual std::unique_ptr<Node> clone() const;
    virtual void process(Phenotype& currentState);

    std::vector<std::unique_ptr<Node>> children;
};

class ChangeProcessorRandomNode : public Node {
public:
    ChangeProcessorRandomNode(int taskId, int newProcId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& currentState) override;

private:
    int taskId;
    int newProcId;
};

class MoveTaskToFastestProcessorNode : public Node {
public:
    explicit MoveTaskToFastestProcessorNode(int taskId, std::mt19937_64& rng);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& currentState) override;

private:
    int taskId;
    std::mt19937_64& rng;
};
