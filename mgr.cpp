#include "mgr.h"
#include "common.h"
#include "rapidjson/document.h"
#include <fstream>
#include <sstream>

namespace df {

void GraphMgr::ShutDown() {
    for (auto i : graphs_) {
        i.second->Destory();
    }
    graphs_.clear();
}

bool GraphMgr::CreateGraph(const std::string& config) {
    using namespace rapidjson;
    using namespace std;
    if (!file_exist(config)) {
        printf("config file: %s not exist \n", config.c_str());
        return false;
    }

    ifstream ifs(config);
    if (!ifs.is_open()) {
        printf("open file %s failed\n", config.c_str());
        return false;
    }
    stringstream buffer;
    buffer << ifs.rdbuf();
    string json(buffer.str());
    Document document; 
    if (document.Parse(json.c_str()).HasParseError()) return false;
    assert(document.IsObject());

    struct engine {
        int id;
        string name;
        int priority;
        int policy;
        string cpuaffi;
        int thread_num;
        std::vector<int> cpus;
    };

    struct connect {
        int src_id;
        int src_port;
        int dst_id;
        int dst_port;
    };

    struct graph {
        int id;
        int priority;
        int policy;
        std::vector<engine> engines;
        std::vector<connect> conns;
    };

    vector<graph> graphs;

    for (size_t i = 0; i < 100; i++)
    {
        char buf[256] = {0};
        snprintf(buf, 256, "graph%d", i);
        if (document.HasMember(buf) && document[buf].IsObject()) {
            const rapidjson::Value& obj = document[buf];
            graph g;
            g.id = obj["graph_id"].GetInt();
            g.priority = obj["priority"].GetInt();
            g.policy = obj["policy"].GetInt();
            for (int j=0; j<100; j++) {
                char buffer[256] = {0};
                snprintf(buffer, 256, "engine%d", j);
                if (obj.HasMember(buffer) && obj[buffer].IsObject()) {
                    const rapidjson::Value& eobj = obj[buffer];
                    engine e;
                    e.id = eobj["id"].GetInt();
                    e.name = eobj["engine_name"].GetString();
                    e.priority = eobj["priority"].GetInt();
                    e.policy = eobj["policy"].GetInt();
                    e.cpuaffi = eobj["cpu_affi"].GetString();
                    e.thread_num = eobj["thread_num"].GetInt();
                    const Value& array = eobj["cpus"];
                    if (array.IsArray()) {
                        for (SizeType k=0; k<array.Size(); k++) {
                            e.cpus.push_back(array[k].GetInt());
                        }
                    }
                    g.engines.push_back(e);
                }
                memset(buffer, 0, 256);
                snprintf(buffer, 256, "connects%d", j);
                if (obj.HasMember(buffer) && obj[buffer].IsObject()) {
                    const rapidjson::Value& cobj = obj[buffer];
                    connect c;
                    c.src_id = cobj["src_engine_id"].GetInt();
                    c.src_port = cobj["src_port_id"].GetInt();
                    c.dst_id = cobj["target_engine_id"].GetInt();
                    c.dst_port = cobj["target_port_id"].GetInt();
                    g.conns.push_back(c);
                }
            }
            graphs.push_back(g);
        }
    }

    for (const auto& g : graphs) {
        auto graph = std::make_shared<Graph>(g.id);
        for (const auto& e : g.engines) {
            auto engine = std::make_shared<Engine>(e.id, e.priority, 
                          e.policy, e.cpuaffi, e.cpus, e.thread_num);
            EnginePortID id;
            id.graph_id = g.id;
            id.engine_id = e.id;
            id.port_id = 0;
            graph->AddEngine(id, engine);
        }

        for (const auto& c : g.conns) {
            auto conn = std::make_shared
        }
    }
    








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
