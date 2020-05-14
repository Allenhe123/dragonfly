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
    Engine(int id, int priority, const std::string& policy, 
           const std::string& affi, const std::vector<int>& cpus, int threadnum);
    virtual ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator = (const Engine&) = delete;

    void Init();
    void Stop() noexcept;
    void NotifyOne();
    
    void Push(const Task& data);

    const std::vector<int32_t>& Cores() const noexcept;
    int32_t Priority() const noexcept;
    const std::string& Policy() const noexcept;

    void SetChild(const std::shared_ptr<Engine>& ch) noexcept;
    void SetParent(const std::shared_ptr<Engine>& pa) noexcept;
    const std::shared_ptr<Engine>& Child() const noexcept;
    const std::shared_ptr<Engine>& Parent() const noexcept;
    int32_t Id() const noexcept;

    void SetFunctor(std::function<Task(const Task&)> f) noexcept;

    uint32_t CurrentQueueSize() const noexcept;
    uint32_t MaxQueueSize() const noexcept;

    bool SetSchedAffinity();
    bool SetSchedPolicy();

protected:
    void Entry();
    void NotifyAll();

protected:
    std::thread thread_;
    TaskQueue queue_;
    std::condition_variable cv_;
    std::mutex mutex_;
    std::shared_ptr<Engine> parent_ = nullptr;
    std::shared_ptr<Engine> child_ = nullptr;
    bool stop_ = false;
    FUNCTOR functor_;
    int32_t id_ = -1;
    int32_t priority_ = -1;
    std::string policy_ = "";
    std::string cpu_affi_ = "";
    std::vector<int32_t> cpus_;
    int32_t thread_num_ = 1;
    std::atomic<pid_t> tid_{-1};
    std::atomic<bool> running_{false};
};

}

#endif