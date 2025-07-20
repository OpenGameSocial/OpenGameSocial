#pragma once

#include "Config.h"
#include "Core/Http/Http.h"
#include "Services/Account/AccountService.h"
#include "Services/ServiceContainer.h"


namespace OGS::Backend
{
    struct CBackendUrlProvider
    {
    public:
        template <typename T>
        static std::string GetUrlPath()
        {
            static std::string UrlBase = GetUrlBase();

            return UrlBase + T::Endpoint;
        }

    private:
        static std::string GetUrlBase()
        {
            std::string Result = CConfig::BackendUrl;

            if (Result.ends_with('/'))
            {
                return Result;
            }

            return Result + '/';
        }
    };

    template <typename TMode = Http::CJsonMode>
    struct TAuthenticatedMode
    {
    public:
        template <typename TBody>
        static void SetupRequest(OGS::Http::CHttpRequest& Request, const TBody& Body)
        {
            using namespace OGS::Services;

            static auto Service = CServiceContainer::GetServiceWeak<Account::CAccountService>();
            static const std::string HeaderName = "Authorization";

            TMode::SetupRequest(Request, Body);

            if (const auto ServicePtr = Service.lock())
            {
                Request.SetHeader(HeaderName, ServicePtr->GetToken());
            }
        }
    };

    using CAuthenticatedMode = TAuthenticatedMode<>;
}