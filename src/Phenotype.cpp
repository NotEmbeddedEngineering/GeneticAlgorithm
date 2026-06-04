#include "Phenotype.h"

Phenotype::Phenotype(const std::shared_ptr<TaskGraph> graph) : graph(graph) {}

void Phenotype::evaluate() {}

size_t Phenotype::getTgProcId(size_t phenotypeProcId) const {
    return phenotypeProcToTgProc[phenotypeProcId];
}

size_t Phenotype::getPhenotypeProcId(size_t taskId) const {
    return taskToPhenotypeProcessor[taskId];
}

// Adds internally new proc based on TaskGraph processor id and returns internal mapping
size_t Phenotype::addProc(size_t tgProcId) {
    // Add new processor to our phenotype architecture
    this->phenotypeProcToTgProc.push_back(tgProcId);
    // Add chanells conected to proc
    this->phenotypeProcToPhenotypeChannel.emplace_back();
    return this->phenotypeProcToTgProc.size() - 1;
}

std::shared_ptr<TaskGraph> Phenotype::getGraph() const {
    auto tg_ptr = this->graph;
    return tg_ptr;
}

bool Phenotype::isValidArchitecture() const {
    // TODO IMPLEMENT
    return true;
}

void Phenotype::changeTaskProc(size_t taskId, size_t phenotypeProcId) {
    auto oldProcId = this->getPhenotypeProcId(taskId);
    this->taskToPhenotypeProcessor[taskId] = phenotypeProcId;

    // Maintain correct processor usage
    this->phenotypeProcUsage[oldProcId] -=
        this->graph->getTime(this->getTgProcId(oldProcId), taskId);
    this->phenotypeProcUsage[phenotypeProcId] -=
        this->graph->getTime(this->getTgProcId(phenotypeProcId), taskId);
}
