#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Core/Common/Guid.h"
#include "Core/Http/Http.h"
#include "Core/Http/Uri.h"

#include <windows.h>
#include <winhttp.h>

#include "Core/Http/HttpResponse.h"


namespace OGS::Http
{
    class CHttpRequest;
    
    namespace Win64
    {
        class CWinHttpRequest final
        {
        public:
            explicit CWinHttpRequest(const CGuid& InGuid);

            ~CWinHttpRequest();

            void SetUrl(const Uri& InUrl)
            {
                Url = InUrl;
            }

            void SetMethod(EHttpMethod InMethod);

            void SetHeader(const std::string& Key, const std::string& Value);

            void SetBody(const std::string& InBody)
            {
                Body = InBody;
            }

            [[nodiscard]] bool IsCompleted() const
            {
                return bIsCompleted;
            }

            bool Run(const std::shared_ptr<CHttpRequest>& Owner);

        private:
            CWinHttpRequest();

            static void WinHttpCallback(
                HINTERNET hInternet,
                DWORD_PTR dwContext,
                DWORD dwInternetStatus,
                LPVOID lpvStatusInformation,
                DWORD dwStatusInformationLength
            );

            static CHttpResponse GetFromWinError(DWORD dwErrorCode);

        private:
            bool bIsCompleted = false;

            CGuid Guid;

            HINTERNET SessionHandle = nullptr;
            HINTERNET ConnectHandle = nullptr;
            HINTERNET RequestHandle = nullptr;

            Uri Url;
            std::wstring Method;
            std::wstring Headers;
            std::string Body;

            int32_t ResponseCode = 0;
            std::string ResponseBody;

            friend std::shared_ptr<CWinHttpRequest>;
        };
    }

    using CPlatformHttpRequest = Win64::CWinHttpRequest;
}
