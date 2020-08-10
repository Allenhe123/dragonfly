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

    // bool operator == (const EnginePortID& other) const noexcept {
    //     return graph_id == other.graph_id 
    //     && engine_id == other.engine_id
    //     && port_id == other.port_id;
    // }

    // bool operator != (const EnginePortID& other) const noexcept {
    //     return graph_id != other.graph_id 
    //     || engine_id != other.engine_id
    //     || port_id != other.port_id;
    // }
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
        return lhs.graph_id == rhs.graph_id 
        && lhs.engine_id == rhs.engine_id 
        && lhs.port_id == rhs.port_id;
    }
};

struct Connect {
    int src_id;
    int src_port;
    int dst_id;
    int dst_port;
};

using EngineList = std::unordered_map<EnginePortID, std::shared_ptr<Engine>, HashFunc, CmpFunc>;
using ConnList = std::vector<Connect>;

class Graph
{
public:
    Graph() = default;
    Graph(int32_t id, int priority, const std::string& policy): id_(id),
          priority_(priority), policy_(policy) {}
    void Destory();
    void SetFunctor(const EnginePortID& id, FUNCTOR func);
    void SendData(const EnginePortID& id, const Task t);

    std::shared_ptr<Engine> GetEngine(const EnginePortID& id) const noexcept;
    std::shared_ptr<Engine> GetEngine(int32_t id) const noexcept;
    void AddEngine(const EnginePortID& id, const std::shared_ptr<Engine> e) noexcept;
    void AddConn(const Connect& c) noexcept;
    uint32_t Id() const noexcept { return id_; }

    const EngineList& GetEngineList() const noexcept;
    const ConnList& GetConnList() const noexcept;
    void Dump() const noexcept;

private:
    uint32_t id_ = 0;
    int priority_;
    std::string policy_;
    EngineList engines_;
    ConnList conns_;
};

}

#endif
