#include "../mgr.h"
#include <cstdio>
#include <thread>
#include <chrono>

using namespace df;

/////// 用可变模板参数???
Task process1(const Task& t) {
    std::shared_ptr<std::string> input_arg = std::static_pointer_cast<std::string>(t);

    printf("process1: %s\n", input_arg->c_str());

   // std::shared_ptr<std::string> output_string_ptr = std::make_shared<std::string>();

    return std::static_pointer_cast<void>(input_arg);
}

Task process2(const Task& t) {
    std::shared_ptr<std::string> input_arg = std::static_pointer_cast<std::string>(t);

    printf("process2: %s\n", input_arg->c_str());

    return std::static_pointer_cast<void>(input_arg);
}

Task process3(const Task& t) {
    std::shared_ptr<std::string> input_arg = std::static_pointer_cast<std::string>(t);

    printf("process3: %s\n", input_arg->c_str());

    return std::static_pointer_cast<void>(input_arg);
}

int main(int argc, char* argv[]) {
    GraphMgr::Instance()->CreateGraph("sample.json");
    GraphMgr::Instance()->Dump();
    
    EnginePortID id;
    id.graph_id = 100;
    id.engine_id = 1000;
    id.port_id = 0;
    GraphMgr::Instance()->SetFunctor(id, process1);
    id.engine_id = 1001;
    GraphMgr::Instance()->SetFunctor(id, process2);
    id.engine_id = 1002;
    GraphMgr::Instance()->SetFunctor(id, process3);
    printf("after setfunctor\n");

    id.engine_id = 1000;
    for (;;) {
        std::shared_ptr<std::string> tt = std::make_shared<std::string>("hello world!!!");
        auto t =  std::static_pointer_cast<void>(tt);
        GraphMgr::Instance()->SendData(id, t);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
	//break;
    }

    GraphMgr::Instance()->CleanUp();

    return 0;
}
