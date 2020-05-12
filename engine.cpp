#include "engine.h"
#include <cstdio>

namespace df {

void Engine::Init() {
    std::thread t(Entry, this);
    thread_ = std::move(t);
    thread_.join();
}

void Engine::SetFunctor(std::function<Task(const Task&)> f) noexcept {
    functor_ = f;
}

void Engine::Entry() {
    for (;;) {
        if (stop_) break;

        std::unique_lock<std::mutex> lk;
        cv_.wait(lk, );

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

bool Engine::SetCore(int core) {
    unsigned int n = std::thread::hardware_concurrency();
    if (core >= n) {
        printf("bind core index: %d exceed max core index %u\n", core, n - 1);
        return false;
    }
    cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
	if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
		perror("pthread_setaffinity_np");
}

bool Engine::SetPriority() {

}

int Engine::Core() const noexcept {


    return 0;
}
int Engine::Priority() const noexcept {

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

}
