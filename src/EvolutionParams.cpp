#include "EvolutionParams.h"

#include <cmath>

EvolutionParams::EvolutionParams(int taskCount, int processorCount, double alpha, double beta,
                                 double gamma, double delta, int epsilon, int maxGenerations,
                                 int maxTreeDepth) {

    this->alpha = alpha;
    this->beta = beta;
    this->gamma = gamma;
    this->delta = delta;
    this->epsilon = epsilon;
    this->maxGenerations = maxGenerations;
    this->maxTreeDepth = maxTreeDepth;

    populationSize = static_cast<int>(std::round(alpha * taskCount * processorCount));
    numClones = static_cast<int>(std::round(delta * populationSize));
    numMutations = static_cast<int>(std::round(beta * populationSize));
    numCrossovers = static_cast<int>(std::round(gamma * populationSize));
}
