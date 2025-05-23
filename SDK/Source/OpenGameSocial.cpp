#include "OpenGameSocial.h"

#include "Core/Treading/ThreadPool.h"

#include "Core/Http/HttpManager.h"
#include "Core/Logging/Logger.h"


static void OnHttpRequestCompleted(const OGS::Http::CHttpResponse& Resp)
{
    printf("Received http response [%i]:\n", Resp.GetCode());
    printf("%s\n", Resp.GetResult().c_str());
}

void OGS_Init(const OGS_Init_Options* Options)
{
    OGS::Log(OGS_Verbose, "Initializing OpenGameSocial: %i", Options->ThreadPoolSize);
    OGS::Log(OGS_Info, "Initializing OpenGameSocial: %i", Options->ThreadPoolSize);
    OGS::Log(OGS_Warning, "Initializing OpenGameSocial: %i", Options->ThreadPoolSize);
    OGS::Log(OGS_Error, "Initializing OpenGameSocial: %i", Options->ThreadPoolSize);
    OGS::Log(OGS_Critical, "Initializing OpenGameSocial: %i", Options->ThreadPoolSize);

    const auto Request = OGS::Http::CHttpRequest::CreateRequest();
    Request->SetUrl("http://localhost:5211/WeatherForecast");
    Request->SetMethod(OGS::Http::EHttpMethod::GET);
    Request->SetOnCompleted(OGS::Http::CHttpResponseDelegate::CreateStatic(OnHttpRequestCompleted));
    Request->Run();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
}

void OGS_SetLogger(OGS_ELogLevel MinLevel, bool bThreadSafe, OGS_LogCallback Callback)
{
    OGS::CLogger::Get().Init(MinLevel, bThreadSafe, Callback);
}
