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

// --- ChangeTaskProcessorRandom ---
ChangeTaskProcessorRandomNode::ChangeTaskProcessorRandomNode(const int taskId,
                                                             const int newPhenotypeProcId)
    : taskId(taskId), newPhenotypeProcId(newPhenotypeProcId) {}

std::unique_ptr<Node> ChangeTaskProcessorRandomNode::clone() const {
    return std::make_unique<ChangeTaskProcessorRandomNode>(*this);
}

void ChangeTaskProcessorRandomNode::process(Phenotype& pheno) {
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
    auto ppProcView = std::views::iota(0uz, pheno.getPhenotypeProcCount())
                      | std::views::filter([&](const size_t phProcId) {
                            return graph->getProc(pheno.getTgProcId(phProcId)).isPP();
                        });

    size_t fastestPhProcId;
    if (std::ranges::empty(ppProcView)) {
        // If no PPs bought, then buy the best one
        auto graphPpProcView = std::views::iota(0uz, graph->getProcessorsCount())
                               | std::views::filter([&](const size_t tgProcId) {
                                     return graph->getProc(tgProcId).isPP();
                                 });
        size_t bestToBuyId =
            *std::ranges::min_element(graphPpProcView, {}, [&](const size_t tgProcId) {
                const int32_t procTime = graph->getTime(tgProcId, taskId);
                const int32_t procCost = graph->getProc(tgProcId).cost;
                return std::pair(procTime, procCost);
            });
        fastestPhProcId = pheno.addProc(bestToBuyId);
    } else {
        fastestPhProcId = *std::ranges::min_element(ppProcView, {}, [&](const size_t phProcId) {
            const size_t tgProcId = pheno.getTgProcId(phProcId);
            const int32_t procTime = graph->getTime(tgProcId, taskId);
            const int32_t procUsage = pheno.getPhenotypeProcUsage(phProcId);

            // Sortowanie leksykograficzne bjacz
            return std::pair(procTime, procUsage);
        });
    }

    pheno.changeTaskProc(taskId, fastestPhProcId);
    Node::process(pheno);
}

// --- MoveTaskToFastestHC ---
MoveTaskToFastestHCNode::MoveTaskToFastestHCNode(const int taskId) : taskId(taskId) {}

std::unique_ptr<Node> MoveTaskToFastestHCNode::clone() const {
    return std::make_unique<MoveTaskToFastestHCNode>(*this);
}

void MoveTaskToFastestHCNode::process(Phenotype& pheno) {
    const auto& graph = pheno.getGraph();

    // We have to buy new HC
    auto hcProcView = std::views::iota(0uz, graph->getProcessorsCount())
                      | std::views::filter(
                          [&](const size_t tgProcId) { return graph->getProc(tgProcId).isHC(); });

    const size_t fastestHcId = *std::ranges::min_element(
        hcProcView, {}, [&](const size_t tgProcId) { return graph->getTime(tgProcId, taskId); });

    size_t newHcId = pheno.addProc(fastestHcId);
    pheno.changeTaskProc(taskId, newHcId);

    Node::process(pheno);
}

// --- MoveTaskToCheapestPP ---
MoveTaskToCheapestPPNode::MoveTaskToCheapestPPNode(const int taskId) : taskId(taskId) {}

std::unique_ptr<Node> MoveTaskToCheapestPPNode::clone() const {
    return std::make_unique<MoveTaskToCheapestPPNode>(*this);
}

void MoveTaskToCheapestPPNode::process(Phenotype& pheno) {
    const auto& graph = pheno.getGraph();

    // Don't buy new PP, assign task to cheapest existing PP
    auto ppProcView = std::views::iota(0uz, pheno.getPhenotypeProcCount())
                      | std::views::filter([&](const size_t phProcId) {
                            return graph->getProc(pheno.getTgProcId(phProcId)).isPP();
                        });

    size_t cheapestPhProcId;
    if (std::ranges::empty(ppProcView)) {
        // If no PPs bought, then buy the best one
        auto graphPpProcView = std::views::iota(0uz, graph->getProcessorsCount())
                               | std::views::filter([&](const size_t tgProcId) {
                                     return graph->getProc(tgProcId).isPP();
                                 });
        size_t bestToBuyId =
            *std::ranges::min_element(graphPpProcView, {}, [&](const size_t tgProcId) {
                const int32_t procCost = graph->getProc(tgProcId).cost;
                const int32_t procTime = graph->getTime(tgProcId, taskId);
                return std::pair(procCost, procTime);
            });
        cheapestPhProcId = pheno.addProc(bestToBuyId);
    } else {
        cheapestPhProcId = *std::ranges::min_element(ppProcView, {}, [&](const size_t phProcId) {
            const size_t tgProcId = pheno.getTgProcId(phProcId);
            const Processor& proc = graph->getProc(tgProcId);
            const int32_t procUsage = pheno.getPhenotypeProcUsage(phProcId);

            // Sortowanie leksykograficzne bjacz
            return std::pair(proc.cost, procUsage);
        });
    }

    pheno.changeTaskProc(taskId, cheapestPhProcId);

    Node::process(pheno);
}

// --- MoveTaskToCheapestHC ---
MoveTaskToCheapestHCNode::MoveTaskToCheapestHCNode(const int taskId) : taskId(taskId) {}

std::unique_ptr<Node> MoveTaskToCheapestHCNode::clone() const {
    return std::make_unique<MoveTaskToCheapestHCNode>(*this);
}

void MoveTaskToCheapestHCNode::process(Phenotype& pheno) {
    const auto& graph = pheno.getGraph();

    // We have to buy new HC
    auto hcProcView = std::views::iota(0uz, graph->getProcessorsCount())
                      | std::views::filter(
                          [&](const size_t tgProcId) { return graph->getProc(tgProcId).isHC(); });

    const size_t cheapestHcId = *std::ranges::min_element(
        hcProcView, {}, [&](const size_t tgProcId) { return graph->getProc(tgProcId).cost; });

    size_t newHcId = pheno.addProc(cheapestHcId);
    pheno.changeTaskProc(taskId, newHcId);

    Node::process(pheno);
}

// --- MoveTaskToLeastBusyPP ---
MoveTaskToLeastBusyPP::MoveTaskToLeastBusyPP(const int taskId) : taskId(taskId) {}

std::unique_ptr<Node> MoveTaskToLeastBusyPP::clone() const {
    return std::make_unique<MoveTaskToLeastBusyPP>(*this);
}

void MoveTaskToLeastBusyPP::process(Phenotype& pheno) {
    const auto& graph = pheno.getGraph();

    auto ppProcView = std::views::iota(0uz, pheno.getPhenotypeProcCount())
                      | std::views::filter([&](const size_t phProcId) {
                            return graph->getProc(pheno.getTgProcId(phProcId)).isPP();
                        });

    // If no PPs bought, don't do anything
    if (!std::ranges::empty(ppProcView)) {
        const size_t leastUsedPhProcId =
            *std::ranges::min_element(ppProcView, {}, [&](const size_t phProcId) {
                const size_t tgProcId = pheno.getTgProcId(phProcId);
                const Processor& proc = graph->getProc(tgProcId);
                const int32_t procUsage = pheno.getPhenotypeProcUsage(phProcId);

                // Sortowanie leksykograficzne bjacz
                return std::pair(procUsage, proc.cost);
            });

        pheno.changeTaskProc(taskId, leastUsedPhProcId);
    }

    Node::process(pheno);
}

// --- ChangeChannelRandom ---
ChangeChannelRandomNode::ChangeChannelRandomNode(int taskId, int newChannelId)
    : taskId(taskId), newChannelId(newChannelId) {}

std::unique_ptr<Node> ChangeChannelRandomNode::clone() const {
    return std::make_unique<ChangeChannelRandomNode>(*this);
}

void ChangeChannelRandomNode::process(Phenotype& pheno) {
    pheno.changeTaskChannel(taskId, newChannelId);
    Node::process(pheno);
}
