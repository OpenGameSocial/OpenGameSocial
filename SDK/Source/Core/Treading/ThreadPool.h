#pragma once

#include <condition_variable>
#include <functional>
#include <thread>


namespace OGS::Threading
{
    namespace Details
    {
        struct SThreadTask final
        {
        public:
            explicit SThreadTask(std::function<void()>&& InTask)
                : Task(std::move(InTask))
            {
            }

            ~SThreadTask() = default;

            SThreadTask(const SThreadTask& other) = delete;
            SThreadTask(SThreadTask&& other) noexcept = delete;
            SThreadTask& operator=(const SThreadTask& other) = delete;
            SThreadTask& operator=(SThreadTask&& other) noexcept = delete;

        public:
            std::atomic<SThreadTask*> Next;
            std::function<void()> Task;
        };

        class CThreadTaskList final
        {
        public:
            CThreadTaskList() = default;

            CThreadTaskList(const CThreadTaskList& other) = delete;
            CThreadTaskList(CThreadTaskList&& other) noexcept = delete;
            CThreadTaskList& operator=(const CThreadTaskList& other) = delete;
            CThreadTaskList& operator=(CThreadTaskList&& other) noexcept = delete;

            void AddTask(std::function<void()>&& InTask)
            {
                auto* NewTask = new SThreadTask(std::move(InTask));
                NewTask->Next.store(nullptr);

                auto* OldTail = Tail.load(std::memory_order_relaxed);

                while (true)
                {
                    if (OldTail == nullptr)
                    {
                        if (Tail.compare_exchange_weak(OldTail, NewTask,
                                                       std::memory_order_release,
                                                       std::memory_order_relaxed))
                        {
                            Head.store(NewTask, std::memory_order_release);
                            return;
                        }

                        continue;
                    }

                    auto* Next = OldTail->Next.load();
                    if (Next == nullptr)
                    {
                        if (OldTail->Next.compare_exchange_weak(Next, NewTask,
                                                                std::memory_order_release,
                                                                std::memory_order_relaxed))
                        {
                            Tail.compare_exchange_weak(OldTail, NewTask,
                                                       std::memory_order_release,
                                                       std::memory_order_relaxed);
                            return;
                        }
                    }
                    else
                    {
                        Tail.compare_exchange_weak(OldTail, Next,
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed);
                    }

                    OldTail = Tail.load(std::memory_order_relaxed);
                }
            }

            bool TryTakeTask(std::function<void()>& OutTask)
            {
                while (true)
                {
                    auto* OldHead = Head.load(std::memory_order_relaxed);

                    if (OldHead == nullptr)
                    {
                        return false;
                    }

                    auto* Next = OldHead->Next.load(std::memory_order_acquire);

                    if (Head.compare_exchange_weak(OldHead, Next,
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed))
                    {
                        OutTask = std::move(OldHead->Task);

                        if (Next == nullptr)
                        {
                            Tail.compare_exchange_weak(OldHead, nullptr,
                                                       std::memory_order_release,
                                                       std::memory_order_relaxed);
                        }

                        delete OldHead;

                        return true;
                    }
                }
            }

            ~CThreadTaskList()
            {
                auto* Task = Head.load();

                while (Task)
                {
                    auto* Temp = Task;
                    Task = Task->Next.load();
                    delete Temp;
                }
            }

        private:
            std::atomic<SThreadTask*> Head;
            std::atomic<SThreadTask*> Tail;
        };
    }

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
        bool Initialized = false;

        std::thread::id MainThread;

        std::atomic<bool> IsRunning = true;

        Details::CThreadTaskList MainThreadTasks;
        Details::CThreadTaskList SharedTasks;
        Details::CThreadTaskList PoolTasks;

        std::vector<std::thread> Threads;
        std::vector<std::thread::id> ThreadIds;

        std::mutex ConditionMutex;
        std::condition_variable Condition;
    };

    void EnqueueTask(ETaskThread Thread, std::function<void()>&& InTask);

    bool IsMainThread();

    int32_t GetCurrentThreadIndex();
}
