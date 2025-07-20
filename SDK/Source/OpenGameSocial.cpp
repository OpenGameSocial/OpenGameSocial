#include "OpenGameSocial.h"

#include "TestAuthModels.h"
#include "WeatherModels.h"
#include "Core/Http/Http.h"
#include "Core/Threading/ThreadPool.h"

#include "Core/Http/HttpManager.h"
#include "Core/Logging/Logger.h"
#include "Services/ServiceContainer.h"
#include "Services/Accounts/AccountService.h"


static OGS::TLogCategory LogOpenGameSocial("LogOpenGameSocial");

struct CBackendUrlProvider
{
public:
    template <typename T>
    static std::string GetUrlPath()
    {
        static std::string UrlBase = "http://localhost:5211/";

        return UrlBase + T::Endpoint;
    }
};

template <typename TMode = OGS::Http::CJsonMode>
struct TAuthenticatedMode
{
public:
    template <typename TBody>
    static void SetupRequest(OGS::Http::CHttpRequest& Request, const TBody& Body)
    {
        using namespace OGS::Services;

        static auto Service = CServiceContainer::GetServiceWeak<Accounts::CAccountService>();
        static const std::string HeaderName = "Authorization";

        TMode::SetupRequest(Request, Body);

        if (const auto ServicePtr = Service.lock())
        {
            Request.SetHeader(HeaderName, ServicePtr->GetToken());
        }
    }
};

using CAuthenticatedMode = TAuthenticatedMode<>;

static void OnHttpRequestCompleted(OGS::Http::THttpResponse<CWeather>&& Resp)
{
    printf("Received http response [%i]:\n", Resp.GetCode());

    if (Resp.GetCode() <= 0)
    {
        return;
    }

    printf("Today's weather:\n");

    const auto& Weather = Resp->Data;

    for (const auto& Data : Weather)
    {
        printf("=====================================\n");

        printf("Date: %s\n", Data.Date.c_str());
        printf("Temperature celsius: %i\n", Data.TemperatureC);
        printf("Temperature fahrenheit: %i\n", Data.TemperatureF);

        if (Data.Summary)
        {
            printf("Summary: %s\n", Data.Summary.value().c_str());
        }
    }
}

static void OnTestAuthCompleted(OGS::Http::THttpResponse<CTestAuth>&& Resp)
{
    printf("Received http response [%i]:\n", Resp.GetCode());

    if (Resp.GetCode() <= 0)
    {
        return;
    }

    if (Resp.GetCode() != 200)
    {
        printf("%s\n", Resp.GetResultStr().c_str());
        return;
    }

    printf("Auth message: %s\n", Resp->Message.c_str());
}

void OGS_Init(const OGS_Init_Options* Options)
{
    LogOpenGameSocial.Verbose("Initializing OpenGameSocial: %i", Options->ThreadPoolSize);

    RunAutoInit();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
    OGS::Services::CServiceContainer::Get().Init();

    OGS::Http::Post<CWeather, CBackendUrlProvider>({
        .Count = 256
    }).BindStatic(&OnHttpRequestCompleted);

    OGS::Http::Get<CTestAuth, CBackendUrlProvider, CAuthenticatedMode>({})
        .BindStatic(&OnTestAuthCompleted);
}

void OGS_SetLogger(OGS_ELogLevel MinLevel, bool bThreadSafe, OGS_LogCallback Callback)
{
    OGS::CLogger::Get().Init(MinLevel, bThreadSafe, Callback);
}
