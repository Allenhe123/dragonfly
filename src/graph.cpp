#include "graph.h"

namespace df {

Graph::Graph(int32_t id): graphid_(id) {
}

void Graph::Destory() {
    engines_.clear();
}

void Graph::SendData(const EnginePortID& id, const Task t) {
    auto ite = engines_.find(id);
    if (ite == engines_.end()) {
        printf("Graph::SendData can not find the engine:%d\n", id.engine_id);
        return;
    }
    ite->second->Push(t);
}

void Graph::SetFunctor(const EnginePortID& id, FUNCTOR func)
{
    auto ite = engines_.find(id);
    if (ite == engines_.end()) {
        printf("SetFunctor can not find the engine \n");
        return;
    }
    return ite->second->SetFunctor(func);
}

std::shared_ptr<Engine> Graph::GetEngine(const EnginePortID& id) const noexcept {
    auto ite = engines_.find(id);
    if (ite == engines_.end()) {
        printf("GetEngine can not find the engine \n");
        return nullptr;
    }
    return ite->second;
}

std::shared_ptr<Engine> Graph::GetEngine(int32_t id) const noexcept {
    for (const auto& e : engines_) {
        if (e.first.engine_id == id) {
            return e.second;
        }
    }
}

void Graph::AddEngine(const EnginePortID& id, const std::shared_ptr<Engine>& e) noexcept {
    engines_[id] = e;
}

const EngineList& Graph::GetEngineList() const noexcept {
    return engines_;
}

void Graph::AddConn(const Connect& c) noexcept {
    conns_.push_back(c);
}

const ConnList& Graph::GetConnList() const noexcept {
    return conns_;
}

void Graph::Dump() const noexcept {
    for (const auto& e: engines_) {
        printf("graph-id:%d, engine-id:%d, port-id:%d\n", e.first.graph_id, e.first.engine_id, e.first.port_id);
        e.second->Dump();
    }
    printf("------------------\n");
}


}
