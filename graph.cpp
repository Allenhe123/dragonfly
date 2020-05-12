#include "graph.h"

namespace df {

int Graph::CreateGraph(const std::string& config) {

    return 0;
}

std::shared_ptr<Graph> Graph::GetInstance(uint32_t graphid) {
    auto ite = graphs_.find(graphid);
    return ite != graphs_.end() ? ite->second : nullptr;
}

void Graph::DestoryGraph() {
    graphs_.clear();
}

void Graph::SendData(EnginePortID engineid, const Task& t)
{
    auto ite = engines_.find(engineid);
    if (ite == engines_.end()) {
        printf("can not find the engine \n");
        return;
    }
    ite->second->Push(t);
}




}
