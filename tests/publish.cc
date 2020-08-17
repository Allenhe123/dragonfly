
#include "gtest/gtest.h"

#include "../src/publisher_proxy.h"
#include "../src/subscribe_proxy.h"
#include "../src/time.h"
#include "msg.pb.h"

TEST (TestPub, SyncPub) {
    const int max_length = 1024;
    asio::io_context io_context;
    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    asio::connect(s, resolver.resolve("127.0.0.1", "8989"));

    EXPECT_TRUE(s.is_open());

    for (size_t i=0; i<10; i++)
    {
        MsgTest msgTest;
        msgTest.set_id(i);
        msgTest.set_timestamp(df::Now());
        std::ostringstream oss;
        oss << "sync hello server, " << i << " times";
        msgTest.set_data(oss.str());

        auto msg = std::make_shared<df::Msg>(df::Msg::MsgType::MSG_TYPE_TEST);
        msg->set_body_length(msgTest.ByteSizeLong());
        msg->encode_header();
        msgTest.SerializeToArray(msg->body(), msgTest.ByteSizeLong());
        asio::write(s, asio::buffer(msg->data(), msg->length()));
    }
}

TEST (TestPub, AsyncPub) {
    asio::io_context context_pub;
    PublisherProxy proxy_pub(context_pub, "127.0.0.1", 8989, 0);

    std::thread thread_pub( [&context_pub] () {
        context_pub.run();
    });

    // add below line, post will not called
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    for (size_t i=0; i<10; i++) {

        MsgTest msgTest;
        msgTest.set_id(i);
        msgTest.set_timestamp(df::Now());
        std::ostringstream oss;
        oss << "async hello server, " << i << " times";
        msgTest.set_data(oss.str());

        auto msg = std::make_shared<df::Msg>(df::Msg::MsgType::MSG_TYPE_TEST);
        msg->set_body_length(oss.str().size());
        msg->encode_header();
        msgTest.SerializeToArray(msg->body(), msgTest.ByteSizeLong());
        proxy_pub.Write(msg);
    }

    // proxy_pub.Close();
    thread_pub.join();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}