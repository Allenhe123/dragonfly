#include "graph.h"

namespace df {

Graph::Graph(int32_t id): graphid_(id) {
}

void Graph::Destory() {
    engines_.clear();
}

void Graph::SendData(const EnginePortID& id, const Task& t) {
    auto ite = engines_.find(id);
    if (ite == engines_.end()) {
        printf("can not find the engine \n");
        return;
    }
    ite->second->Push(t);
}

void Graph::SetFunctor(const EnginePortID& id, FUNCTOR func)
{
    auto ite = engines_.find(id);
    if (ite == engines_.end()) {
        printf("can not find the engine \n");
        return;
    }
    return ite->second->SetFunctor(func);
}

std::shared_ptr<Engine> Graph::GetEngine(const EnginePortID& id) const noexcept {
    auto ite = engines_.find(id);
    if (ite == engines_.end()) {
        printf("can not find the engine \n");
        return nullptr;
    }
    return ite->second;
}

void Graph::AddEngine(const EnginePortID& id, const std::shared_ptr<Engine>& e) noexcept {
    engines_[id] = e;
}

}
