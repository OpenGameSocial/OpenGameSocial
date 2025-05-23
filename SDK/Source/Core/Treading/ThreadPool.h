#pragma once

#include <condition_variable>
#include <functional>
#include <thread>

#include "Core/Common/Containers/NonBlockingQueue.h"


namespace OGS::Threading
{
    enum class ETaskThread : uint8_t
    {
        MainThread,
        ThreadPool,
        AnyThread
    };

    static constexpr int32_t MainThreadIndex = -2;
    static constexpr int32_t UnknownThreadIndex = -2;

    class CThreadPool final
    {
    public:
        static CThreadPool& Get();

        ~CThreadPool();

        [[nodiscard]] bool IsCurrentThreadMain() const
        {
            return std::this_thread::get_id() == MainThread;
        }

        [[nodiscard]] int32_t GetCurrentThreadIndex() const
        {
            if (IsCurrentThreadMain())
            {
                return MainThreadIndex;
            }

            const auto CurrentThreadId = std::this_thread::get_id();

            for (size_t i = 0; i < ThreadIds.size(); ++i)
            {
                if (ThreadIds[i] == CurrentThreadId)
                {
                    return static_cast<int32_t>(i);
                }
            }

            return UnknownThreadIndex;
        }

        bool Init(uint32_t ThreadsCount);

        void Tick();

        void AddTask(ETaskThread Thread, std::function<void()>&& InTask);

    private:
        void ThreadRoutine(int32_t ThreadIndex);

    private:
        using CThreadTaskList = TNonBlockingQueue<std::function<void()>>;

        bool Initialized = false;

        std::thread::id MainThread;

        std::atomic<bool> IsRunning = true;

        CThreadTaskList MainThreadTasks;
        CThreadTaskList SharedTasks;
        CThreadTaskList PoolTasks;

        std::vector<std::thread> Threads;
        std::vector<std::thread::id> ThreadIds;

        std::mutex ConditionMutex;
        std::condition_variable Condition;
    };

    void EnqueueTask(ETaskThread Thread, std::function<void()>&& InTask);

    bool IsMainThread();

    int32_t GetCurrentThreadIndex();
}
