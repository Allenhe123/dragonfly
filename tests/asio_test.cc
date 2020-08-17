
#include "gtest/gtest.h"

#include "../src/publisher_proxy.h"
#include "../src/subscribe_proxy.h"

TEST (ASIO_TEST, Test_Post1) {
    asio::io_context io_context;
    asio::post(io_context, []() {
        printf("asio post call1 func\n");
    });
    io_context.run();
}

TEST (ASIO_TEST, Test_Post2) {
    asio::io_context io_context;
    std::thread t([&io_context]() {
        io_context.run();
    });

    asio::post(io_context, []() {
        printf("asio post call2 func\n");
    });

    if (t.joinable()) t.join();
}

TEST (ASIO_TEST, Test_Timer1) {
    asio::io_context io_context;
    asio::steady_timer t(io_context, asio::chrono::seconds(5));
    t.wait();
    std::cout << "hello world!" << std::endl;
}

// The asio library provides a guarantee that callback handlers will only be called from 
// threads that are currently calling io_context::run(). Therefore unless the 
// io_context::run() function is called the callback for the asynchronous wait completion 
// will never be invoked.
TEST (ASIO_TEST, Test_Timer2) {
    asio::io_context io_context;
    asio::steady_timer t(io_context, asio::chrono::seconds(5));
    //perform a async wait on the timer
    t.async_wait( [] (const asio::error_code& e) {
        EXPECT_TRUE(!e);
        std::cout << "hello world!" << std::endl;
    });
    io_context.run();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}