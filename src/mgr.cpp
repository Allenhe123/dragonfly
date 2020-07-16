#include "mgr.h"
#include "common.h"
#include "rapidjson/document.h"
#include <fstream>
#include <sstream>

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
            g.policy = obj["policy"].GetString();
            for (int j=0; j<100; j++) {
                char buffer[256] = {0};
                snprintf(buffer, 256, "engine%d", j);
                if (obj.HasMember(buffer) && obj[buffer].IsObject()) {
                    const rapidjson::Value& eobj = obj[buffer];
                    engine e;
                    e.id = eobj["id"].GetInt();
                    e.name = eobj["engine_name"].GetString();
                    e.priority = eobj["priority"].GetInt();
                    e.policy = eobj["policy"].GetString();
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
                    Connect c;
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
    printf("after parse json config file\n");

    for (const auto& g : graphs) {
        printf("##graph id:%d\n", g.id);
        auto graph = std::make_shared<Graph>(g.id);
        for (const auto& e : g.engines) {
            printf("  engine id:%d, priority:%d, policy:%s, cpuaffi:%s, threadnum:%d \n", 
                    e.id, e.priority, e.policy.c_str(), e.cpuaffi.c_str(), e.thread_num);
            auto engine = std::make_shared<Engine>(e.id, e.priority, 
                          e.policy, e.cpuaffi, e.cpus, e.thread_num);
            engine->Init();
            engine->SetSchedAffinity();
            engine->SetSchedPolicy();
            EnginePortID id;
            id.graph_id = g.id;
            id.engine_id = e.id;
            id.port_id = 0;
            graph->AddEngine(id, engine);
            printf("after add engine\n");
        }
        for (const auto& c : g.conns) {
            printf("srcid:%d, srcport:%d, dstid:%d, dstport:%d\n", c.src_id, c.src_port, c.dst_id, c.dst_port);
            graph->AddConn(c);
        }
        graphs_[g.id] = graph;
    }
    printf("after create engines\n");

    for (const auto& g : graphs_) {
        int id = g.first;
        const auto& graph = g.second;
        const EngineList& eglist = graph->GetEngineList();
        const ConnList& conlist = graph->GetConnList();

        for (auto& engine : eglist) {
            int32_t childid = findChild(conlist, engine.second);
            int32_t parentid = findParent(conlist, engine.second);
            if (childid != -1) {
                const auto& child = graph->GetEngine(childid);
                engine.second->SetChild(child);
                printf("%d set child to %d\n", engine.first.engine_id, childid);
            }
            if (parentid != -1) {
                const  auto& parent = graph->GetEngine(parentid);
                engine.second->SetParent(parent);
                printf("%d set parent to %d\n", engine.first.engine_id, parentid);
            }
        }
    }
    printf("after set connections\n");
    
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
