#include "Node.h"

#include <iostream>

// --- NODE ---
Node::Node(const Node& from) {
    for (const auto& child : from.children) {
        children.emplace_back(child->clone());
    }
}
Node& Node::operator=(const Node& from) {
    if (&from == this) {
        return *this;
    }

    children.clear();
    for (const auto& child : from.children) {
        children.emplace_back(child->clone());
    }
    return *this;
}

std::unique_ptr<Node> Node::clone() const {
    return std::make_unique<Node>(*this);
}

void Node::process(Phenotype& currentState) {
    for (const auto& child : children) {
        child->process(currentState);
    }
}

// --- ChangeProcessorRandomNode ---
ChangeProcessorRandomNode::ChangeProcessorRandomNode(const int taskId, const int newProcId)
    : Node(), taskId(taskId), newProcId(newProcId) {}

ChangeProcessorRandomNode::ChangeProcessorRandomNode(const ChangeProcessorRandomNode& from)
    : Node(from), taskId(from.taskId), newProcId(from.newProcId) {}

ChangeProcessorRandomNode&
ChangeProcessorRandomNode::operator=(const ChangeProcessorRandomNode& from) {
    if (this == &from) {
        return *this;
    }

    Node::operator=(from);
    taskId = from.taskId;
    newProcId = from.newProcId;

    return *this;
}

std::unique_ptr<Node> ChangeProcessorRandomNode::clone() const {
    return std::make_unique<ChangeProcessorRandomNode>(*this);
}

void ChangeProcessorRandomNode::process(Phenotype& currentState) {
    currentState.taskToProcessor[taskId] = newProcId;
    Node::process(currentState);
}
