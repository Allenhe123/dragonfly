#include "../mgr.h"

using namespace df;

int main(int argc, char* argv[]) {
    GraphMgr::Instance()->CreateGraph("sample.json");
    // GraphMgr::Instance()->CleanUp();

    return 0;
}