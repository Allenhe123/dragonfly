#ifndef INC_DF_ENGINE_H_
#define INC_DF_ENGINE_H_

#include <queue>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

namespace df {

using Task = std::shared_ptr<void>;
using TaskQueue = std::queue<Task>;

// struct Entity
// {
//     std::condition_variable cv_;
//     std::mutex mutex_;
//     TaskQueue queue_;
// };

// using Queue = std::unordered_map<std::string, Entity>;


class Engine
{
public:
    Engine() = default;
    ~Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator = (const Engine&) = delete;

    void Init();
    void Stop() noexcept;
    void NotifyOne();
    void NotifyAll();
    void Push(const Task& data);

    int Core() const;
    int Priority() const;

    void SetChild(Engine* ch) noexcept { child_ = ch; }
    void SetParent(Engine* pa) noexcept { parent_ = pa; }
    Engine* Child() const noexcept { return child_; }
    Engine* Parent() const noexcept { return parent_; }

    void SetFunctor(std::function<Task(const Task&)> f) noexcept;

protected:
    bool SetCore(int core);
    bool SetPriority();
    void Entry();

protected:
    std::thread thread_;
    TaskQueue queue_;
    std::condition_variable cv_;
    std::mutex mutex_;
    Engine* parent_ = nullptr;
    Engine* child_ = nullptr;
    bool stop_ = false;
    std::function<Task (const Task&)> functor_;
};

}

#endif