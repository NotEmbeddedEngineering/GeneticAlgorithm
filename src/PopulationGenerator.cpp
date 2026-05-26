#include "PopulationGenerator.h"

#include <algorithm>

PopulationGenerator::PopulationGenerator(const TaskGraph& graph, const int numberOfChilds,
                                         EvolutionParams& params)
    : graph(graph), params(params), numberOfChilds(numberOfChilds), rng(std::random_device{}()) {}

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

std::vector<DecisionTree> PopulationGenerator::generatePopulationZero(int populationSize) {
    std::vector<DecisionTree> population;
    population.reserve(populationSize);

    std::generate_n(std::back_inserter(population), populationSize,
                    [this]() { return buildSingleTree(params.maxTreeDepth); });

    return population;
}

std::vector<DecisionTree> PopulationGenerator::generateNextPopulation(
    const std::vector<EvaluatedTree>& prevPopulation, int populationSize) {

    std::vector<DecisionTree> population;
    population.reserve(populationSize);

    // 1. Klonowanie
    // 2. Krzyżowanie
    // 3. Mutacja
    // TODO
}


std::vector<EvaluatedTree> PopulationGenerator::evaluatePopulation(const std::vector<DecisionTree>& population, const Phenotype& baseSolition) {
    std::vector<EvaluatedTree> evaluatedPopulation;
    evaluatedPopulation.reserve(population.size());

    for (const auto& tree : population) {
        Phenotype candidate = tree.decode(baseSolition);
        candidate.evaluate();
        evaluatedPopulation.push_back({tree, candidate});
    }

    return evaluatedPopulation;
}

Phenotype PopulationGenerator::run(const Phenotype& initialSolution) {
    auto population = generatePopulationZero(static_cast<int>(params.populationSize));
    int noImprovementCounter = 0;
    double bestFitness = -1e9;

    for (int gen = 0; gen < params.maxGenerations; ++gen) {
        auto evaluated = evaluatePopulation(population, initialSolution);
        double currentBest; // TODO
    }
}