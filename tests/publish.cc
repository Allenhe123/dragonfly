
#include "gtest/gtest.h"

#include "../src/publisher_proxy.h"
#include "../src/subscribe_proxy.h"

/*
TEST (TestPub, SyncPub) {
    const int max_length = 1024;
    asio::io_context io_context;
    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    asio::connect(s, resolver.resolve("127.0.0.1", "8989"));

    EXPECT_TRUE(s.is_open());

    while (1)
    {
        std::cout << "Enter message: ";
        char request[max_length];
        std::cin.getline(request, max_length);
        if (strcmp("q", request) == 0) break;
        size_t request_length = std::strlen(request);
        asio::write(s, asio::buffer(request, request_length));
    }
}
*/

TEST (TestPub, AsyncPub) {
    asio::io_context context_pub;
    PublisherProxy proxy_pub(context_pub, "127.0.0.1", 8989, 0);

    std::thread thread_pub( [&context_pub] () {
        context_pub.run();
    });

    // add below line, post will not called
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    proxy_pub.Test();


    // char line[256];
    // while (std::cin.getline(line, 256))
    // {
    //     auto p = std::shared_ptr<char []> (new char[256]);
    //     strncpy(p.get(), line, strlen(line));
    //     // proxy_pub.Write(p, strlen(line));
    //     proxy_pub.Test();
    //     printf("write++ \n");
    // }

    char line[256] = "GGGGGGGGGGG";
    for (int i=0; i<3; i++)
    {
        auto p = std::shared_ptr<char []> (new char[256]);
        strncpy(p.get(), line, strlen(line));
        proxy_pub.Write(p, strlen(line));
        printf("write++ \n");
    }

    // for (size_t i=0; i<10; i++) {
    //     std::ostringstream oss;
    //     oss << "hello server, " << i << " times\n";
    //     // auto p = std::make_shared<char []>(oss.str().size());
    //     auto p = std::shared_ptr<char []> (new char[oss.str().size()]);
    //     strncpy(p.get(), oss.str().c_str(), oss.str().size());
    //     proxy_pub.Write(p, oss.str().size());
    //     // printf("%s", p.get());
    //     std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // }

    // proxy_pub.Close();

    thread_pub.join();

    printf("####################\n");

    // for (size_t i=0; i<1; i++) {
    //     proxy_pub.Write("hello", 5);
    // }

    // asio::io_context io;
    // asio::steady_timer t(io, asio::chrono::seconds(2));
    // t.wait();

    // proxy_pub.Close();

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}