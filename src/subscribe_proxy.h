#ifndef SUBSCRIBE_PROXY_H_
#define SUBSCRIBE_PROXY_H_

#include <sstream>
#include <deque>
#include <chrono>
#include <thread>
#include "asio.hpp"

using asio::ip::tcp;

// struct Msg {
//     Msg(char* p, size_t l): data_(p), len_(l) {}
//     char* data_ = nullptr;
//     size_t len_ = 0;
// };

// using MsgQueue = std::deque<Msg>;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket): socket_(std::move(socket)) {
        
    }

    void start() {
        do_read();
    }

    void close() {
        socket_.close();
    }

private:

    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_, max_length),
        [this, self](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                printf("recv len:%lu, msg: %s\n", length, data_);
            } 

            do_read();
        });

        // auto self(shared_from_this());
        // asio::async_read(socket_, asio::buffer(data, 1024), [this, self](std::error_code ec, std::size_t /*length*/) 
        // {
        //     if (!ec) {
        //         std::cout << "read msg: " << data << std::endl;
        //     }
        //     else
        //     {
        //         do_read();
        //     }
        // });
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
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