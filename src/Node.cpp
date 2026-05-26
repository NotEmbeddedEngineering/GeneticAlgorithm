#include "Node.h"

#include <iostream>

// --- NODE ---
Node::Node(const Node& from) {
    for (const auto& child : from.children) {
        this->children.emplace_back(std::make_unique<Node>(*child));
    }
}
Node& Node::operator=(const Node& from) {
    if (&from == this) {
        return *this;
    }

    this->children.clear();
    for (const auto& child : from.children) {
        this->children.emplace_back(std::make_unique<Node>(*child));
    }
    return *this;
}

void Node::process(const Phenotype& currentState) {
    for (const auto& child : children) {
        child->process(currentState);
    }
}

// --- ChangeProcessorRandomNode ---
ChangeProcessorRandomNode::ChangeProcessorRandomNode() : Node() {}
ChangeProcessorRandomNode::ChangeProcessorRandomNode(const ChangeProcessorRandomNode& from)
    : Node(from) {
    taskId = from.taskId;
    newProcId = from.newProcId;
}

void ChangeProcessorRandomNode::process(const Phenotype& currentState) {
    newProcId = 67;
    taskId = 69;
    std::cout << "ChangeProcessorRandomNode " << newProcId << " " << taskId << "\n";

    Node::process(currentState);
}
