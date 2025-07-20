#include "OpenGameSocial.h"

#include "WeatherModels.h"
#include "Core/Http/Http.h"
#include "Core/Threading/ThreadPool.h"

#include "Core/Http/HttpManager.h"
#include "Core/Logging/Logger.h"
#include "Services/ServiceContainer.h"


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

void OGS_Init(const OGS_Init_Options* Options)
{
    LogOpenGameSocial.Verbose("Initializing OpenGameSocial: %i", Options->ThreadPoolSize);

    CWeather::CRequest RequestModel
    {
        .Count = 256
    };

    OGS::Http::Post<CWeather, CBackendUrlProvider>(RequestModel)
        .BindStatic(&OnHttpRequestCompleted);

    RunAutoInit();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
    OGS::Services::CServiceContainer::Get().Init();
}

void OGS_SetLogger(OGS_ELogLevel MinLevel, bool bThreadSafe, OGS_LogCallback Callback)
{
    OGS::CLogger::Get().Init(MinLevel, bThreadSafe, Callback);
}
