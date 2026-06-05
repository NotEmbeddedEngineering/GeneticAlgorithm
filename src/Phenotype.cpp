#include "Phenotype.hpp"
#include <algorithm>
#include <cstddef>
#include <limits>
#include <queue>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

Phenotype::Phenotype(const std::shared_ptr<TaskGraph> graph) : graph(graph) {
    // Init internals
    this->taskToPhenotypeProcessor = std::vector<size_t>(this->graph->getTaskCount(), 0);
    this->phenotypeProcUsage = {};

    // Count indegree for correct order of processing
    std::vector<size_t> indegree(this->graph->getTaskCount());
    std::vector<std::vector<size_t>> predecessors(this->graph->getTaskCount(),
                                                  std::vector<size_t>());
    for (size_t t = 0; t < this->graph->getTaskCount(); ++t) {
        for (auto [n, _] : this->graph->getAdj()[t]) {
            ++indegree[n];
            predecessors[n].push_back(t);
        }
    }

    std::queue<size_t> tasksToAssign{};
    for (size_t t = 0; t < this->graph->getTaskCount(); ++t) {
        if (indegree[t] == 0) {
            tasksToAssign.push(t);
        }
    }

    // Update as earliest start time for this task but add data transfer time at processing
    std::vector<double> endTimes(this->graph->getTaskCount(), 0);
    // Used to check if we can use this proc for task
    std::unordered_map<size_t, std::vector<std::pair<double, double>>> phenotypeProcTimespans{};

    while (!tasksToAssign.empty()) {
        size_t t = tasksToAssign.front();
        tasksToAssign.pop();

        // Process all neigbours
        for (auto [nb, _] : this->graph->getAdj()[t]) {
            if (--indegree[nb] == 0) {
                tasksToAssign.push(nb);
            }
        }

        // Assign best proc to task
        auto procsView =
            std::views::iota(static_cast<size_t>(0), this->graph->getProcessorsCount());
        auto bestProcIt = std::ranges::min_element(procsView, {}, [&, t](size_t procId) {
            if (this->graph->canExecute(procId, t)) {
                return this->graph->getTime(procId, t);
            }
            return std::numeric_limits<int32_t>::max();
        });

        size_t bestProcId = *bestProcIt;
        int32_t minTime = this->graph->getTime(bestProcId, t);

        if (predecessors[t].empty()) {
            endTimes[t] = minTime;
            // No predecessors we WANT to buy new proc always
            this->phenotypeProcToTgProc.push_back(bestProcId);
            this->phenotypeProcUsage.push_back(minTime);
            this->taskToPhenotypeProcessor[t] = phenotypeProcToTgProc.size() - 1;
            continue;
        }

        // Now Funcking Data Transfer Shenaniganss
        // I cooked (https://pin.it/7w6D3f6lB) ok?? It was 3 lines without chanels
        auto calculateEndTime = [&](int32_t predId) -> double {
            auto predTgProc = this->getTgProcId(this->getPhenotypeProcId(predId));
            if (predTgProc != bestProcId || this->graph->getProc(predTgProc).isHC()) {
                auto bestChanId = this->graph->findFastestChanel(predTgProc, bestProcId);
                auto [_, data] = *std::ranges::find_if(this->graph->getAdj()[predId], [t](Edge e) {
                    return static_cast<size_t>(e.targetTaskId) == t;
                });

                int32_t maxTransfer = this->graph->getChan(bestChanId).bandwidth;

                return endTimes[predId] +
                       static_cast<double>(data) / static_cast<double>(maxTransfer);
            }
            return endTimes[predId];
        };

        auto lastPredIt = std::ranges::max_element(predecessors[t], {}, calculateEndTime);
        double earliestStart = calculateEndTime(*lastPredIt);

        if (this->graph->getProc(bestProcId).isHC()) {
            // Just add proc and dont care
            this->phenotypeProcToTgProc.push_back(bestProcId);
            this->phenotypeProcUsage.push_back(minTime);
            this->taskToPhenotypeProcessor[t] = phenotypeProcToTgProc.size() - 1;
            endTimes[t] = minTime + earliestStart;
        } else {
            // Do some PP shinanigans
            if (phenotypeProcToTgProc[taskToPhenotypeProcessor[*lastPredIt]] == bestProcId) {
                // no transfer needed just use last pred proc
                bool assignable = true;
                for (const auto& [s, e] :
                     phenotypeProcTimespans[taskToPhenotypeProcessor[*lastPredIt]]) {
                    if ((s > earliestStart && s < earliestStart + minTime) ||
                        (e > earliestStart && e < earliestStart + minTime)) {
                        assignable = false;
                    }
                }

                if (assignable) {
                    this->taskToPhenotypeProcessor[t] = taskToPhenotypeProcessor[*lastPredIt];
                    phenotypeProcTimespans[taskToPhenotypeProcessor[*lastPredIt]].emplace_back(
                        earliestStart, earliestStart + minTime);
                    endTimes[t] = minTime + earliestStart;
                    this->phenotypeProcUsage[this->taskToPhenotypeProcessor[t]] += minTime;
                    continue;
                }
            }

            bool assigned = false;
            for (size_t phProcId = 0; phProcId < this->phenotypeProcToTgProc.size(); ++phProcId) {
                if (phenotypeProcToTgProc[phProcId] == bestProcId) {
                    // Check if we cna insert
                    bool assignable = true;
                    for (const auto& [s, e] : phenotypeProcTimespans[phProcId]) {
                        double taskEnd = earliestStart + minTime;
                        if (std::max(s, earliestStart) < std::min(e, taskEnd)) {
                            assignable = false;
                        }
                    }
                    if (assignable) {
                        this->taskToPhenotypeProcessor[t] = taskToPhenotypeProcessor[phProcId];
                        phenotypeProcTimespans[taskToPhenotypeProcessor[phProcId]].emplace_back(
                            earliestStart, earliestStart + minTime);
                        endTimes[t] = minTime + earliestStart;
                        this->phenotypeProcUsage[this->taskToPhenotypeProcessor[t]] += minTime;
                        assigned = true;
                        break;
                    }
                }
            }
            if (assigned) {
                continue;
            }

            this->phenotypeProcToTgProc.push_back(bestProcId);
            this->phenotypeProcUsage.push_back(minTime);
            this->taskToPhenotypeProcessor[t] = phenotypeProcToTgProc.size() - 1;
            endTimes[t] = minTime + earliestStart;
        }
    }
}

void Phenotype::evaluate() {
    // TODO DO THE CHORE
    // Count indegree for correct order of processing
    std::vector<size_t> indegree(this->graph->getTaskCount());
    std::vector<std::vector<size_t>> predecessors(this->graph->getTaskCount(),
                                                  std::vector<size_t>());

    for (size_t t = 0; t < this->graph->getTaskCount(); ++t) {
        for (auto [n, _] : this->graph->getAdj()[t]) {
            ++indegree[n];
            predecessors[n].push_back(t);
        }
    }

    std::queue<size_t> tasksToProcess{};
    for (size_t t = 0; t < this->graph->getTaskCount(); ++t) {
        if (indegree[t] == 0) {
            tasksToProcess.push(t);
        }
    }

    std::vector<double> startTimes{};
    std::vector<double> endTimes{};
    // Says when proc is made avalivable
    std::vector<double> phProcFree(this->phenotypeProcToTgProc.size(), 0);
    std::vector<std::unordered_set<size_t>> phChanConectedPhProcs(this->graph->getChannelsCount(),
                                                                  std::unordered_set<size_t>());
    while (!tasksToProcess.empty()) {
        auto t = tasksToProcess.front();
        tasksToProcess.pop();

        auto taskPhProc = this->getPhenotypeProcId(t);
        // Select correct start time
        // On proc first come first serve
        startTimes[t] = std::max(startTimes[t], phProcFree[taskPhProc]);
        endTimes[t] = startTimes[t] + this->graph->getTime(this->getTgProcId(taskPhProc), t);

        // Mark as used up to task end
        // TODO could introduce timespans fuckery as some task MABEY could go between prev phProcFree and startTimes[t]
        phProcFree[taskPhProc] = endTimes[t];

        for (auto [nb, data] : this->graph->getAdj()[t]) {
            double transferTime = 0.0;

            auto tPhProc = this->getPhenotypeProcId(t);
            auto nbPhProc = this->getPhenotypeProcId(nb);

            if (tPhProc != nbPhProc) {

                auto fastestChanId = this->graph->findFastestChanel(this->getTgProcId(tPhProc),
                                                                    this->getTgProcId(nbPhProc));
                phChanConectedPhProcs[fastestChanId].insert(tPhProc);
                phChanConectedPhProcs[fastestChanId].insert(nbPhProc);

                transferTime = static_cast<double>(data) /
                               static_cast<double>(this->graph->getChan(fastestChanId).bandwidth);
                startTimes[nb] = std::max(startTimes[nb], endTimes[t] + transferTime);
            }
        }
    }

    // Now cost CHORE
    // Check if proc is used.
    std::vector<bool> phProcUsed(this->phenotypeProcToTgProc.size(), 0);
    for (auto t : std::views::iota(0uz, this->graph->getTaskCount())) {
        phProcUsed[this->getPhenotypeProcId(t)] = true;
    }
    double cost = 0.0;

    // Add cost of used PPs
    for (auto phProc : std::views::iota(0uz, this->phenotypeProcToTgProc.size())) {
        if (phProcUsed[phProc] == false) {
            // TODO ADD REMOVAL AND UPDATE TO MAP
            auto taskTgProc = this->graph->getProc(this->getTgProcId(phProc));
            if (taskTgProc.isPP()) {
                // Add cost
                cost += static_cast<double>(taskTgProc.cost);
            }
        }
    }

    // Add task cost
    for (auto t : std::views::iota(0uz, this->graph->getTaskCount())) {
        auto tTgProc = this->getTgProcId(this->getPhenotypeProcId(t));
        cost += this->graph->getCost(tTgProc, t);
    }

    //   Establish conections of procs
    for (auto chId : std::views::iota(0uz, phChanConectedPhProcs.size())) {
        auto chan = this->graph->getChan(chId);
        // FIX:? Tu nie powinien byc kosz samej szyny + koszt podlaczenia tego procesora?
        cost += chan.cost * phChanConectedPhProcs[chId].size();
    }
    auto endTime = *std::ranges::max_element(endTimes);

    // TODO: improve fitness fn
    this->startTimes = startTimes;
    this->endTimes = endTimes;
    // TODO: IMPROVE, WE HAVE TO DECIDE WEIGHS AND HOW TO TREAT NOT MEETING TIME CONSTRAINT
    this->fitnessScore = 1.0 / endTime + 1.0 / cost;
}

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
    return this->phenotypeProcToTgProc.size() - 1;
}

std::shared_ptr<TaskGraph> Phenotype::getGraph() const {
    auto tg_ptr = this->graph;
    return tg_ptr;
}

void Phenotype::changeTaskProc(size_t taskId, size_t phenotypeProcId) {
    auto oldProcId = this->getPhenotypeProcId(taskId);
    this->taskToPhenotypeProcessor[taskId] = phenotypeProcId;

    // Maintain correct processor usage
    this->phenotypeProcUsage[oldProcId] -=
        this->graph->getTime(this->getTgProcId(oldProcId), taskId);
    this->phenotypeProcUsage[phenotypeProcId] -=
        this->graph->getTime(this->getTgProcId(phenotypeProcId), taskId);

    // New proc is not yet used
    phenotypeProcUsage.push_back(0);
}

size_t Phenotype::getPhenotypeProcCount() const {
    return this->phenotypeProcToTgProc.size();
}
int32_t Phenotype::getPhenotypeProcUsage(size_t phenotypeProcId) const {
    return this->phenotypeProcUsage[phenotypeProcId];
}
