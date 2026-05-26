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

    virtual void process(const Phenotype& currentState);

    std::vector<std::unique_ptr<Node>> children;
};

class ChangeProcessorRandomNode : public Node {
public:
    ChangeProcessorRandomNode();
    ChangeProcessorRandomNode(const ChangeProcessorRandomNode& from);

    void process(const Phenotype& currentState) override;

    int taskId = -1;
    int newProcId = -1;
};
