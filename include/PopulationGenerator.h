#pragma once
#include "DecisionTree.h"
#include "EvolutionParams.h"
#include "FunctionType.h"
#include "Node.h"
#include "Phenotype.h"
#include "TaskGraph.h"
#include <random>

class PopulationGenerator {
public:
    explicit PopulationGenerator(const TaskGraph& graph, int numberOfChilds,
                                 EvolutionParams& params);

    // generator Generacji 0
    std::vector<DecisionTree> generatePopulationZero(int populationSize);

    // generator kolejnej Generacji X
    std::vector<DecisionTree>
    generateNextPopulation(const std::vector<EvaluatedTree>& prevPopulation, int populationSize);

    std::vector<EvaluatedTree> evaluatePopulation(const std::vector<DecisionTree>& population, const Phenotype& baseSolution);

    // Tutaj funkcje do wybierania rodzicow - metoda RANKINGOWA
    std::vector<EvaluatedTree> selectParents(const std::vector<EvaluatedTree>& population,
                                            int numParents);

    // bierzemy Drzewo A i Drzewo B, losujemy po jednym wezle i odcinami i zamieniamy miejscami(krzyżowanie)
    void crossover(DecisionTree& parentA, DecisionTree& parentB);

    // losujemy węzeł drzewa i go zmieniamy(mutujemy)
    // np. zmieniamy jego funkcję, ewentualnie jeszcze targetTaskId, targetProcessorId
    void mutate(DecisionTree& tree);

    // odpalenie symulacji
    // TODO
    // shared pointer dodac
    Phenotype run(const Phenotype& initialSolution);

private:
    const TaskGraph& graph;
    const EvolutionParams params;
    const int numberOfChilds;
    std::mt19937_64 rng;

    // wybiera random funkcje
    FunctionType randomFunctionType();

    // tworzy pojedynczego Node'a
    std::unique_ptr<Node> createRandomNode();

    // DFS do budownia galezi
    void expandTree(Node* currentNode, int maxDepth);

    // generator jednego drzewa
    DecisionTree buildSingleTree(int maxDepth);
};
