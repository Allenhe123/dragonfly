
#include "gtest/gtest.h"

#include "../src/publisher_proxy.h"
#include "../src/subscribe_proxy.h"

TEST (ASIO_TEST, Test1) {
    asio::io_context io_context;
    asio::post(io_context, []() {
        printf("asio post call1 func\n");
    });
    io_context.run();
}

TEST (ASIO_TEST, Test2) {
    asio::io_context io_context;
    std::thread t([&io_context]() {
        io_context.run();
    });

    asio::post(io_context, []() {
        printf("asio post call2 func\n");
    });
    

    if (t.joinable()) t.join();
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}