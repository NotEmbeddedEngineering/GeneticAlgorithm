#include "EvolutionParams.h"
#include "FunctionType.h"

#include <iostream>

// KONFIGURACJA
EvolutionParams getDefaultParams(){
    return EvolutionParams{
        .alpha = 5.0,
        .beta = 0.1,  // % mutacji
        .gamma = 0.6, // % krzyzowania 
        .delta = 0.3, // % klonowania
        .mutationRate = 0.05,

        .epsilon = 20,
        .maxGenerations = 100,
        .maxTreeDepth = 10
    };
}


// WYKONANIE
int main() {
    auto params = getDefaultParams();

    const int numTasks = 12;
    const int numProcessors = 4;

    params.setup(numTasks, numProcessors);

    return 0;
}
