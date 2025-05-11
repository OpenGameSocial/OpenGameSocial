#include "OpenGameSocial.h"

#include "Core/Treading/ThreadPool.h"

#include "Core/Http/HttpManager.h"


static void OnHttpRequestCompleted(const OGS::Http::CHttpResponse& Resp)
{
    printf("Received http response [%i]:\n", Resp.GetCode());
    printf("%s\n", Resp.GetResult().c_str());
}

void OGS_Init(const OGS_Init_Options* Options)
{
    const auto Request = OGS::Http::CHttpRequest::CreateRequest();
    Request->SetUrl("http://localhost:5211/WeatherForecast");
    Request->SetMethod(OGS::Http::EHttpMethod::GET);
    Request->SetOnCompleted(OGS::Http::CHttpResponseDelegate::CreateStatic(OnHttpRequestCompleted));
    Request->Run();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
}
