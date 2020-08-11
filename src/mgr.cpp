#include "mgr.h"

#include <fstream>
#include <sstream>
#include <string>

#include "file.h"
#include "dfConfig.pb.h"
#include "graph.pb.h"
#include "engine.pb.h"
#include "connect.pb.h"

#include "common.h"
#include "pin_thread.h"

namespace df {

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
        graphs_[graph.id()] = graph_instance;

        std::vector<int> cpus;
        apollo::cyber::scheduler::ParseCpuset(graph.cpus(), &cpus);
        apollo::cyber::scheduler::SetSchedAffinity1(pthread_self(), cpus, graph.cpu_affi());
        apollo::cyber::scheduler::SetSchedPolicy1(pthread_self(), graph.policy(), graph.priority(), "main thread");

        for (int j=0; j<graph.engines_size(); j++) {
            auto& engine = graph.engines(j);
            auto engine_instance = std::make_shared<Engine>(engine.id(), engine.thread_num(),
                                                            engine.parent_num(), engine.child_num());
            engine_instance->Init();

            cpus.clear();
            apollo::cyber::scheduler::ParseCpuset(engine.cpus(), &cpus);

            for (int k=0; k<engine.thread_num(); k++) {
                engine_instance->SetSchedAffinity(k, engine.cpu_affi(), cpus);
                engine_instance->SetSchedPolicy(k, engine.priority(), engine.policy());
            }

            EnginePortID epid;
            epid.graph_id = graph_instance->Id();
            epid.engine_id = engine_instance->Id();
            epid.port_id = 0;
            graph_instance->AddEngine(epid, engine_instance);
        }

        for (int j=0; j<graph.connect_intra_size(); j++) {
            auto& conn_intra = graph.connect_intra(j);
            const EngineList& eglist = graph_instance->GetEngineList();

            EnginePortID epid;
            epid.graph_id = graph.id();
            epid.engine_id = conn_intra.dst_engine_id();
            auto ite_child = eglist.find(epid);
            epid.engine_id = conn_intra.src_engine_id();
            auto ite_parent = eglist.find(epid);

            if (ite_child == eglist.end() || ite_parent == eglist.end()) {
                std::cout << "invalid graph id or engine id" << std::endl;
                return false;
            }

            // parent has OutputNum() outut args
            // child has InputNum() input args
            // parent's src_port_id() output to child's dst_port_id() input queue
            assert(conn_intra.src_port_id() < ite_parent->second->OutputNum());
            assert(conn_intra.dst_port_id() < ite_child->second->InputNum());
            ite_parent->second->SetChild(conn_intra.src_port_id(), ite_child->second);
            ite_parent->second->AddConn(conn_intra.src_port_id(), conn_intra.dst_port_id());
        }

        for (int j=0; j<graph.connect_publiser_size(); j++) {
            auto& conn_pub = graph.connect_publiser(j);
            const EngineList& eglist = graph_instance->GetEngineList();
            EnginePortID epid;
            epid.graph_id = graph.id();
            epid.engine_id = conn_pub.engine_id();
            auto ite = eglist.find(epid);
            ite->second->AddPublisher(conn_pub.port_id(), 
                    conn_pub.remote_ip(), conn_pub.remote_port());
        }

        for (int j=0; j<graph.connect_recipient_size(); j++) {
            auto& conn_rec = graph.connect_recipient(j);
            const EngineList& eglist = graph_instance->GetEngineList();
            EnginePortID epid;
            epid.graph_id = conn_rec.graph_id();
            epid.engine_id = conn_rec.engine_id();
            auto ite = eglist.find(epid);
            ite->second->AddRecipient(conn_rec.port_id(), conn_rec.listen_port());
        }
    }
}

void GraphMgr::Dump() const noexcept {
    printf("+++++++++++++++++++++++++++++++++\n");
    for (const auto& g : graphs_) {
	    g.second->Dump();
    }
    printf("+++++++++++++++++++++++++++++++++\n");
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
