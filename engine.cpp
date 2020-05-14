#include "engine.h"
#include "common.h"
#include <cstdio>
#include <cstring>
#include <sched.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/syscall.h>

namespace df {

Engine::Engine(int id, int priority, const std::string& policy, 
               const std::string& affi, const std::vector<int>& cpus, 
               int threadnum):id_(id), priority_(priority), 
               policy_(policy), cpu_affi_(affi), thread_num_(threadnum) {
               cpus_.assign(cpus.begin(), cpus.end());
}

Engine::~Engine() {
    Stop();
}

void Engine::Init() {
    std::thread t(&Engine::Entry, this);
    thread_ = std::move(t);
    thread_.join();
}

void Engine::SetFunctor(std::function<Task(const Task&)> f) noexcept {
    functor_ = f;
}

void Engine::Entry() {
    tid_.store(static_cast<int>(syscall(SYS_gettid)));
    for (;;) {
        if (stop_) break;

        std::unique_lock<std::mutex> lk(mutex_);
        cv_.wait(lk, [this]{ return !queue_.empty();});

        auto t = queue_.front();
        queue_.pop();
        lk.unlock();

        auto tt = functor_(t);

        if (child_ != nullptr && tt != nullptr) {
            child_->Push(tt);
            child_->NotifyOne();
        }
    }
}

void Engine::Stop() noexcept {
    stop_ = true;
    NotifyAll();
}

void Engine::NotifyOne() {
    cv_.notify_one();
}

void Engine::NotifyAll() {
    cv_.notify_all();
}

void Engine::SetChild(const std::shared_ptr<Engine>& ch) noexcept { child_ = ch; }
void Engine::SetParent(const std::shared_ptr<Engine>& pa) noexcept { parent_ = pa; }
const std::shared_ptr<Engine>& Engine::Child() const noexcept { return child_; }
const std::shared_ptr<Engine>& Engine::Parent() const noexcept { return parent_; }

bool Engine::SetSchedAffinity() {
    if (cpu_affi_.empty() || cpus_.empty()) return false;
    cpu_set_t set;
    CPU_ZERO(&set);
    if (cpus_.size()) {
        if (!cpu_affi_.compare("range")) {
            for (const auto cpu : cpus_) {
                CPU_SET(cpu, &set);
            }
            pthread_setaffinity_np(thread_.native_handle(), sizeof(set), &set);
        } else if (!cpu_affi_.compare("1to1")) {
            CPU_SET(cpus_[0], &set);
            pthread_setaffinity_np(thread_.native_handle(), sizeof(set), &set);
    }
  }
  return true;
}

bool Engine::SetSchedPolicy() {
    if (policy_.empty() || priority_ == -1) return false;
    struct sched_param sp;
    int policy;
    memset(reinterpret_cast<void *>(&sp), 0, sizeof(sp));
    sp.sched_priority = priority_;

    if (!policy_.compare("SCHED_FIFO")) {
        policy = SCHED_FIFO;
        pthread_setschedparam(thread_.native_handle(), policy, &sp);
    } else if (!policy_.compare("SCHED_RR")) {
        policy = SCHED_RR;
        pthread_setschedparam(thread_.native_handle(), policy, &sp);
    } else if (!policy_.compare("SCHED_OTHER")) {
        // Set normal thread nice value.
        while (tid_.load() == -1) {
            cpu_relax();
        }
        setpriority(PRIO_PROCESS, tid_.load(), priority_);
    }
    return true;
}

int32_t Engine::Priority() const noexcept {
    return priority_;
}

const std::string& Engine::Policy() const noexcept {
    return policy_;
}


const std::vector<int32_t>& Engine::Cores() const noexcept {
    return cpus_;
}

void Engine::Push(const Task& data) {
    while (queue_.size() >= 200)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::lock_guard<std::mutex> lk(mutex_);
    queue_.push(data);
}

uint32_t Engine::CurrentQueueSize() const noexcept {
    return queue_.size();
}

uint32_t Engine::MaxQueueSize() const noexcept {
    return 200;
}

int32_t Engine::Id() const noexcept {
    return id_;
}

}
