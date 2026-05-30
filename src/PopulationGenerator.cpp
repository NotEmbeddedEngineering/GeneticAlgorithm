#include "PopulationGenerator.h"

#include <algorithm>
#include <iostream>
#include <ostream>

PopulationGenerator::PopulationGenerator(const TaskGraph& graph,
                                         const EvolutionParams& params)
    : graph(graph), params(params), rng(std::random_device{}()) {}

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
    while (graph.getTime(processorId,taskId)== -1) {
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

    const int depth = params.maxTreeDepth - remainingDepth;
    const double p_d = 1.0 / (static_cast<double>(depth) + 1.0);

    std::binomial_distribution<int> numChilds(params.numberOfChilds, p_d);

    int range = numChilds(rng);
    for (int i = 0; i < range; ++i) {
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

    const std::vector<DecisionTree> clones = selection(prevPopulation, params.numClones);

    // 1. Klonowanie
    for (int i = 0; i < params.numClones; ++i) {
        best_specimen.push_back(clones[i]);
    }

    const std::vector<DecisionTree> mutants = selection(prevPopulation, params.numMutations);

    // 2. Mutacja
    for (int i = 0; i < params.numMutations; ++i) {
        DecisionTree mutant = mutants[i];
        mutate(mutant);
        best_specimen.push_back(mutant);
    }

    // 3. Krzyżowanie
    std::vector parents = selection(prevPopulation, params.numCrossovers);
    std::ranges::shuffle(parents, rng);

    for (int i = 0; i < params.numCrossovers - 1; i += 2) {
        DecisionTree& mother = parents[i];
        DecisionTree& father = parents[i + 1];
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

    for (const auto& tree : population) {
        Phenotype candidate = tree.decode(baseSolition);
        candidate.evaluate();
        evaluatedPopulation.emplace_back(tree, candidate);
    }

    return evaluatedPopulation;
}

Phenotype PopulationGenerator::run(const Phenotype& initialSolution) {
    auto population = generatePopulationZero();
    int noImprovementCounter = 0;

    Phenotype bestPhenotype = initialSolution;
    double bestFitness = -1e9;

    for (int gen = 0; gen < params.maxGenerations; ++gen) {
        auto evaluated = evaluatePopulation(population, initialSolution);

        const auto currentBestIterator = std::ranges::max_element(
            evaluated, {}, [](const auto& x) { return x.phenotype.getFitnessScore(); });

        if (const double currentBest = currentBestIterator->phenotype.getFitnessScore();
            currentBest > bestFitness) {
            bestFitness = currentBest;
            bestPhenotype = currentBestIterator->phenotype;
            noImprovementCounter = 0;
        } else {
            ++noImprovementCounter;
        }

        if (noImprovementCounter >= params.epsilon) {
            std::cout << "Brak poprawy przez " << noImprovementCounter << " generacji. Koniec."
                      << std::endl;
            break;
        }

        population = generateNextPopulation(evaluated);
    }

    return bestPhenotype;
}
