#include "OpenGameSocial.h"

#include "Core/Treading/ThreadPool.h"

#include "Core/Http/HttpManager.h"
#include "Core/Logging/Logger.h"
#include "Services/ServiceContainer.h"
#include "Services/Accounts/AccountService.h"

#include <nlohmann/json.hpp>

#include "Services/Accounts/TestSerialization.h"


static OGS::TLogCategory LogOpenGameSocial("LogOpenGameSocial");

// void to_json(nlohmann::json& nlohmann_json_j, const SWeatherForecast& nlohmann_json_t)
// {
//     nlohmann_json_j["date"] = nlohmann_json_t.Date;
//     nlohmann_json_j["temperatureC"] = nlohmann_json_t.TemperatureC;
//     nlohmann_json_j["temperatureF"] = nlohmann_json_t.TemperatureF;
// }
//
// void from_json(const nlohmann::json& nlohmann_json_j, SWeatherForecast& nlohmann_json_t)
// {
//     nlohmann_json_j.at("date").get_to(nlohmann_json_t.Date);
//     nlohmann_json_j.at("temperatureC").get_to(nlohmann_json_t.TemperatureC);
//     nlohmann_json_j.at("temperatureF").get_to(nlohmann_json_t.TemperatureF);
// };
//
// void from_json(const nlohmann::json& input, ::SWeatherForecast& output)
// {
//     input.at("date").get_to(output.Date);
//     input.at("temperaturec").get_to(output.TemperatureC);
//     input.at("temperaturef").get_to(output.TemperatureF);
//
//     auto it = input.find("summary");
//     if (it != input.end())
//     {
//         using TestType = TFieldRealType<decltype(output.Summary)>::Type;
//         output.Summary = it->template get<TestType>();
//         // input.at("summary").get_to(output.Summary);
//     }
//     // input.at("summary").get_to(output.Summary);
// }

static void OnHttpRequestCompleted(const OGS::Http::CHttpResponse& Resp)
{
    printf("Received http response [%i]:\n", Resp.GetCode());
    auto j = nlohmann::json::parse(Resp.GetResult());
    std::vector<SWeatherForecast> Weather = j;
    printf("%s\n", Resp.GetResult().c_str());
}

void OGS_Init(const OGS_Init_Options* Options)
{
    auto& Services = OGS::Services::CServiceContainer::Get();
    Services.RegisterService<OGS::Services::Accounts::CAccountService>();
    Services.RegisterService<OGS::Services::Accounts::CTestService>();

    LogOpenGameSocial.Verbose("Initializing OpenGameSocial: %i", Options->ThreadPoolSize);
    LogOpenGameSocial.Info("Initializing OpenGameSocial: %i", Options->ThreadPoolSize);
    LogOpenGameSocial.Error("Initializing OpenGameSocial: %i", Options->ThreadPoolSize);
    // LogOpenGameSocial.Critical("Initializing OpenGameSocial: %i", Options->ThreadPoolSize);

    const auto Request = OGS::Http::CHttpRequest::CreateRequest();
    Request->SetUrl(std::string("http://localhost:5211/WeatherForecast"));
    Request->SetMethod(OGS::Http::EHttpMethod::GET);
    Request->SetOnCompleted(OGS::Http::CHttpResponseDelegate::CreateStatic(OnHttpRequestCompleted));
    Request->Run();

    CAutoInitRegistry Init;
    Init.RunInit();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
    OGS::Services::CServiceContainer::Get().Init();
}

void OGS_SetLogger(OGS_ELogLevel MinLevel, bool bThreadSafe, OGS_LogCallback Callback)
{
    OGS::CLogger::Get().Init(MinLevel, bThreadSafe, Callback);
}
