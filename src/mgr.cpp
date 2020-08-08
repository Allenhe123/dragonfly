#include "mgr.h"
#include "common.h"
// #include "rapidjson/document.h"
#include <fstream>
#include <sstream>

#include "file.h"
#include "dfConfig.pb.h"
#include "graph.pb.h"
#include "engine.pb.h"
#include "connect.pb.h"

namespace df {

namespace {
    using namespace std;
    struct engine {
        int id;
        string name;
        int priority;
        string policy;
        string cpuaffi;
        int thread_num;
        std::vector<int> cpus;
    };

    struct graph {
        int id;
        int priority;
        string policy;
        std::vector<engine> engines;
        std::vector<Connect> conns;
    };

    int32_t findChild(const std::vector<Connect>& conns, 
                      const std::shared_ptr<Engine>& e) {
        for (const auto& c : conns) {
            if (c.src_id == e->Id()) {
                return c.dst_id;
            }
        }
        return -1;
    }

    int32_t findParent(const std::vector<Connect>& conns,
                       const std::shared_ptr<Engine>& e) {
        for (const auto& c : conns) {
            if (c.dst_id == e->Id()) {
                return c.src_id;
            }
        }
        return -1;
    }

}

GraphMgr::GraphMgr() {}
GraphMgr::~GraphMgr() { ShutDown(); }

void GraphMgr::ShutDown() {
    for (auto i : graphs_) {
        i.second->Destory();
    }
    graphs_.clear();
}

bool GraphMgr::CreateGraph(const std::string& config) {
    df::conf::proto::DfConfig df_config;
    if (file_exist(config) && apollo::cyber::common::GetProtoFromFile(config, &df_config)) {
        std::cout << "load conf successfully" << std::endl;
    } else {
       return false;
    }

    for (int i=0; i<df_config.graphs_size(); i++) {
        auto& graph = df_config.graphs(i);
        auto graph_instance = std::make_shared<Graph>(graph.id(), graph.priority(), graph.policy());

        for (int j=0; j<graph.engines_size(); j++) {
            
        }


        graphs_[graph.id()] = graph_instance;
    }
}

void GraphMgr::Dump() const noexcept {
    for (const auto& g : graphs_) {
        printf("+++++++++++++++++\n");
        printf("graph-id: %d\n", g.first);
	g.second->Dump();
    }
}

std::shared_ptr<Graph> GraphMgr::GetGraph(uint32_t graphid) const noexcept {
    auto ite = graphs_.find(graphid);
    return ite == graphs_.end() ? nullptr : ite->second;
}

std::shared_ptr<Engine> GraphMgr::GetEngine(const EnginePortID& id) const noexcept {
    auto graph = GetGraph(id.graph_id);
    if (graph == nullptr) {
        return nullptr;
    }
    return graph->GetEngine(id);
}

void GraphMgr::SetFunctor(const EnginePortID& id, FUNCTOR func) noexcept {
    auto graph = GetGraph(id.graph_id);
    if (graph == nullptr) {
        printf("can not find graph: %d\n", id.graph_id);
        return;
    }
    return graph->SetFunctor(id, func);
}

void GraphMgr::SendData(const EnginePortID& id, const Task t) const noexcept {
    auto graph = GetGraph(id.graph_id);
    if (graph == nullptr) {
        printf("GraphMgr::SendData can not find  graph! graph_id: %d\n", id.graph_id);
        return;
    }
    return graph->SendData(id, t);
}

}
