#include "../mgr.h"
#include <cstdio>

using namespace df;


Task process1(const Task& t) {

    printf("process1 called\n");

    return nullptr;
}

Task process2(const Task& t) {

    printf("process1 called\n");
    return nullptr;
}

Task process3(const Task& t) {

    printf("process1 called\n");
    return nullptr;
}

int main(int argc, char* argv[]) {
    GraphMgr::Instance()->CreateGraph("sample.json");
    

    EnginePortID id;
    id.graph_id = 100;
    id.engine_id = 1000;
    id.port_id = 0;
    GraphMgr::Instance()->SetFunctor(id, process1);
    id.engine_id = 1001;
    GraphMgr::Instance()->SetFunctor(id, process2);
    id.engine_id = 1002;
    GraphMgr::Instance()->SetFunctor(id, process3);


    GraphMgr::Instance()->CleanUp();

    return 0;
}