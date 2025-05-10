#pragma once

#include <unordered_map>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Core/Common/Guid.h"


namespace OGS::Http
{
    class CHttpManager final
    {
    public:
        static CHttpManager& Get();

        void CompleteRequest(const CGuid& InGuid, CHttpResponse&& Response);
        void AddRequest(const std::shared_ptr<CHttpRequest>& Request);

    private:
        std::unordered_map<CGuid, std::shared_ptr<CHttpRequest>> Requests;
    };
}
