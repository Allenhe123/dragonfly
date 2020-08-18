
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
    // perform a async wait on the timer
    t.async_wait( [] (const asio::error_code& e) {
        EXPECT_TRUE(!e);
        std::cout << "hello world!" << std::endl;
    });
    // 用到了异步调用接口，必须调用io_context.run()启动事件循环
    io_context.run();
}



void print(asio::steady_timer* t, int* count) {
    if (*count < 5) {
        std::cout << "cnt: " << *count << std::endl;
        ++(*count);
        t->expires_at(t->expiry() + asio::chrono::seconds(1));
        t->async_wait(std::bind(print, t, count));
    }
}

TEST (ASIO_TEST, Test_Handler1) {
    asio::io_context io_ctx;
    int cnt = 0;
    asio::steady_timer timer(io_ctx, asio::chrono::seconds(1));

    timer.async_wait(std::bind(print, &timer, &cnt));
    io_ctx.run();
}

TEST (ASIO_TEST, Test_Handler2) {
    class Printer {
    public:
        Printer(asio::io_context& io): timer_(io, asio::chrono::seconds(1)), count_(0) {
            timer_.async_wait(std::bind(&Printer::print, this));
        }

        ~Printer()
        {
            std::cout << "Final count is " << count_ << std::endl;
        }

        void print()
        {
            if (count_ < 5) {
                std::cout << count_ << std::endl;
                ++count_;
                timer_.expires_at(timer_.expiry() + asio::chrono::seconds(1));
                timer_.async_wait(std::bind(&Printer::print, this));
            }
        }

    private:
        asio::steady_timer timer_;
        int count_;
    };

    asio::io_context io;
    Printer p(io);
    io.run();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}