#include "mgr.h"
#include "file.h"

namespace df {

void GraphMgr::ShutDown() {
    for (auto i : graphs_) {
        i.second->Destory();
    }
    graphs_.clear();
}

bool GraphMgr::CreateGraph(const std::string& config) {
    using namespace df::common;
    if (!file_exist(config)) {
        printf("config file: %s not exist \n", config.c_str());
        return false;
    }

    if (!GetProtoFromFile(config, ))




}

std::shared_ptr<Graph> GraphMgr::GetGraph(uint32_t graphid) const noexcept {
    auto ite = graphs_.find(graphid);
    return ite == graphs_.end() ? nullptr : ite->second;
}

std::shared_ptr<Engine> GraphMgr::GetEngine(const EnginePortID& id) const noexcept {
    auto graph = GetGraph(id.graph_id);
    if (graph == nullptr) {
        return;
    }
    return graph->GetEngine(id);
}

void GraphMgr::SetFunctor(const EnginePortID& id, FUNCTOR func) noexcept {
    auto graph = GetGraph(id.graph_id);
    if (graph == nullptr) {
        return;
    }
    return graph->SetFunctor(id, func);
}

void GraphMgr::SendData(const EnginePortID& id, const Task& t) const noexcept {
    auto graph = GetGraph(id.graph_id);
    if (graph == nullptr) {
        return;
    }
    return graph->SendData(id, t);
}

}
