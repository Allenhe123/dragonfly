
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

// Synchronising handlers in multithreaded programs
TEST (ASIO_TEST, Test_Handler3) {
    class Printer {
    public:
        Printer(asio::io_context& io): strand_(asio::make_strand(io)), 
            timer1_(io, asio::chrono::seconds(1)), timer2_(io, asio::chrono::seconds(1)),
            count_(0) {
                timer1_.async_wait(asio::bind_executor(strand_, std::bind(&Printer::Print1, this)));
                timer2_.async_wait(asio::bind_executor(strand_, std::bind(&Printer::Print2, this)));
            }
        ~Printer() {
            std::cout << "Final count is " << count_ << std::endl;
        }

        void Print1() {
            if (count_ < 10)
            {
                std::cout << "Timer 1: " << count_ << std::endl;
                ++count_;

                timer1_.expires_at(timer1_.expiry() + asio::chrono::seconds(1));

                timer1_.async_wait(asio::bind_executor(strand_,
                        std::bind(&Printer::Print1, this)));
            }
        }
        void Print2() {
            if (count_ < 10)
            {
                std::cout << "Timer 2: " << count_ << std::endl;
                ++count_;

                timer2_.expires_at(timer2_.expiry() + asio::chrono::seconds(1));

                timer2_.async_wait(asio::bind_executor(strand_,
                        std::bind(&Printer::Print2, this)));
            }
        }

    private:
        asio::strand<asio::io_context::executor_type> strand_;
        asio::steady_timer timer1_;
        asio::steady_timer timer2_;
        int count_;
    };


    asio::io_context io;
    Printer p(io);
    // std::thread t(std::bind(&asio::io_context::run, &io));
    std::thread t([&io]() {
        io.run();
    });
    io.run();
    t.join();
}

// sync client
TEST (ASIO_TEST, Test_TCP1) {
    using asio::ip::tcp;
    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "daytime");
    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);
    for (;;)
    {
        std::array<char, 128> buf;
        asio::error_code error;
        size_t len = socket.read_some(asio::buffer(buf), error);
        if (error == asio::error::eof)
            break; // Connection closed cleanly by peer.
        else if (error)
            throw asio::system_error(error); // Some other error.
        std::cout.write(buf.data(), len);
    }
}

// sync daytime server
std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}
TEST (ASIO_TEST, Test_TCP2) {
    using asio::ip::tcp;
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));
    for (;;)
    {
        tcp::socket socket(io_context);
        acceptor.accept(socket); 
        std::string message = make_daytime_string();
        asio::error_code ignored_error;
        asio::write(socket, asio::buffer(message), ignored_error);
    }
}

// asynchronous TCP daytime server
TEST (ASIO_TEST, Test_TCP3) {
    using asio::ip::tcp;
    asio::io_context io_context;

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}