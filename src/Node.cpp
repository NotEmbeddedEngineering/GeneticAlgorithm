#include "../include/Node.h"

// --- NODE ---
std::unique_ptr<Node> Node::clone() const {
    auto clonedNode = std::make_unique<Node>(type, targetTaskId, targetProcessorId);

    for (const auto& child : children) {
        clonedNode->children.push_back(child->clone());
    }

    return clonedNode;
}

void Node::process(Phenotype& currentState) const {
    switch (this->type) {
        case FunctionType::CHANGE_PROCESSOR_RANDOM:
            // logika
            break;
        case FunctionType::CHANGE_CHANNEL_RANDOM:
            // ...
            break;

    }
}
