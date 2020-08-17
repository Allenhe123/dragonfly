#ifndef SUBSCRIBE_PROXY_H_
#define SUBSCRIBE_PROXY_H_

#include <sstream>
#include <deque>
#include <chrono>
#include <thread>
#include "asio.hpp"
#include "msg.h"
#include "msg.pb.h"
#include "time.h"


using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket): socket_(std::move(socket)) {
    }

    void start() {
        do_read_header();
    }

    void close() {
        socket_.close();
    }

private:
    void do_read_header()
    {
        auto self(shared_from_this());
        asio::async_read(socket_, asio::buffer(read_msg_.data(), df::Msg::header_length + df::Msg::type_length),
            [this, self](std::error_code ec, std::size_t size)
            {
                if (!ec && read_msg_.decode_header())
                {
                    printf("size: %d, type:%d\n", read_msg_.body_length(), read_msg_.type());
                    do_read_body();
                }
                else {
                    printf("do_read_header failed\n");
                }
            });
    }

    void do_read_body()
    {
        auto self(shared_from_this());
        asio::async_read(socket_, asio::buffer(read_msg_.body(), read_msg_.body_length()),
            [this, self](std::error_code ec, std::size_t size)
            {
                if (!ec) {
                    DeSerialize();
                    do_read_header();
                }
                else {
                    printf("do_read_body failed\n");
                }
            });
    }

private:
    void DeSerialize() {
        if (read_msg_.type() == df::Msg::MsgType::MSG_TYPE_TEST) {
            MsgTest msg;
            msg.ParseFromArray(read_msg_.body(), read_msg_.body_length());
            std::ostringstream oss;
            oss << msg.id() << " " << msg.data() << " " << msg.timestamp();
            auto delta = (df::DfTime::Now() - msg.timestamp()) / 1000.0f;
            printf("recv: %s, delta: %f us\n", oss.str().c_str(), delta);
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    df::Msg read_msg_;
};

class SubscribeProxy {
public:
    SubscribeProxy(asio::io_context& ctx, uint32_t port, uint32_t qidx): 
        port_(port), qidx_(qidx), acceptor_(ctx, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }
    ~SubscribeProxy()  = default;

    void Close() {
        session_->close();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
            if (!ec) {
                printf("accept success!\n");
                session_ = std::make_shared<Session>(std::move(socket));
                session_->start();
            }
            else {
                printf("accept failed!\n");
            }

            do_accept();
        });
    }

private:
    tcp::acceptor acceptor_;
    std::shared_ptr<Session> session_;

    uint32_t port_ = 0;
    uint32_t qidx_ = 0;
};


#endif