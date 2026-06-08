#include "PopulationGenerator.hpp"

#include <algorithm>
#include <execution>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <queue>
#include <thread>
#include <valarray>
#include <vector>

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

    std::unique_ptr<Node> node;

    auto randomPhProcId = [&](size_t taskId) {
        size_t phProcessorId;
        size_t tgProcessorId;

        // TODO: handle channels
        // Regenerate if invalid
        do {
            taskId = taskDist(rng);
            phProcessorId = phProcDist(rng);
            tgProcessorId = currentSolution.getTgProcId(phProcessorId);
        } while (graph->getTime(tgProcessorId, taskId) == -1);

        return phProcessorId;
    };

    switch (randomFunctionType()) {
        case FunctionType::CHANGE_TASK_PROCESSOR_RANDOM: {
            size_t phProcessorId = randomPhProcId(taskId);
            node = std::make_unique<ChangeTaskProcessorRandomNode>(taskId, phProcessorId);
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
        case FunctionType::MOVE_TASK_TO_CHEAPEST_PP: {
            node = std::make_unique<MoveTaskToCheapestPPNode>(taskId);
            break;
        }
        case FunctionType::MOVE_TASK_TO_CHEAPEST_HC: {
            node = std::make_unique<MoveTaskToCheapestHCNode>(taskId);
            break;
        }
        case FunctionType::MOVE_TASK_TO_LEAST_BUSY_PP: {
            node = std::make_unique<MoveTaskToLeastBusyPP>(taskId);
            break;
        }
        case FunctionType::CHANGE_CHANNEL_RANDOM: {
            // TODO: handle edge cases when the processor can't be connected to the new channel
            size_t channelId = channelDist(rng);
            node = std::make_unique<ChangeChannelRandomNode>(taskId, channelId);
            break;
        }
        case FunctionType::MOVE_PROCESSOR_TO_BEST_BANDWIDTH_CHANNEL: {
            size_t phProcessorId = randomPhProcId(taskId);
            node = std::make_unique<MoveProcToBestBandwidthChannelNode>(phProcessorId);
            break;
        }
        case FunctionType::MOVE_PROCESSOR_TO_CHEAPEST_CHANNEL: {
            size_t phProcessorId = randomPhProcId(taskId);
            node = std::make_unique<MoveProcToCheapestChannelNode>(phProcessorId);
            break;
        }

        case FunctionType::NO_OPERATION:
        case FunctionType::COUNT:
            throw std::runtime_error("PopulationGenerator: Wylosowano niedozwolony typ operacji");
    }

    return node;
}

void PopulationGenerator::expandTree(Node* currentNode, const int remainingDepth) {
    if (remainingDepth <= 0)
        return;

    for (int i = 0; i < params.numberOfChildren; ++i) {
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
PopulationGenerator::generateNextPopulation(std::vector<EvaluatedTree>&& prevPopulation) {

    std::vector<DecisionTree> best_specimen;
    best_specimen.reserve(params.populationSize);

    std::ranges::sort(prevPopulation, [](const EvaluatedTree& a, const EvaluatedTree& b) {
        return a.phenotype.getFitnessScore() < b.phenotype.getFitnessScore(); // best first
    });

    std::vector<DecisionTree> clones = selection(prevPopulation, params.numClones);

    // 1. Klonowanie
    for (int i = 0; i < params.numClones; ++i) {
        best_specimen.push_back(std::move(clones[i]));
    }

    std::vector<DecisionTree> mutants = selection(prevPopulation, params.numMutations);

    // 2. Mutacja
    for (int i = 0; i < params.numMutations; ++i) {
        DecisionTree& mutant = mutants[i];
        mutate(mutant);
        best_specimen.push_back(std::move(mutant));
    }

    // 3. Krzyżowanie
    std::vector parents = selection(prevPopulation, params.numCrossovers);
    std::ranges::shuffle(parents, rng);

    for (int i = 0; i < params.numCrossovers - 1; i += 2) {
        DecisionTree& mother = parents[i];
        DecisionTree& father = parents[i + 1];
        crossover(mother, father);

        best_specimen.push_back(std::move(mother));
        best_specimen.push_back(std::move(father));
    }

    return best_specimen;
}

std::vector<EvaluatedTree>
PopulationGenerator::evaluatePopulation(const std::vector<DecisionTree>& population,
                                        const Phenotype& baseSolution) {
    const size_t numElements = population.size();
    if (numElements == 0)
        return {};

    std::vector<std::optional<EvaluatedTree>> tmpEvaluated(numElements);

    const unsigned int numThreads = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::jthread> threads;
    threads.reserve(numThreads);

    size_t chunkSize = (numElements + numThreads - 1) / numThreads;

    for (unsigned int t = 0; t < numThreads; ++t) {
        size_t startIdx = t * chunkSize;
        size_t endIdx = std::min(startIdx + chunkSize, numElements);

        if (startIdx >= numElements)
            break;

        threads.emplace_back([startIdx, endIdx, &population, &baseSolution, &tmpEvaluated]() {
            for (size_t i = startIdx; i < endIdx; ++i) {
                Phenotype candidate = population[i].decode(baseSolution);
                candidate.evaluate();
                tmpEvaluated[i].emplace(population[i], candidate);
            }
        });
    }

    threads.clear();

    std::vector<EvaluatedTree> evaluatedPopulation;
    evaluatedPopulation.reserve(numElements);
    for (auto& opt : tmpEvaluated) {
        evaluatedPopulation.push_back(std::move(*opt));
    }

    return evaluatedPopulation;
}

Phenotype PopulationGenerator::run() {
    auto population = generatePopulationZero();
    int noImprovementCounter = 0;

    Phenotype bestPhenotype = currentSolution;
    double bestFitness = std::numeric_limits<double>::max();

    for (int gen = 0; gen < params.maxGenerations; ++gen) {
        auto evaluated = evaluatePopulation(population, currentSolution);

        const auto currentBestIterator = std::ranges::min_element(
            evaluated, {}, [](const auto& x) { return x.phenotype.getFitnessScore(); });

        if (const double currentBest = currentBestIterator->phenotype.getFitnessScore();
            currentBest < bestFitness) {
            // 1000      1200
            bestFitness = currentBest;
            bestPhenotype = std::move(currentBestIterator->phenotype);
            noImprovementCounter = 0;
        } else {
            ++noImprovementCounter;
        }

        std::cout
            << "Generacja "
            << gen
            << " - fitnessScore: "
            << bestPhenotype.getFitnessScore()
            << std::endl;

        if (noImprovementCounter >= params.epsilon) {
            std::cout
                << "Brak poprawy przez "
                << noImprovementCounter
                << " generacji. Koniec."
                << std::endl;
            break;
        }

        population = generateNextPopulation(std::move(evaluated));
    }

    return bestPhenotype;
}

std::vector<DecisionTree> PopulationGenerator::selection(const std::vector<EvaluatedTree>& pop,
                                                         int populationSize) {

    const size_t N = pop.size();
    const size_t n = populationSize;
    if (N == 0 || n == 0)
        return {};

    const double selectionPressure = 1.75;

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

void PopulationGenerator::dfs(std::shared_ptr<Node>& parentRoot,
                              std::vector<std::shared_ptr<Node>*>& node) {
    if (!parentRoot)
        return;
    node.push_back(&parentRoot);
    for (auto& child : parentRoot->children)
        dfs(child, node);
}

void PopulationGenerator::crossover(DecisionTree& parentA, DecisionTree& parentB) {
    std::vector<std::shared_ptr<Node>*> nodesA;
    std::vector<std::shared_ptr<Node>*> nodesB;
    dfs(parentA.root, nodesA);
    dfs(parentB.root, nodesB);

    if (!nodesA.empty() || !nodesB.empty())
        return;
    std::uniform_int_distribution<int> distA(0, nodesA.size() - 1);
    std::uniform_int_distribution<int> distB(0, nodesB.size() - 1);

    std::shared_ptr<Node>* targetA = nodesA[distA(rng)];
    std::shared_ptr<Node>* targetB = nodesB[distB(rng)];
    std::swap(*targetA, *targetB);
}

void PopulationGenerator::mutate(DecisionTree& tree) {
    std::vector<std::shared_ptr<Node>*> node;
    dfs(tree.root, node);
    if (node.empty())
        return;
    std::uniform_int_distribution<int> distNode(0, node.size() - 1);
    int target = distNode(rng);
    std::shared_ptr<Node>* targetNode = node[target];
    auto newNode = createRandomNode();
    newNode->children = (*targetNode)->children;
    *targetNode = std::move(newNode);
}
