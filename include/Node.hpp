#pragma once

#include "Phenotype.hpp"
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
    ChangeProcessorRandomNode(int taskId, int newPhenotypeProcId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& currentState) override;

private:
    int taskId;
    int newPhenotypeProcId;
};

class MoveTaskToFastestPPNode : public Node {
public:
    explicit MoveTaskToFastestPPNode(int taskId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& pheno) override;

private:
    int taskId;
};
