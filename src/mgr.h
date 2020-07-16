#ifndef INC_DF_INIT_H_
#define INC_DF_INIT_H_

#include <string>
#include <memory>
#include "common.h"
#include "graph.h"

namespace df {

class GraphMgr
{
public:
    virtual ~GraphMgr();
    void ShutDown();
    bool CreateGraph(const std::string& config);
    void Dump() const noexcept;

    void SetFunctor(const EnginePortID& id, FUNCTOR func) noexcept;
    void SendData(const EnginePortID& id, const Task t) const noexcept;

private:
    std::shared_ptr<Graph> GetGraph(uint32_t graphid) const noexcept;
    std::shared_ptr<Engine> GetEngine(const EnginePortID& id) const noexcept;
    
private:
    std::unordered_map<int, std::shared_ptr<Graph>>  graphs_;
    DECLARE_SINGLETON(GraphMgr);
};

}

#endif
