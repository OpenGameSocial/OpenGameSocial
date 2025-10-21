#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "Core/Common/Guid.h"
#include "Core/Http/HttpEnums.h"
#include "Core/Http/Uri.h"

#include <curl/curl.h>


class CCurlMultiManager;

namespace OGS::Http
{
    class CHttpRequest;

    namespace Linux
    {
        class CCurlHttpRequest final
        {
        public:
            explicit CCurlHttpRequest(const CGuid& InGuid);
            ~CCurlHttpRequest();

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
                return bIsCompleted.load();
            }

            bool Run(const std::shared_ptr<CHttpRequest>& Owner);

        private:
            static size_t WriteCallback(void* InBuffer, size_t InSize, size_t InItemsCount, void* InUserData);
            static size_t HeaderCallback(char* InBuffer, size_t InSize, size_t InItemsCount, void* InUserData);

            void OnCompleted(CURLcode Result, long http_code, const std::string& ErrorString);

            static std::string GetFromCurlError(CURLcode InCode);

        private:
            std::atomic<bool> bIsCompleted{false};

            CGuid Guid;

            Uri Url;
            std::string Method;
            std::vector<std::string> HeadersList;
            std::string Body;

            int32_t ResponseCode = 0;
            std::string ResponseBody;
            std::string ResponseHeaders;

            CURL* EasyHandle = nullptr;

            friend class ::CCurlMultiManager;
            friend std::shared_ptr<CCurlHttpRequest>;
        };
    }

    using CPlatformHttpRequest = Linux::CCurlHttpRequest;
}
