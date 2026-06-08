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

    std::vector<std::shared_ptr<Node>> children;
};

class ChangeTaskProcessorRandomNode : public Node {
public:
    ChangeTaskProcessorRandomNode(int taskId, int newPhenotypeProcId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& pheno) override;

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

class MoveTaskToFastestHCNode : public Node {
public:
    explicit MoveTaskToFastestHCNode(int taskId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& pheno) override;

private:
    int taskId;
};

class MoveTaskToCheapestPPNode : public Node {
public:
    explicit MoveTaskToCheapestPPNode(int taskId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& pheno) override;

private:
    int taskId;
};

class MoveTaskToCheapestHCNode : public Node {
public:
    explicit MoveTaskToCheapestHCNode(int taskId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& pheno) override;

private:
    int taskId;
};

class MoveTaskToLeastBusyPP : public Node {
public:
    explicit MoveTaskToLeastBusyPP(int taskId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& pheno) override;

private:
    int taskId;
};

class ChangeChannelRandomNode : public Node {
public:
    explicit ChangeChannelRandomNode(int taskId, int newChannelId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& pheno) override;

private:
    int taskId;
    int newChannelId;
};

class MoveProcToBestBandwidthChannelNode : public Node {
public:
    explicit MoveProcToBestBandwidthChannelNode(int phProcId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& pheno) override;

private:
    int phProcId;
};

class MoveProcToCheapestChannelNode : public Node {
public:
    explicit MoveProcToCheapestChannelNode(int phProcId);

    [[nodiscard]] std::unique_ptr<Node> clone() const override;
    void process(Phenotype& pheno) override;

private:
    int phProcId;
};
