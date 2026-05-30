#pragma once

struct EvolutionParams {
    EvolutionParams(int taskCount, int processorCount, double alpha = 5.0, double beta = 0.1,
                    double gamma = 0.6, double delta = 0.3, int epsilon = 20,
                    int maxGenerations = 100, int maxTreeDepth = 10);

    double alpha = 5.0;       // Mnożnik populacji
    double beta = 0.1;        // Prawdopodobieństwo mutacji
    double gamma = 0.6;       // Prawdopodobieństwo krzyżowania
    double delta = 0.3;       // klonowanie
    int epsilon = 20;         // Warunek stopu (ile pokoleń bez poprawy znosimy)
    int maxGenerations = 100; // Twardy limit pokoleń
    int maxTreeDepth = 10;    // Głębokość drzewa

    // Automatycznie obliczone w konstruktorze
    int populationSize; // Pi: Całkowita liczebność pokolenia (alpha * num_tasks * num_resources)
    int numClones;      // f: Liczba osobników przechodzących bez zmian (delta * Pi)
    int numMutations;   // omega: Liczba osobników losowo modyfikowanych (beta * Pi)
    int numCrossovers;  // ksi: Liczba osobników do krzyżowania (gamma * Pi)
    int numberOfChilds; // maksymalna liczba dzieci w drzewie
};
