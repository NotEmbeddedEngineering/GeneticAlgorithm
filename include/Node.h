#pragma once

#include "Phenotype.h"
#include <memory>
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

    virtual std::unique_ptr<Node> clone() const;
    virtual void process(Phenotype& currentState);

    std::vector<std::unique_ptr<Node>> children;
};

class ChangeProcessorRandomNode : public Node {
public:
    ChangeProcessorRandomNode(int taskId, int newProcId);
    ChangeProcessorRandomNode(const ChangeProcessorRandomNode& from);
    ChangeProcessorRandomNode& operator=(const ChangeProcessorRandomNode& from);
    ChangeProcessorRandomNode(ChangeProcessorRandomNode&& from) = default;
    ChangeProcessorRandomNode& operator=(ChangeProcessorRandomNode&& from) = default;

    std::unique_ptr<Node> clone() const override;
    void process(Phenotype& currentState) override;

private:
    int taskId;
    int newProcId;
};
