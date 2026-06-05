#include "Node.hpp"

#include <algorithm>
#include <ranges>

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

// --- ChangeProcessorRandom ---
ChangeProcessorRandomNode::ChangeProcessorRandomNode(const int taskId, const int newPhenotypeProcId)
    : taskId(taskId), newPhenotypeProcId(newPhenotypeProcId) {}

std::unique_ptr<Node> ChangeProcessorRandomNode::clone() const {
    return std::make_unique<ChangeProcessorRandomNode>(*this);
}

void ChangeProcessorRandomNode::process(Phenotype& pheno) {
    pheno.changeTaskProc(taskId, newPhenotypeProcId);
    Node::process(pheno);
}

// --- MoveTaskToFastestPP ---
MoveTaskToFastestPPNode::MoveTaskToFastestPPNode(const int taskId) : taskId(taskId) {}

std::unique_ptr<Node> MoveTaskToFastestPPNode::clone() const {
    return std::make_unique<MoveTaskToFastestPPNode>(*this);
}

void MoveTaskToFastestPPNode::process(Phenotype& pheno) {
    const auto& graph = pheno.getGraph();

    // Don't buy new PP, assign task to fastest existing PP
    auto ppProcIt = std::views::iota(0uz, pheno.getPhenotypeProcCount())
                    | std::views::filter([&](size_t phProcId) {
                          return graph->getProc(pheno.getTgProcId(phProcId)).isPP();
                      });

    const auto bestPhProcId = *std::ranges::min_element(ppProcIt, {}, [&](size_t phProcId) {
        const size_t tgProcId = pheno.getTgProcId(phProcId);
        const int32_t procTime = graph->getTime(tgProcId, taskId);
        const int32_t procUsage = pheno.getPhenotypeProcUsage(phProcId);

        // Sortowanie leksykograficzne bjacz
        return std::make_pair(procTime, procUsage);
    });

    pheno.changeTaskProc(taskId, bestPhProcId);

    Node::process(pheno);
}

// --- MoveTaskToFastestHC ---
MoveTaskToFastestHCNode::MoveTaskToFastestHCNode(int taskId) : taskId(taskId) {}

std::unique_ptr<Node> MoveTaskToFastestHCNode::clone() const {
    return std::make_unique<MoveTaskToFastestHCNode>(*this);
}

void MoveTaskToFastestHCNode::process(Phenotype& pheno) {
    const auto& graph = pheno.getGraph();

    // We have to buy new HC
    auto hcProcIt =
        std::views::iota(0uz, graph->getProcessorsCount())
        | std::views::filter([&](size_t tgProcId) { return graph->getProc(tgProcId).isHC(); });

    const auto bestHcProcId = *std::ranges::min_element(
        hcProcIt, {}, [&](size_t tgProcId) { return graph->getTime(tgProcId, taskId); });

    pheno.addProc(bestHcProcId);

    Node::process(pheno);
}
