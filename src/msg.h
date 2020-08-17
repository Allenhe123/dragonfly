#ifndef DRAGONFLY_MESSAGE_H_
#define DRAGONFLY_MESSAGE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace df {

class Msg
{
public:
    enum { header_length = 4 };
    enum { type_length = 4 };
    enum { max_body_length = 512 };
    enum class MsgType { MSG_TYPE_TEST = 1 };

    Msg(): body_length_(0), buffer_(data_) {}
    Msg(MsgType type): body_length_(0), type_(type), buffer_(data_) {}

    ~Msg() {
        if (heap_buffer_ != nullptr) {
            delete []heap_buffer_;
            heap_buffer_ = nullptr;
        }
    }

    const char* data() const
    {
        return buffer_;
    }

    char* data()
    {
        return buffer_;
    }

    std::size_t length() const
    {
        return header_length + type_length + body_length_;
    }

    const char* body() const
    {
        return buffer_ + header_length + type_length;
    }

    char* body()
    {
        return buffer_ + header_length + type_length;
    }

    std::size_t body_length() const
    {
        return body_length_;
    }

    MsgType type() const noexcept {
        return type_;
    }

    void set_body_length(std::size_t new_length)
    {
        if (new_length <= max_body_length) { buffer_ = data_;}
        else {
            if (new_length <= heap_buffer_size_) { buffer_ = heap_buffer_; }
            else {
                if (heap_buffer_ != nullptr) delete []heap_buffer_;
                heap_buffer_ = new char[new_length + header_length + type_length];
                heap_buffer_size_ = new_length + header_length + type_length;
            }
        }
        body_length_ = new_length;
    }

    bool decode_header()
    {
        char header[header_length + 1] = "";
        std::strncat(header, buffer_, header_length);
        set_body_length(std::atoi(header));
        std::memcpy(header, buffer_ + header_length, type_length);
        type_ = static_cast<MsgType>(std::atoi(header));
        return true;
    }

    void encode_header()
    {
        char header[header_length + 1] = "";
        std::sprintf(header, "%4d", static_cast<int>(body_length_));
        std::memcpy(buffer_, header, header_length);
        std::sprintf(header, "%4d", static_cast<int>(type_));
        std::memcpy(buffer_ + header_length, header, type_length);
    }

private:
    char data_[header_length + type_length + max_body_length];
    std::size_t body_length_;

    char* heap_buffer_ = nullptr;
    size_t heap_buffer_size_ = 0;
    char* buffer_ =  nullptr;

    MsgType type_;
};

}

#endif