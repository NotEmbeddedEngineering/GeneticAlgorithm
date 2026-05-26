#include "EvolutionParams.h"
#include "FunctionType.h"

#include <iostream>

int main() {
    EvolutionParams params{};
    params.alpha = 5.0;
    params.beta = 0.1;  // % mutacji
    params.gamma = 0.6; // % krzyzowania
    params.delta = 0.3; // % klony
    params.mutationRate = 0.05;

    params.maxGenerations = 100;
    params.epsilon = 20;
    params.maxTreeDepth = 10;

    int numTasks = 12;
    int numProcessors = 4;

    params.setup(numTasks, numProcessors);

    return 0;
}
