#include "EvolutionParams.hpp"
#include "Phenotype.hpp"
#include "PopulationGenerator.hpp"
#include "TaskGraph.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <memory>

int main() {
    std::shared_ptr<TaskGraph> graph;
    try {
        std::string graphPath = "../testGraph.txt";
        graph = std::make_shared<TaskGraph>(graphPath);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    const EvolutionParams params(graph->getTaskCount(), graph->getProcessorsCount(), 1.0, 0.1, 0.6,
                                 0.3, 20, 100, 4, 3);

    Phenotype initialSolution(graph);
    PopulationGenerator populationGenerator(graph, initialSolution, params);

    auto start = std::chrono::high_resolution_clock::now();
    Phenotype bestPhenotype = populationGenerator.run();
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Czas wykonania: " << std::chrono::duration<double>{end - start}.count() << "s"
              << std::endl;

    return 0;
}
