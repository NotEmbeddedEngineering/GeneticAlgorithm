#pragma once
#include "Phenotype.h"
#include "DecisionTree.h"
#include "TaskGraph.h"
#include "FunctionType.h"
#include "Node.h"
#include <random>

class PopulationGenerator {
private:
    const TaskGraph& graph;
    const int numberOfChilds;
    std::mt19937_64 rng;

    // wybiera random funkcje
    FunctionType randomFunctionType();

    // tworzy pojedynczego Node'a
    std::unique_ptr<Node> createRandomNode();

    // Polaczyc te dwie funkcje w jedno!!! TODO
    // DFS do budownia galezi
    void expandTree(Node* currentNode, int maxDepth);
    // generator jednego drzewa
    DecisionTree buildSingleTree(int maxDepth);

public:
    explicit PopulationGenerator(const TaskGraph& graph, int numberOfChilds);

    // generator Generacji 0
    std::vector<DecisionTree> generatePopulationZero(int populationSize);

    // generator kolejnej Generacji X
    std::vector<DecisionTree> generateNextPopulation(const std::vector<DecisionTree>& prevPopulation,
        int populationSize);

    // Tutaj funkcje do wybierania rodzicow - metoda RANKINGOWA
    std::vector<DecisionTree> selectParents(const std::vector<DecisionTree>& population, int numParents);

    // bierzemy Drzewo A i Drzewo B, losujemy po jednym wezle i odcinami i zamieniamy miejscami(krzyżowanie)
    void crossover(DecisionTree& parentA, DecisionTree& parentB);

    // losujemy węzeł drzewa i go zmieniamy(mutujemy)
    // np. zmieniamy jego funkcję, ewentualnie jeszcze targetTaskId, targetProcessorId
    void mutate(DecisionTree& tree);

    // odpalenie symulacji
    // TODO
    // shared pointer dodac
    Phenotype run(const Phenotype& initialSolution);
};
