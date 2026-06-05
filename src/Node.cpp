#include "Node.hpp"

#include <algorithm>

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
ChangeProcessorRandomNode::ChangeProcessorRandomNode(const int taskId, const int newPhenotypeProcId)
    : taskId(taskId), newPhenotypeProcId(newPhenotypeProcId) {}

std::unique_ptr<Node> ChangeProcessorRandomNode::clone() const {
    return std::make_unique<ChangeProcessorRandomNode>(*this);
}

void ChangeProcessorRandomNode::process(Phenotype& currentState) {
    currentState.changeTaskProc(taskId, newPhenotypeProcId);
    Node::process(currentState);
}

// --- MoveTaskToFastestProcessor ---
MoveTaskToFastestProcessorNode::MoveTaskToFastestProcessorNode(const int taskId,
                                                               std::mt19937_64& rng)
    : taskId(taskId), rng(rng) {}

std::unique_ptr<Node> MoveTaskToFastestProcessorNode::clone() const {
    return std::make_unique<MoveTaskToFastestProcessorNode>(*this);
}

// TODO Split to differantiate between using fastest PP and buying new HC
void MoveTaskToFastestProcessorNode::process(Phenotype& currentState) {
    // TODO fix to use phenotype internal proc
    // auto graph = currentState.getGraph();

    // std::vector<size_t> fastestProcIds;
    // int fastestProcTime = std::numeric_limits<int>::max();

    // for (size_t procId = 0; procId < graph->getProcessorsCount(); procId++) {
    //     int time = graph->getTime(procId, taskId);

    //     if (time < fastestProcTime) {
    //         fastestProcTime = time;
    //         fastestProcIds.clear();
    //         fastestProcIds.push_back(procId);
    //     } else if (time == fastestProcTime) {
    //         fastestProcIds.push_back(procId);
    //     }
    // }

    // std::uniform_int_distribution<int> dist(0, fastestProcIds.size() - 1);
    // const int randomFastestProcIdx = dist(rng);
    // currentState.taskToProcessor[taskId] = fastestProcIds[randomFastestProcIdx];

    Node::process(currentState);
}
