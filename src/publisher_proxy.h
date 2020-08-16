#ifndef PUBLISHER_PROXY_H_
#define PUBLISHER_PROXY_H_

#include <sstream>
#include <deque>
#include <chrono>
#include <thread>
#include <mutex>
#include "asio.hpp"

using asio::ip::tcp;

struct Msg {
    Msg(std::shared_ptr<char []> p, size_t l): data_(p), len_(l) {}
    std::shared_ptr<char []> data_ = nullptr;
    size_t len_ = 0;
};

using MsgQueue = std::deque<Msg>;

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


    void Test() {
    asio::post(io_context_, []() {
        printf("++++++++++++++\n");        
    });
    }
    
    void Write(std::shared_ptr<char []> p, size_t length) {
        asio::post(io_context_, [this, p, length]() {
            printf("post call back...\n");
            bool write_in_progress = !msg_queue_.empty();
            Msg msg(p, length);
            msg_queue_.push_back(msg);
            if (!write_in_progress)
            {
                do_write();
            }
        });


        // bool call_do_write = false;
        // if (msg_queue_.empty())  call_do_write = true;
        // {
        //     std::lock_guard<std::mutex> lk(mutex_);
        //     msg_queue_.emplace_back(p, length);
        //     printf("### qsize:%d\n", msg_queue_.size());
        // }
        
        // if (call_do_write) do_write();

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
        printf("call do_write....\n");
    asio::async_write(socket_, asio::buffer(msg_queue_.front().data_.get(), msg_queue_.front().len_),
        [this](std::error_code ec, std::size_t /*length*/) {
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
    // void do_write()
    // {  
    //     if (!msg_queue_.empty()) {
    //         printf("call asyn write,.......\n");
    //         asio::async_write(socket_, asio::buffer(msg_queue_.front().data_.get(), msg_queue_.front().len_),
    //         [this](std::error_code ec, std::size_t len) {
    //             if (!ec) {
    //                 {
    //                     std::lock_guard<std::mutex> lk(mutex_);
    //                     msg_queue_.pop_front();
    //                     printf("async write success, size: %d \n", msg_queue_.size());
    //                 }
    //                 do_write();
    //             } else {
    //                 std::this_thread::sleep_for(std::chrono::microseconds(1));
    //                 do_write();
    //             }
    //         });
    //     }
    // }

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