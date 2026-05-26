#include "DecisionTree.h"
#include "Node.h"

int main() {
    auto root = std::make_unique<Node>();
    root->children.emplace_back(std::make_unique<ChangeProcessorRandomNode>());

    DecisionTree tree(std::move(root));
    TaskGraph graph;
    Phenotype baseSolution(graph);
    Phenotype solution = tree.decode(baseSolution);

    return 0;
}
