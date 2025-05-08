#pragma once

#include <string>

#include "Platform.h"


namespace OGS::Http
{
    class CHttpRequest final
    {
    public:
        CHttpRequest(const CHttpRequest&) = delete;
        CHttpRequest& operator=(const CHttpRequest&) = delete;

        ~CHttpRequest() = default;

        static CHttpRequest CreateRequest();

        void SetUrl(const std::wstring& url);
        void SetUrl(std::wstring&& url);
    
    private:
        CHttpRequest();

    private:
        // CPlatformHttpRequest PlatformRequest;
    };
}
