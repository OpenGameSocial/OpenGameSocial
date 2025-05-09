#include "WinHttpManager.h"

#include "Core/Treading/ThreadPool.h"


OGS::Http::Win64::CWinHttpManager& OGS::Http::Win64::CWinHttpManager::GetInstance()
{
    static CWinHttpManager Instance;
    return Instance;
}

void OGS::Http::Win64::CWinHttpManager::AddRequest(const std::shared_ptr<CWinHttpRequest>& Request)
{
    auto Lambda = [this, Request]
    {
        RunningRequests.emplace(Request);
    };

    if (Threading::IsMainThread())
    {
        Lambda();
        return;
    }

    Threading::EnqueueTask(Threading::ETaskThread::MainThread, Lambda);
}
