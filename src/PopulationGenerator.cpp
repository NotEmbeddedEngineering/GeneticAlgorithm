#include "PopulationGenerator.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <ostream>
#include <queue>
#include <valarray>

PopulationGenerator::PopulationGenerator(const std::shared_ptr<TaskGraph> graph,
                                         Phenotype& initialSolution, const EvolutionParams& params)
    : graph(graph), currentSolution(initialSolution), params(params), rng(std::random_device{}()) {}

FunctionType PopulationGenerator::randomFunctionType() {
    std::uniform_int_distribution<int> dist(1, static_cast<int>(FunctionType::COUNT) - 1);

    return static_cast<FunctionType>(dist(rng));
}

std::unique_ptr<Node> PopulationGenerator::createRandomNode() {
    std::uniform_int_distribution<size_t> taskDist(0, graph->getTaskCount() - 1);
    std::uniform_int_distribution<size_t> phProcDist(0,
                                                     currentSolution.getPhenotypeProcCount() - 1);
    std::uniform_int_distribution<size_t> channelDist(0, graph->getChannelsCount() - 1);

    size_t taskId = taskDist(rng);
    size_t phProcessorId = phProcDist(rng);
    size_t tgProcessorId = currentSolution.getTgProcId(phProcessorId);
    size_t channelId = channelDist(rng);

    // Regenerate if invalid
    while (graph->getTime(tgProcessorId, taskId) == -1) {
        taskId = taskDist(rng);
        phProcessorId = phProcDist(rng);
        tgProcessorId = currentSolution.getTgProcId(phProcessorId);
    }
    while (!graph->isConnected(channelId, tgProcessorId)) {
        channelId = channelDist(rng);
    }

    std::unique_ptr<Node> node;

    switch (randomFunctionType()) {
        case FunctionType::CHANGE_PROCESSOR_RANDOM: {
            node = std::make_unique<ChangeProcessorRandomNode>(taskId, phProcessorId);
            break;
        }
        case FunctionType::MOVE_TASK_TO_FASTEST_PP: {
            node = std::make_unique<MoveTaskToFastestPPNode>(taskId);
            break;
        }
        case FunctionType::MOVE_TASK_TO_FASTEST_HC: {
            node = std::make_unique<MoveTaskToFastestHCNode>(taskId);
            break;
        }
        case FunctionType::MOVE_TASK_TO_CHEAPEST_PROCESSOR:
        case FunctionType::MOVE_TASK_TO_LEAST_BUSY_PROCESSOR:
        case FunctionType::CHANGE_CHANNEL_RANDOM:
        case FunctionType::MOVE_PROCESSOR_TO_BEST_BANDWIDTH_CHANNEL:
        case FunctionType::MOVE_PROCESSOR_TO_CHEAPEST_CHANNEL:
            // TODO: Node
            node = std::make_unique<Node>();
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

    std::binomial_distribution<int> numChilds(params.numberOfChildren, p_d);

    const int range = numChilds(rng);
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

Phenotype PopulationGenerator::run() {
    auto population = generatePopulationZero();
    int noImprovementCounter = 0;

    Phenotype bestPhenotype = currentSolution;
    double bestFitness = -1e9;

    for (int gen = 0; gen < params.maxGenerations; ++gen) {
        auto evaluated = evaluatePopulation(population, currentSolution);

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
            std::cout
                << "Brak poprawy przez "
                << noImprovementCounter
                << " generacji. Koniec."
                << std::endl;
            break;
        }

        population = generateNextPopulation(evaluated);
    }

    return bestPhenotype;
}

std::vector<DecisionTree>
PopulationGenerator::selection(const std::vector<EvaluatedTree>& population, int populationSize) {

    const size_t N = population.size();
    const size_t n = populationSize;
    if (N == 0 || n == 0)
        return {};

    const double selectionPressure = 1.75;

    std::vector<EvaluatedTree> pop = population;
    std::ranges::sort(pop, [](const EvaluatedTree& a, const EvaluatedTree& b) {
        return a.phenotype.getFitnessScore() > b.phenotype.getFitnessScore(); // best first
    });

    std::vector<double> weights(N);
    double sum{};
    if (N == 1)
        weights[0] = 1.0;
    else
        for (size_t rank{}; rank < N; ++rank) {
            weights[rank] = selectionPressure
                            - 2
                            * (selectionPressure - 1)
                            * static_cast<double>(rank)
                            / (static_cast<double>(N) - 1);
            if (weights[rank] < 0)
                weights[rank] = 0;
            sum += weights[rank];
        }
    if (sum != 0)
        for (auto& weight : weights)
            weight /= sum;

    std::vector<double> cdf(N);
    cdf[0] = weights[0];
    for (size_t i{1}; i < N; ++i)
        cdf[i] = cdf[i - 1] + weights[i];
    cdf.back() = 1.0;

    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::vector<DecisionTree> result;
    result.reserve(n);
    for (size_t _{}; _ < n; ++_) {
        double rVal = dist(rng);
        size_t id = std::distance(cdf.begin(), std::ranges::upper_bound(cdf, rVal));
        if (id >= N)
            id = N - 1;
        result.push_back(pop[id].tree);
    }
    return result;
}

void PopulationGenerator::crossover(DecisionTree& parentA, DecisionTree& parentB) {}
void PopulationGenerator::mutate(DecisionTree& tree) {}
