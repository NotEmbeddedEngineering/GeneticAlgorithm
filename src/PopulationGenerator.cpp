#include "PopulationGenerator.h"

#include <algorithm>

PopulationGenerator::PopulationGenerator(const TaskGraph& graph, const int numberOfChilds,
                                         const EvolutionParams& params)
    : graph(graph), params(params), numberOfChilds(numberOfChilds), rng(std::random_device{}()) {}

FunctionType PopulationGenerator::randomFunctionType() {
    std::uniform_int_distribution<int> dist(1, static_cast<int>(FunctionType::COUNT) - 1);

    return static_cast<FunctionType>(dist(rng));
}

std::unique_ptr<Node> PopulationGenerator::createRandomNode() {
    std::uniform_int_distribution<int> taskDist(0, graph.numTasks - 1);
    std::uniform_int_distribution<int> procDist(0, graph.numProcessors - 1);
    std::uniform_int_distribution<int> channelDist(0, graph.numChannels - 1);

    int taskId = taskDist(rng);
    int processorId = procDist(rng);
    int channelId = channelDist(rng);

    // Regenerate if invalid
    while (graph.times[processorId][taskId] == -1) {
        taskId = taskDist(rng);
        processorId = procDist(rng);
    }
    while (graph.channels[channelId].connected_processor[processorId] == 0) {
        channelId = channelDist(rng);
    }

    std::unique_ptr<Node> node;

    switch (randomFunctionType()) {
        case FunctionType::CHANGE_PROCESSOR_RANDOM: {
            node = std::make_unique<ChangeProcessorRandomNode>(taskId, processorId);
            break;
        }
        case FunctionType::MOVE_TASK_TO_FASTEST_PROCESSOR:
            break;
        case FunctionType::MOVE_TASK_TO_CHEAPEST_PROCESSOR:
            break;
        case FunctionType::MOVE_TASK_TO_LEAST_BUSY_PROCESSOR:
            break;
        case FunctionType::CHANGE_CHANNEL_RANDOM:
            break;
        case FunctionType::MOVE_PROCESSOR_TO_BEST_BANDWIDTH_CHANNEL:
            break;
        case FunctionType::MOVE_PROCESSOR_TO_CHEAPEST_CHANNEL:
            break;

        case FunctionType::NO_OPERATION:
        case FunctionType::COUNT:
            throw std::runtime_error("PopulationGenerator: Wylosowano niedozwolony typ operacji");
    }

    return node;
}

void PopulationGenerator::expandTree(Node* currentNode, const int remainingDepth) {
    if (remainingDepth <= 0)
        return;

    std::uniform_int_distribution<int> numChilds(0, numberOfChilds);

    for (int i = 0; i < numChilds(rng); ++i) {
        auto child = createRandomNode();
        expandTree(child.get(), remainingDepth - 1);
        currentNode->children.push_back(std::move(child));
    }
}

DecisionTree PopulationGenerator::buildSingleTree(const int maxDepth) {
    auto root = std::make_unique<Node>();
    expandTree(root.get(), maxDepth);

    return DecisionTree(std::move(root));
}

std::vector<DecisionTree> PopulationGenerator::generatePopulationZero() {
    std::vector<DecisionTree> population;
    population.reserve(params.populationSize);

    std::generate_n(std::back_inserter(population), params.populationSize,
                    [this]() { return buildSingleTree(params.maxTreeDepth); });

    return population;
}

std::vector<DecisionTree>
PopulationGenerator::generateNextPopulation(const std::vector<EvaluatedTree>& prevPopulation) {

    std::vector<DecisionTree> best_specimen;
    best_specimen.reserve(params.populationSize);

    const int numOfParents =
        std::max({params.numCrossovers, params.numMutations, params.numClones});
    std::vector<DecisionTree> parents = selectParents(prevPopulation, numOfParents);

    // 1. Klonowanie
    for (int i = 0; i < params.numClones; ++i) {
        best_specimen.push_back(parents[i]);
    }

    // 2. Mutacja
    for (int i = 0; i < params.numMutations; ++i) {
        DecisionTree mutant = parents[i];
        mutate(mutant);
        best_specimen.push_back(mutant);
    }

    // 3. Krzyżowanie
    std::vector parentsToCross(parents.begin(), parents.begin() + params.numCrossovers);
    std::ranges::shuffle(parentsToCross, rng);

    for (int i = 0; i < params.numCrossovers - 1; i += 2) {
        DecisionTree& mother = parentsToCross[i];
        DecisionTree& father = parentsToCross[i + 1];
        crossover(mother, father);

        best_specimen.push_back(mother);
        best_specimen.push_back(father);
    }

    return best_specimen;
}

std::vector<EvaluatedTree>
PopulationGenerator::evaluatePopulation(const std::vector<DecisionTree>& population,
                                        const Phenotype& baseSolition) {
    std::vector<EvaluatedTree> evaluatedPopulation;
    evaluatedPopulation.reserve(population.size());

    for (int i = 0; i < population.size(); ++i) {
        auto tree = population[i];
        Phenotype candidate = tree.decode(baseSolition);
        candidate.evaluate();
        evaluatedPopulation.emplace_back(tree, candidate);
    }

    return evaluatedPopulation;
}

Phenotype PopulationGenerator::run(const Phenotype& initialSolution) {
    auto population = generatePopulationZero();
    int noImprovementCounter = 0;
    double bestFitness = -1e9;

    for (int gen = 0; gen < params.maxGenerations; ++gen) {
        auto evaluated = evaluatePopulation(population, initialSolution);
        double currentBest; // TODO
    }
}
