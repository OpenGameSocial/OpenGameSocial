#pragma once

#include <nlohmann/json.hpp>

#include "HttpRequest.h"


namespace OGS::Http
{
    struct CJsonMode
    {
    public:
        using CHeaders = std::vector<std::pair<std::string, std::string>>;

        template <typename TBody>
        static void SetupRequest(CHttpRequest& Request, const TBody& Body)
        {
            nlohmann::json Json = Body;

            Request.SetHeader("Content-Type", "application/json");
            Request.SetBody(Json.dump());
        }
    };

    template <typename T, EHttpMethod Method, typename TUrlProvider, typename TMode = CJsonMode>
    typename THttpRequest<T>::CCompleteDelegate& SendRequest(const typename THttpRequest<T>::CRequest& Request = {})
    {
        auto HttpRequest = THttpRequest<T>::CreateRequest();
        HttpRequest->SetMethod(Method);
        HttpRequest->SetUrl(TUrlProvider::template GetUrlPath<T>());
        TMode::SetupRequest(*HttpRequest, Request);

        HttpRequest->Run();

        return HttpRequest->OnComplete();
    }

    template <typename T, typename TUrlProvider, typename TMode = CJsonMode>
    typename THttpRequest<T>::CCompleteDelegate& Get(const typename THttpRequest<T>::CRequest& Request = {})
    {
        return SendRequest<T, EHttpMethod::Get, TUrlProvider, TMode>(Request);
    }

    template <typename T, typename TUrlProvider, typename TMode = CJsonMode>
    typename THttpRequest<T>::CCompleteDelegate& Post(const typename THttpRequest<T>::CRequest& Request = {})
    {
        return SendRequest<T, EHttpMethod::Post, TUrlProvider, TMode>(Request);
    }

    template <typename T, typename TUrlProvider, typename TMode = CJsonMode>
    typename THttpRequest<T>::CCompleteDelegate& Put(const typename THttpRequest<T>::CRequest& Request = {})
    {
        return SendRequest<T, EHttpMethod::Put, TUrlProvider, TMode>(Request);
    }

    template <typename T, typename TUrlProvider, typename TMode = CJsonMode>
    typename THttpRequest<T>::CCompleteDelegate& Delete(const typename THttpRequest<T>::CRequest& Request = {})
    {
#undef DELETE
        return SendRequest<T, EHttpMethod::Delete, TUrlProvider, TMode>(Request);
    }

    template <typename T, typename TUrlProvider, typename TMode = CJsonMode>
    typename THttpRequest<T>::CCompleteDelegate& Patch(const typename THttpRequest<T>::CRequest& Request = {})
    {
        return SendRequest<T, EHttpMethod::Patch, TUrlProvider, TMode>(Request);
    }
}
