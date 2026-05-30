#include "EvolutionParams.h"
#include "Phenotype.h"
#include "PopulationGenerator.h"

#include <filesystem>

int main() {
    constexpr int numTasks = 12;
    constexpr int numProcessors = 4;
    const EvolutionParams params(numTasks, numProcessors, 5.0, 0.1, 0.6, 0.3, 20, 100, 10);

    const auto graph = std::make_shared<TaskGraph>(); // TODO: czytaj graf z pliku
    PopulationGenerator populationGenerator(graph, params);
    const Phenotype initialSolution(graph); // TODO: wygeneruj pierwszy, najgorszy fenotyp
    populationGenerator.run(initialSolution);

    return 0;
}
