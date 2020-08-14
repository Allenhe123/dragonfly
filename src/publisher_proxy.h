#ifndef PUBLISHER_PROXY_H_
#define PUBLISHER_PROXY_H_

#include <iostream>
#include <sstream>
#include <deque>
#include <chrono>
#include <thread>
#include "asio.hpp"
// #include "asio/io_context.hpp"

using asio::ip::tcp;

struct Msg {
    Msg(char* p, size_t l): data_(p), len_(l) {}
    char* data_ = nullptr;
    size_t len_ = 0;
};

using MsgQueue = std::deque<Msg>;

class PublisherProxy {
public:
    PublisherProxy(asio::io_context& ctx, const std::string& ip, uint32_t port, uint32_t qidx): 
        io_context_(ctx), socket_(ctx), ip_(ip), port_(port) {
            tcp::resolver resolver(ctx);
            std::ostringstream oss;
            oss << port;
            auto endpoints = resolver.resolve(ip, oss.str());
            endpoints_ = endpoints;
            do_connect(endpoints);
        }

    ~PublisherProxy() { socket_.close(); }
    
    void Write(char* msg, size_t length) {
        msg_queue_.emplace_back(msg, length);
    }

    bool Connected() const noexcept { return connected_; }

private:
    void do_connect(const tcp::resolver::results_type& endpoints)
    {
        asio::async_connect(socket_, endpoints,
            [this](std::error_code ec, tcp::endpoint) {
                if (!ec) {
                    connected_ = true;
                    std::cout << "connect to " << ip_ << ":" << port_ << "successfully" << std::endl;
                } else {
                    std::cout << "retry connect to " << ip_ << ":" << port_ << std::endl;
                    do_connect(endpoints_);
                }
            }
        );
    }


    void do_write()
    {
        asio::async_write(socket_,
            asio::buffer(msg_queue_.front().data_, msg_queue_.front().len_),
            [this](std::error_code ec, std::size_t len) {
            if (!ec) {
                msg_queue_.pop_front();
                if (!msg_queue_.empty())
                {
                    do_write();
                }
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            });
    }

private:
    asio::io_context& io_context_;
    tcp::socket socket_;
    std::string ip_;
    uint32_t port_;
    tcp::resolver::results_type endpoints_;
    bool connected_ = false;

    MsgQueue msg_queue_;
};


#endif