#include "PopulationGenerator.h"

PopulationGenerator::PopulationGenerator(const TaskGraph& graph, const int numberOfChilds)
    : graph(graph), numberOfChilds(numberOfChilds), rng(std::random_device{}()) {}

FunctionType PopulationGenerator::randomFunctionType() {
    std::uniform_int_distribution<int> dist(1, static_cast<int>(FunctionType::COUNT));

    return static_cast<FunctionType>(dist(rng));
}

std::unique_ptr<Node> PopulationGenerator::createRandomNode() {
    FunctionType type = randomFunctionType();

    std::uniform_int_distribution<int> taskDist(0, graph.numTasks - 1);
    std::uniform_int_distribution<int> procDist(0, graph.numProcessors - 1);
    std::uniform_int_distribution<int> channelDist(0, graph.numChannels - 1);

    int taskId = taskDist(rng);
    int processorId = procDist(rng);
    int channelId = channelDist(rng);

    if (graph.times[processorId][taskId] == -1) {
        // TODO
    }

    if (graph.channels[channelId].connected_processor[processorId] == 0) {
        // TODO
    }

    return std::make_unique<Node>(type, taskId, processorId, channelId);
}

void PopulationGenerator::expandTree(Node* currentNode, int remainingDepth) {
    if (remainingDepth <= 0)
        return;

    std::uniform_int_distribution<int> numChilds(0, numberOfChilds);

    for (int i = 0; i < numChilds(rng); ++i) {
        auto child = createRandomNode();
        expandTree(child.get(), remainingDepth - 1);
        currentNode->children.push_back(std::move(child));
    }
}

DecisionTree PopulationGenerator::buildSingleTree(int maxDepth) {
    auto root = std::make_unique<Node>(FunctionType::NO_OPERATION);
    expandTree(root.get(), maxDepth);

    return DecisionTree(std::move(root));
}
