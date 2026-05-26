#pragma once
#include <cmath>

struct EvolutionParams {
    double alpha; // Mnożnik populacji
    double beta;  // Prawdopodobieństwo mutacji
    double gamma; // Prawdopodobieństwo krzyżowania
    double delta; // klonowanie

    double mutationRate;   // %

    int epsilon;        // Warunek stopu (ile pokoleń bez poprawy znosimy)
    int maxGenerations; // Twardy limit pokoleń
    int maxTreeDepth;   // Głębokość drzewa

    int populationSize; // Pi: Całkowita liczebność pokolenia (alpha * num_tasks * num_resources)
    int numClones;      // f: Liczba osobników przechodzących bez zmian (delta * Pi)
    int numMutations;   // omega: Liczba osobników losowo modyfikowanych (beta * Pi)
    int numCrossovers;  // ksi: Liczba osobników do krzyżowania (gamma * Pi)

    void setup(int taskCount, int processorCount) {
        populationSize = static_cast<int>(std::round(alpha * taskCount * processorCount));

        numClones = static_cast<int>(std::round(delta * populationSize));
        numMutations = static_cast<int>(std::round(beta * populationSize));
        numCrossovers = static_cast<int>(std::round(gamma * populationSize));
    }
};
