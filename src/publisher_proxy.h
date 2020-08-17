#ifndef PUBLISHER_PROXY_H_
#define PUBLISHER_PROXY_H_

#include <sstream>
#include <deque>
#include <chrono>
#include <thread>
#include <mutex>
#include "asio.hpp"
#include "msg.h"

using asio::ip::tcp;
using MsgQueue = std::deque<std::shared_ptr<df::Msg>>;

class PublisherProxy {
public:
    PublisherProxy(asio::io_context& ctx, const std::string& ip, uint32_t port, uint32_t qidx):
        io_context_(ctx), socket_(ctx), ip_(ip), port_(port), qidx_(qidx) {
            tcp::resolver resolver(ctx);
            std::ostringstream oss;
            oss << port;
            endpoints_ = resolver.resolve(ip, oss.str());
            do_connect(endpoints_);
        }

    ~PublisherProxy() { socket_.close(); }
    
    void Write(std::shared_ptr<df::Msg> msg) {
        asio::post(io_context_, [this, msg]() {
            bool write_in_progress = !msg_queue_.empty();
            msg_queue_.push_back(msg);

            if (!write_in_progress)
            {
                printf("+++++++call do_write\n");
                do_write();
            }
        });
    }

    void Close()
    {
        asio::post(io_context_, [this]() { socket_.close(); });
    }

    bool Connected() const noexcept { return connected_; }

private:
    void do_connect(const tcp::resolver::results_type& endpoints)
    {
        asio::async_connect(socket_, endpoints,
            [this](std::error_code ec, tcp::endpoint ep) {
                if (!ec) {
                    this->connected_ = true;
                    printf("connect to %s:%u successfully\n", ip_.c_str(), port_);
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    printf("retry connect to %s:%lu\n", ip_.c_str(), port_);
                    do_connect(endpoints_);
                }
            }
        );
    }

    void do_write()
    {
        asio::async_write(socket_, asio::buffer(msg_queue_.front()->data(), msg_queue_.front()->length()),
        [this](std::error_code ec, std::size_t size) {
          if (!ec)
          {
            msg_queue_.pop_front();
            if (!msg_queue_.empty())
            {
               do_write();
            }
          }
          else
          {
            socket_.close();
          }
        });
    }

/*
底层套接字的一些函数，如发送等也无法保证一次异步操作就把所有的数据都通过TCP流发送出去。async_write将被告知有多少
字节实际发送了，然后要求在下一次异步操作时发送剩余的字节。async_write是通过一次或者多次调用async_write_some
函数来实现的，那么如果在第一个async_write还没有完成就调用第二个async_write，async_write_some就有可能先
将第二个buffer的数据先发送出去。
因此，NEVER start your second async_write before the first has completed.
*/

private:
    asio::io_context& io_context_;
    tcp::socket socket_;
    std::string ip_;
    uint32_t port_;
    uint32_t qidx_ = 0;
    tcp::resolver::results_type endpoints_;
    bool connected_ = false;

    std::mutex mutex_;

    MsgQueue msg_queue_;
};


#endif