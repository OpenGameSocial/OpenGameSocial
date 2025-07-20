#include "OpenGameSocial.h"

#include "WeatherModels.h"
#include "Core/Threading/ThreadPool.h"

#include "Core/Http/HttpManager.h"
#include "Core/Logging/Logger.h"
#include "Services/ServiceContainer.h"


static OGS::TLogCategory LogOpenGameSocial("LogOpenGameSocial");

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
    nlohmann::json j = RequestModel;

    const auto Request = OGS::Http::THttpRequest<CWeather>::CreateRequest();
    Request->SetUrl(std::string("http://localhost:5211/WeatherForecast"));
    Request->SetMethod(OGS::Http::EHttpMethod::POST);
    Request->SetHeader("Content-Type", "application/json");
    Request->SetBody(j.dump());
    Request->OnComplete().BindStatic(&OnHttpRequestCompleted);
    Request->Run();

    RunAutoInit();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
    OGS::Services::CServiceContainer::Get().Init();
}

void OGS_SetLogger(OGS_ELogLevel MinLevel, bool bThreadSafe, OGS_LogCallback Callback)
{
    OGS::CLogger::Get().Init(MinLevel, bThreadSafe, Callback);
}
