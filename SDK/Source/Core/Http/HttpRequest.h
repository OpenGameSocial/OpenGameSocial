#pragma once

#include <string>

#include "Http.h"
#include "HttpResponse.h"
#include "Platform.h"
#include "Core/Common/Delegate.h"
#include "Core/Common/Guid.h"


namespace OGS::Http
{
    using CHttpResponseDelegate = TDelegate<const CHttpResponse&>;

    class CHttpRequest final : public std::enable_shared_from_this<CHttpRequest>
    {
    public:
        CHttpRequest(const CHttpRequest&) = delete;
        CHttpRequest& operator=(const CHttpRequest&) = delete;

        ~CHttpRequest() = default;

        static std::shared_ptr<CHttpRequest> CreateRequest();

        void SetUrl(const Uri& Url);
        void SetMethod(EHttpMethod Method);

        void SetHeader(const std::string& Key, const std::string& Value);

        void SetBody(const std::string& Body);

        void SetOnCompleted(const CHttpResponseDelegate& InDelegate)
        {
            OnComplete = InDelegate;
        }

        const CGuid& GetGuid() const
        {
            return Guid;
        }

        bool Run();

    private:
        CHttpRequest();

        void Complete(CHttpResponse&& Response);

    private:
        CGuid Guid;
        CHttpResponseDelegate OnComplete;
        CPlatformHttpRequest PlatformRequest;

        friend class CHttpManager;
    };
}
