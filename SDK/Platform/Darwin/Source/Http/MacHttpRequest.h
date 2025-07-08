#pragma once

#include <string>
#include <functional>

#include "Core/Common/Guid.h"
#include "Core/Http/Uri.h"
#include "Core/Http/Http.h"


namespace OGS::Http
{
    class CHttpRequest;
    enum class EHttpMethod;

    namespace Mac
    {
        class CMacHttpRequest final
        {
        public:
            explicit CMacHttpRequest(const CGuid& InGuid)
                : Guid(InGuid)
            {
                SetMethod(EHttpMethod::GET);
            }

            CMacHttpRequest(const CMacHttpRequest&) = delete;
            CMacHttpRequest& operator=(const CMacHttpRequest&) = delete;

            ~CMacHttpRequest() = default;

            void SetUrl(const Uri& InUrl)
            {
                Url = InUrl;
            }

            void SetMethod(EHttpMethod InMethod);

            void SetHeader(const std::string& Key, const std::string& Value)
            {
                Headers[Key] = Value;
            }

            void SetBody(const std::string& InBody)
            {
                Body = InBody;
            }

            bool Run(const std::shared_ptr<CHttpRequest>& Owner);

        private:
            bool Started = false;

            CGuid Guid;

            Uri Url;
            std::string Method;
            std::unordered_map<std::string, std::string> Headers;
            std::string Body;
        };
    }

    using CPlatformHttpRequest = Mac::CMacHttpRequest;
}
