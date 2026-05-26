#pragma once

struct EvolutionParams {
    double alpha;   // Mnożnik populacji
    double beta;    // Prawdopodobieństwo mutacji
    double gamma;   // Prawdopodobieństwo krzyżowania
    double delta;   // klonowanie

    int epsilon;        // Warunek stopu (ile pokoleń bez poprawy znosimy)
    int maxGenerations; // Twardy limit pokoleń
    int maxTreeDepth;   // Głębokość drzewa

    double populationSize; // Pi
    double numClones;      // f
    double numMutations;   // omega
    double numCrossovers;  // ksi
    double mutationRate;   // %

};
