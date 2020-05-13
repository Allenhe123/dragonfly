#ifndef INC_DF_ENGINE_H_
#define INC_DF_ENGINE_H_

#include <atomic>
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
using FUNCTOR = std::function<Task(const Task&)>;

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
    virtual ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator = (const Engine&) = delete;

    void Init();
    void Stop() noexcept;
    void NotifyOne();
    
    void Push(const Task& data);

    int Core() const;
    int Priority() noexcept;
    int Policy() noexcept;

    void SetChild(Engine* ch) noexcept;
    void SetParent(Engine* pa) noexcept;
    Engine* Child() const noexcept;
    Engine* Parent() const noexcept;

    void SetFunctor(std::function<Task(const Task&)> f) noexcept;

    uint32_t CurrentQueueSize() const noexcept;
    uint32_t MaxQueueSize() const noexcept;

    bool SetSchedAffinity(const std::vector<int>& cpus, const  std::string& affinity, int p);
    bool SetSchedPolicy(const std::string& policy, int priority);
    bool SetPolicy(int policy);

protected:
    void Entry();
    void NotifyAll();

protected:
    std::thread thread_;
    TaskQueue queue_;
    std::condition_variable cv_;
    std::mutex mutex_;
    Engine* parent_ = nullptr;
    Engine* child_ = nullptr;
    bool stop_ = false;
    FUNCTOR functor_;
    int priority_ = -1;
    int policy_ = -1;
    std::atomic<pid_t> tid_{-1};
    std::atomic<bool> running_{false};
};

}

#endif