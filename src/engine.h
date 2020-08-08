#ifndef INC_DF_ENGINE_H_
#define INC_DF_ENGINE_H_

/*
each engine have multi inputs and multi outputs, 
each input corressponding to a parent,
each out corressponding to a child,
*/

#include <atomic>
#include <queue>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

#include "multitype_queue.h"

namespace df {

using Task = std::shared_ptr<void>;
using TaskVec = std::vector<std::shared_ptr<void>>;
using TaskQueue = std::queue<Task>;
using FUNCTOR = std::function<TaskVec(const TaskVec&)>;

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
    Engine(int id, int threadnum, int inputSize, int outputSize);
    virtual ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator = (const Engine&) = delete;

    void Init();
    void Stop() noexcept;
    void NotifyOne();
    
    void Push(int32_t queueIdx, const Task& data);

    // const std::vector<int32_t>& Cores() const noexcept;
    // int32_t Priority() const noexcept;
    // const std::string& Policy() const noexcept;

    bool SetChild(int idx, const std::shared_ptr<Engine> ch) noexcept;
    bool SetParent(int idx, const std::shared_ptr<Engine> pa) noexcept;
    const std::shared_ptr<Engine> Child(int idx) const noexcept;
    const std::shared_ptr<Engine> Parent(int idx) const noexcept;
    int32_t Id() const noexcept;

    void SetFunctor(std::function<TaskVec (const TaskVec&)> f) noexcept;

    // uint32_t CurrentQueueSize() const noexcept;
    // uint32_t MaxQueueSize() const noexcept;

    bool SetSchedAffinity(int32_t idx, const std::string& affinity, 
                          const std::vector<int>& cpus);
    bool SetSchedPolicy(int32_t idx, int pority, const std::string& policy);
    void Dump() const noexcept;

protected:
    void Entry();
    void NotifyAll();

protected:
    std::vector<std::thread> threads_;
    TaskQueue queue_;
    std::condition_variable cv_;
    std::mutex mutex_;
    std::vector<std::shared_ptr<Engine>> parents_;
    std::vector<std::shared_ptr<Engine>> childs_;
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

    MultiTypeQueue inQueue_;
    std::vector<std::shared_ptr<void>> args_;
};

}

#endif
