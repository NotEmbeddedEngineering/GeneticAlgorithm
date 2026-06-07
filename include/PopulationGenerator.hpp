#pragma once

#include "DecisionTree.hpp"
#include "EvolutionParams.hpp"
#include "FunctionType.hpp"
#include "Node.hpp"
#include "Phenotype.hpp"
#include "TaskGraph.hpp"
#include <random>

class PopulationGenerator {
public:
    explicit PopulationGenerator(std::shared_ptr<TaskGraph> graph, Phenotype& initialSolution,
                                 const EvolutionParams& params);

    // generator Generacji 0
    std::vector<DecisionTree> generatePopulationZero();

    // generator kolejnej Generacji X
    std::vector<DecisionTree>
    generateNextPopulation(const std::vector<EvaluatedTree>& prevPopulation);

    // Ocenia populacje
    std::vector<EvaluatedTree> evaluatePopulation(const std::vector<DecisionTree>& population,
                                                  const Phenotype& baseSolution);

    // Funkcja do wybierania osobników - metoda RANKINGOWA
    std::vector<DecisionTree> selection(const std::vector<EvaluatedTree>& population,
                                        int populationSize);

    // bierzemy Drzewo A i Drzewo B, losujemy po jednym wezle i odcinami i zamieniamy miejscami(krzyżowanie)
    void crossover(DecisionTree& parentA, DecisionTree& parentB);

    // losujemy węzeł drzewa i go zmieniamy(mutujemy)
    // np. zmieniamy jego funkcję, ewentualnie jeszcze targetTaskId, targetProcessorId
    void mutate(DecisionTree& tree);

    // odpalenie symulacji
    Phenotype run();

private:
    std::shared_ptr<TaskGraph> graph;
    Phenotype& currentSolution;
    const EvolutionParams params;
    std::mt19937_64 rng;

    // wybiera random funkcje
    // trzeba będzie walnąc switcha i zwracać odpowiedni typ Node, np. CHANGE_TASK_PROCESSOR_RANDOM -> ChangeTaskProcessorRandomNode
    FunctionType randomFunctionType();

    // tworzy pojedynczego Node'a
    std::unique_ptr<Node> createRandomNode();

    // DFS do budownia galezi
    void expandTree(Node*, int remainingDepth);

    // generator jednego drzewa
    DecisionTree buildSingleTree(int maxDepth);
};
