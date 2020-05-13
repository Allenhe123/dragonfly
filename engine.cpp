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
    std::thread t(Entry, this);
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

void Engine::SetChild(Engine* ch) noexcept { child_ = ch; }
void Engine::SetParent(Engine* pa) noexcept { parent_ = pa; }
Engine* Engine::Child() const noexcept { return child_; }
Engine* Engine::Parent() const noexcept { return parent_; }

bool Engine::SetPolicy(int policy) {
    pthread_attr_t attr;
	int rs = pthread_attr_init(&attr);
    int rs = pthread_attr_setschedpolicy(&attr, policy);

}

bool Engine::SetSchedAffinity(const std::vector<int>& cpus, const  std::string& affinity, int p) {
    cpu_set_t set;
    CPU_ZERO(&set);
    if (cpus.size()) {
        if (!affinity.compare("range")) {
            for (const auto cpu : cpus) {
                CPU_SET(cpu, &set);
            }
            pthread_setaffinity_np(thread_.native_handle(), sizeof(set), &set);
        } else if (!affinity.compare("1to1")) {
            CPU_SET(cpus[p], &set);
            pthread_setaffinity_np(thread_.native_handle(), sizeof(set), &set);
    }
  }
}

bool Engine::SetSchedPolicy(const std::string& spolicy, int priority) {
    struct sched_param sp;
    int policy;
    memset(reinterpret_cast<void *>(&sp), 0, sizeof(sp));
    sp.sched_priority = priority;

    if (!spolicy.compare("SCHED_FIFO")) {
        policy = SCHED_FIFO;
        pthread_setschedparam(thread_.native_handle(), policy, &sp);
    } else if (!spolicy.compare("SCHED_RR")) {
        policy = SCHED_RR;
        pthread_setschedparam(thread_.native_handle(), policy, &sp);
    } else if (!spolicy.compare("SCHED_OTHER")) {
        // Set normal thread nice value.
        while (tid_.load() == -1) {
            cpu_relax();
        }
        setpriority(PRIO_PROCESS, tid_.load(), priority);
    }

    policy_ = policy;
    priority_ = priority;
}

int Engine::Priority() noexcept {
    if (priority_ != -1) return priority_;
    int policy;
	sched_param param;
	bzero((void*)&param, sizeof(param));
	pthread_getschedparam(thread_.native_handle(),&policy, &param);
    priority_ = param.sched_priority;
    policy_ = policy;
    return priority_;
}

int Engine::Policy() noexcept {
    if (priority_ != -1) return priority_;
    int policy;
	sched_param param;
	bzero((void*)&param, sizeof(param));
	pthread_getschedparam(thread_.native_handle(),&policy, &param);
    priority_ = param.sched_priority;
    policy_ = policy;
    return policy_;
}


int Engine::Core() const noexcept {


    return 0;
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

}
