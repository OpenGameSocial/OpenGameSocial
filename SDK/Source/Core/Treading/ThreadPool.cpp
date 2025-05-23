#include "ThreadPool.h"

#include <thread>


using namespace OGS::Threading;

CThreadPool& CThreadPool::Get()
{
    static CThreadPool Instance;
    return Instance;
}

CThreadPool::~CThreadPool()
{
    IsRunning = false;
    Condition.notify_all();

    for (auto& Thread : Threads)
    {
        if (Thread.joinable())
        {
            Thread.join();
        }
    }

    Threads.clear();
}

bool CThreadPool::Init(uint32_t ThreadsCount)
{
    if (Initialized)
    {
        return false;
    }

    Initialized = true;

    MainThread = std::this_thread::get_id();

    for (uint32_t i = 0; i < ThreadsCount; ++i)
    {
        const auto& Thread = Threads.emplace_back(&CThreadPool::ThreadRoutine, this, i);
        ThreadIds.emplace_back(Thread.get_id());
    }

    return true;
}

void CThreadPool::Tick()
{
    std::function<void()> Task;

    while (MainThreadTasks.TryDequeue(Task))
    {
        Task();
    }

    while (SharedTasks.TryDequeue(Task))
    {
        Task();
    }
}

void CThreadPool::AddTask(ETaskThread Thread, std::function<void()>&& InTask)
{
    if (Thread == ETaskThread::MainThread)
    {
        MainThreadTasks.Enqueue(std::move(InTask));
        return;
    }

    if (Thread == ETaskThread::ThreadPool)
    {
        PoolTasks.Enqueue(std::move(InTask));
        Condition.notify_one();
        return;
    }

    SharedTasks.Enqueue(std::move(InTask));
    Condition.notify_one();
}

void CThreadPool::ThreadRoutine(int32_t ThreadIndex)
{
    std::function<void()> Task;

    while (IsRunning)
    {
        {
            std::unique_lock Lock(ConditionMutex);

            Condition.wait(Lock);
        }

        while (IsRunning && PoolTasks.TryDequeue(Task))
        {
            Task();
        }

        while (IsRunning && SharedTasks.TryDequeue(Task))
        {
            Task();
        }
    }
}

void OGS::Threading::EnqueueTask(ETaskThread Thread, std::function<void()>&& InTask)
{
    CThreadPool::Get().AddTask(Thread, std::move(InTask));
}

bool OGS::Threading::IsMainThread()
{
    return CThreadPool::Get().IsCurrentThreadMain();
}

int32_t OGS::Threading::GetCurrentThreadIndex()
{
    return CThreadPool::Get().GetCurrentThreadIndex();
}
