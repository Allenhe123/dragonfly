#include "engine.h"

#include <cstdio>
#include <cstring>
#include <cassert>
#include <sstream>

#include <sys/syscall.h>

#include "common.h"
#include "time.h"
#include "pin_thread.h"

namespace df {

Engine::Engine(int id, int threadnum, int inputSize, int outputSize): inQueue_(inputSize),
    input_num_(inputSize), output_num_(outputSize), id_(id) {
    childs_.resize(outputSize);
}

Engine::~Engine() {
    Stop();
}

struct Input {
    uint64_t ts_;
    std::string str_;

    Input(uint64_t ts, const std::string& str): ts_(ts), str_(str) {} 
};

void Engine::Init() {
    for (int i=0; i<thread_num_; i++) {
         threads_.push_back(std::thread( [this]() {
            this->tid_.store(static_cast<int>(syscall(SYS_gettid)));
            for (;;) {
                if (this->stop_) break;

                std::unique_lock<std::mutex> lk(this->mutex_);
                this->cv_.wait(lk);  // , [this]{return !this->args_.empty();}
                lk.unlock();

                this->functor_(this->args_, this->output_);

                this->PushToChildren();
            }
         }));
    }
}


void Engine::PushToChildren() const {
    if (output_.size() != output_num_) {
        printf("ERROR\n");
        return;
    }

    for (int i =0; i<output_.size(); i++) {
        if (i < childs_.size()) {
            childs_[i]->Push(i, output_[i]);
        }
    }
}

void Engine::SetFunctor(FUNCTOR f) noexcept {
    functor_ = f;
}

void Engine::Stop() noexcept {
    stop_ = true;
    NotifyAll();

    for (auto& thrd: threads_) {
        if (thrd.joinable()) {
            thrd.join();
        }
    }
}

void Engine::NotifyOne() {
    notify_time_ = df::DfTime::Now();
    cv_.notify_one();
}

void Engine::NotifyAll() {
    cv_.notify_all();
}

bool Engine::SetChild(int idx, const std::shared_ptr<Engine> ch) noexcept { 
    if (idx >= childs_.size()) return false;    
    childs_[idx] = ch;
    return true;
}

const std::shared_ptr<Engine> Engine::Child(int idx) const noexcept {
    if (idx <= childs_.size()) return childs_[idx];
    return nullptr;
}

void Engine::AddPublisher(uint32_t qidx, const std::string& ip, uint32_t port) noexcept {
    auto ptr = std::make_shared<Publisher>();
    ptr->remote_queue_idx = qidx;
    ptr->remote_ip = ip;
    ptr->remote_port = port;
    publishers_.push_back(std::move(ptr)); 
}

void Engine::AddRecipient(uint32_t qidx, uint32_t port) noexcept {
    auto ptr =std::make_shared<Recipient>();
    ptr->local_queue_idx = qidx;
    ptr->listen_port = port;
    recipients_.push_back(std::move(ptr));
}

bool Engine::SetSchedAffinity(int32_t idx, const std::string& affinity, const std::vector<int>& cpus) {
    if (affinity.empty() || cpus.empty()) return false;
    if (idx >= threads_.size()) return false;
    apollo::cyber::scheduler::SetSchedAffinity(&threads_[idx], cpus, affinity);
    return true;
}

bool Engine::SetSchedPolicy(int32_t idx, int poriorty, const std::string& policy) {
    if (policy.empty() || poriorty == -1) return false;
    if (idx >= threads_.size()) return false;
    std::ostringstream oss;
    oss << "Engine:" << id_ << " thread:" << idx;
    apollo::cyber::scheduler::SetSchedPolicy(&threads_[idx], policy, poriorty, oss.str());
    return true;
}

void Engine::Push(int32_t queueIdx, const Task& data) {
    inQueue_.PushData(queueIdx, data);

    {
        std::lock_guard<std::mutex> lk(mutex_);
        args_.clear();
        if (!inQueue_.PopAllData(args_)) return;
    }
 
    NotifyOne();
}

int32_t Engine::Id() const noexcept {
    return id_;
}

void Engine::Dump() const noexcept {
    printf("Engine-id: %d, input-size: %d, output-size: %d\n", id_, input_num_, output_num_);
    for (const auto& ch : childs_) {
        printf("        Engine-id: %d\n", ch->Id());
    }
}

void Engine::InitPublisher() {
    if (!publishers_.empty()) {
        for (const auto& p : publishers_) {
            publisher_proxys_.emplace_back(new PublisherProxy(io_context_pub_, p->remote_ip, 
                p->remote_port, p->remote_queue_idx));
            publish_threads_.push_back(std::thread( [this] () { this->io_context_pub_.run(); }));
        }
    }
}



}
