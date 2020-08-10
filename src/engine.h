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


struct Publisher {
    uint32_t remote_queue_idx;
    std::string remote_ip;
    uint32_t remote_port;
};

struct Recipient {
    uint32_t local_queue_idx;
    uint32_t listen_port;
};

using PublisherPtr = std::shared_ptr<Publisher>;
using RecipientPtr = std::shared_ptr<Recipient>;


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
    
    // @queueIdx - each parent has a input, each parent has a queue
    void Push(int32_t queueIdx, const Task& data);

    // const std::vector<int32_t>& Cores() const noexcept;
    // int32_t Priority() const noexcept;
    // const std::string& Policy() const noexcept;

    bool SetChild(int idx, const std::shared_ptr<Engine> ch) noexcept;
    // bool SetParent(int idx, const std::shared_ptr<Engine> pa) noexcept;
    const std::shared_ptr<Engine> Child(int idx) const noexcept;
    // const std::shared_ptr<Engine> Parent(int idx) const noexcept;
    int32_t Id() const noexcept;
    uint32_t InputNum() const noexcept { return parent_num_; }
    uint32_t OutputNum() const noexcept { return child_num_; }
    void AddConn(uint32_t child_idx, uint32_t child_queue_idx) noexcept {
        intra_conns_[child_idx] = child_queue_idx;
    }

    void AddPublisher(uint32_t qidx, const std::string& ip, uint32_t port) noexcept;
    void AddRecipient(uint32_t qidx, uint32_t port) noexcept;

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
    std::condition_variable cv_;
    std::mutex mutex_;

    uint32_t parent_num_;
    uint32_t child_num_;
    
    // intra
    // std::vector<std::shared_ptr<Engine>> parents_;
    std::vector<std::shared_ptr<Engine>> childs_;
    std::map<uint32_t, uint32_t> intra_conns_;

    std::vector<PublisherPtr> publishers_;
    std::vector<RecipientPtr> recipients_;

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
