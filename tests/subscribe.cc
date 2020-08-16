
#include "gtest/gtest.h"

#include "../src/publisher_proxy.h"
#include "../src/subscribe_proxy.h"

TEST (TestSub, TestName1) {
    asio::io_context context_sub;
    SubscribeProxy proxy_sub(context_sub, 8989, 0);
    context_sub.run();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}