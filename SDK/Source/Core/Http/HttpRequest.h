#pragma once

#include <string>

#include "HttpEnums.h"
#include "HttpResponse.h"
#include "Platform.h"
#include "Core/Common/Delegate.h"
#include "Core/Common/Guid.h"
#include "Core/Http/Uri.h"


namespace OGS::Http
{
    using CHttpResponseDelegate = TDelegate<CHttpResponse&&>;

    class CHttpRequest : public std::enable_shared_from_this<CHttpRequest>
    {
    public:
        CHttpRequest(const CHttpRequest&) = delete;
        CHttpRequest& operator=(const CHttpRequest&) = delete;

        virtual ~CHttpRequest() = default;

        static std::shared_ptr<CHttpRequest> CreateRequest();

        void SetUrl(const Uri& Url);
        void SetMethod(EHttpMethod Method);

        void SetHeader(const std::string& Key, const std::string& Value);

        void SetBody(const std::string& Body);

        CHttpResponseDelegate& OnComplete()
        {
            return OnCompleteDelegate;
        }

        const CGuid& GetGuid() const
        {
            return Guid;
        }

        bool Run();

    protected:
        CHttpRequest();

    private:
        void Complete(CHttpResponse&& Response);

    private:
        CGuid Guid;
        CHttpResponseDelegate OnCompleteDelegate;
        CPlatformHttpRequest PlatformRequest;

        friend class CHttpManager;
    };

    template <typename T>
    class THttpRequest : public CHttpRequest
    {
    public:
        using CRequest = typename T::CRequest;
        using CResponse = typename T::CResponse;
        using CCompleteDelegate = TDelegate<THttpResponse<T>&&>;

        static std::shared_ptr<THttpRequest> CreateRequest()
        {
            auto Result = std::make_shared<THttpRequest>();
            Result->Init();

            return Result;
        }

        CCompleteDelegate& OnComplete()
        {
            return Delegate;
        }

    private:
        void Init()
        {
            auto SharedThis = std::static_pointer_cast<THttpRequest>(shared_from_this());
            CHttpRequest::OnComplete().BindShared(SharedThis, &THttpRequest::OnRequestComplete);
        }

        void OnRequestComplete(CHttpResponse&& Response)
        {
            THttpResponse<T> HttpResponse(std::move(Response));
            Delegate.Execute(std::move(HttpResponse));
        }

    private:
        CCompleteDelegate Delegate;
    };
} // namespace OGS::Http
