#ifndef INC_DF_GRAPH_H_
#define INC_DF_GRAPH_H_

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

#include "engine.h"

namespace df {

using FUNCTOR = std::function<void(const std::shared_ptr<void>&)>;

struct EnginePortID
{
    uint32_t graph_id = 0;
    uint32_t engine_id = 0;
    uint32_t port_id = 0;
};

class Graph
{
public:
    static int CreateGraph(const std::string& config);
    static std::shared_ptr<Graph> GetInstance(uint32_t graphid);
    static void DestoryGraph();

    void SetRecvFunc(FUNCTOR func);
    void SendData(EnginePortID engineid, const Task& t);

private:
    static std::unordered_map<int, std::shared_ptr<Graph>>  graphs_;

    FUNCTOR functor_ = nullptr;
    std::unordered_map<EnginePortID, std::shared_ptr<Engine>> engines_;

};

}

#endif