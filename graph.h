#ifndef INC_DF_GRAPH_H_
#define INC_DF_GRAPH_H_

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <utility>

#include "engine.h"

namespace df {

struct EnginePortID
{
    uint32_t graph_id = 0;
    uint32_t engine_id = 0;
    uint32_t port_id = 0;
};

struct HashFunc
{
    size_t operator () (const EnginePortID& e) const noexcept {
        return std::hash<uint32_t>()((e.engine_id << 4) ^ (e.graph_id << 5) ^ (e.port_id << 6));
    }
};

struct CmpFunc
{
    bool operator () (const EnginePortID& lhs,  const EnginePortID& rhs) const noexcept {
        return lhs.engine_id == rhs.engine_id 
        && lhs.graph_id == rhs.engine_id 
        && lhs.port_id == rhs.port_id;
    }
};

class Graph
{
public:
    Graph() = default;
    Graph(int32_t id);
    void Destory();
    void SetFunctor(const EnginePortID& id, FUNCTOR func);
    void SendData(const EnginePortID& id, const Task& t);

    std::shared_ptr<Engine> GetEngine(const EnginePortID& id) const noexcept;
    void AddEngine(const EnginePortID& id, const std::shared_ptr<Engine>& e) noexcept;

private:
    uint32_t graphid_ = 0;
    std::unordered_map<EnginePortID, std::shared_ptr<Engine>, HashFunc, CmpFunc> engines_;

};

}

#endif