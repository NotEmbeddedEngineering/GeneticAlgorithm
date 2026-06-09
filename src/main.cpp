#include "EvolutionParams.hpp"
#include "Phenotype.hpp"
#include "PopulationGenerator.hpp"
#include "TaskGraph.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

int main(int32_t argc, char** argv) {
    std::shared_ptr<TaskGraph> graph;
    EvolutionParams params = EvolutionParams(0, 0);

    double maxTime = 20000;
    double timeScale = 10.0;
    double costScale = 1.0;
    double penalty = 0.0;

    try {

        if (argc < 13) {
            std::cerr
                << "Invalid params please provide everything.\n"
                << argc
                << " params provided\n";
            std::string graphPath = "../testGraph.txt";
            graph = std::make_shared<TaskGraph>(graphPath);
            params = EvolutionParams(graph->getTaskCount(), graph->getProcessorsCount(), 2.0, 0.3,
                                     0.6, 0.1, 10, 20, 5, 3);
        } else {
            std::string graphPath = argv[1];
            graph = std::make_shared<TaskGraph>(graphPath);
            params = EvolutionParams(graph->getTaskCount(), graph->getProcessorsCount(),
                                     std::stod(argv[2]), std::stod(argv[3]), std::stod(argv[4]),
                                     std::stod(argv[5]), std::stoi(argv[6]), std::stoi(argv[7]),
                                     std::stoi(argv[8]), std::stoi(argv[9]));

            maxTime = std::stod(argv[10]);
            timeScale = std::stod(argv[11]);
            costScale = std::stod(argv[12]);
            penalty = std::stod(argv[13]);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    Phenotype initialSolution(graph, maxTime, timeScale, costScale, penalty);
    initialSolution.evaluate();
    // std::cout << "Initial solution: \n" << initialSolution.toString();
    PopulationGenerator populationGenerator(graph, initialSolution, params);

    auto start = std::chrono::high_resolution_clock::now();
    Phenotype bestPhenotype = populationGenerator.run();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout
        << "Czas wykonania: "
        << std::chrono::duration<double>{end - start}.count()
        << "s"
        << std::endl;
    std::cout << "Fenotyp: \n" << bestPhenotype.toString();

    return 0;
}
