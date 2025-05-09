#pragma once

#include <memory>
#include <mutex>
#include <unordered_set>

#include "WinHttpRequest.h"


namespace OGS::Http::Win64
{
    class CWinHttpManager
    {
    public:
        static CWinHttpManager& GetInstance();

        void AddRequest(const std::shared_ptr<CWinHttpRequest>& Request);

    private:
        std::mutex RequestsMutex;
        std::unordered_set<std::shared_ptr<CWinHttpRequest>> RunningRequests;
    };
}
