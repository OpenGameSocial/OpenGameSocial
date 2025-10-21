#pragma once

#include <memory>
#include <string>

#include "Core/Common/Guid.h"
#include "Core/Http/HttpEnums.h"
#include "Core/Http/Uri.h"

#include "Core/Http/HttpResponse.h"


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
                return bIsCompleted;
            }

            bool Run(const std::shared_ptr<CHttpRequest>& Owner);

        private:
            CCurlHttpRequest();

        private:
            bool bIsCompleted = false;

            CGuid Guid;

            Uri Url;
            std::wstring Method;
            std::wstring Headers;
            std::string Body;

            int32_t ResponseCode = 0;
            std::string ResponseBody;

            friend std::shared_ptr<CCurlHttpRequest>;
        };
    }

    using CPlatformHttpRequest = Linux::CCurlHttpRequest;
}
