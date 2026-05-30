#include "EvolutionParams.h"
#include "Phenotype.h"
#include "PopulationGenerator.h"
#include "TaskGraph.h"

#include <exception>
#include <iostream>
#include <memory>

int main() {

    std::unique_ptr<TaskGraph> graph{};
    try {
        std::string graphPath = "./testGraph.txt";
        const auto graph = std::make_shared<TaskGraph>(graphPath);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    const EvolutionParams params(graph->getTaskCount(), graph->getProcessorsCount(), 5.0, 0.1, 0.6,
                                 0.3, 20, 100, 10);
    PopulationGenerator populationGenerator(graph, params);
    const Phenotype initialSolution(graph); // TODO: wygeneruj pierwszy, najgorszy fenotyp
    populationGenerator.run(initialSolution);

    return 0;
}
