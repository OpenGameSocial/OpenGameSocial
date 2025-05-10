#include "HttpManager.h"

#include "Core/Treading/ThreadPool.h"


OGS::Http::CHttpManager& OGS::Http::CHttpManager::Get()
{
    static CHttpManager Instance;
    return Instance;
}

void OGS::Http::CHttpManager::CompleteRequest(const CGuid& InGuid, CHttpResponse&& Response)
{
    auto DoComplete = [this, InGuid, Response = std::move(Response)]() mutable
    {
        auto It = Requests.find(InGuid);

        if (It == Requests.end())
        {
            // TODO: logme
            return;
        }

        It->second->Complete(std::move(Response));
        Requests.erase(It);
    };

    Threading::EnqueueTask(Threading::ETaskThread::MainThread, DoComplete);
}

void OGS::Http::CHttpManager::AddRequest(const std::shared_ptr<CHttpRequest>& Request)
{
    // TODO: add main thread check
    Requests[Request->GetGuid()] = Request;
}
