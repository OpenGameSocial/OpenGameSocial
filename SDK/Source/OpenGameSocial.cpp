#include "OpenGameSocial.h"

#include "Core/Treading/ThreadPool.h"

#include "Core/Http/HttpManager.h"


static void OnHttpRequestCompleted(const OGS::Http::CHttpResponse& Resp)
{
    DEBUG_BREAK();
}

void OGS_Init(const OGS_Init_Options* Options)
{
    auto Request = OGS::Http::CHttpRequest::CreateRequest();
    Request->SetUrl("https://httpbin.org/get");
    Request->SetMethod(OGS::Http::EHttpMethod::GET);
    Request->SetOnCompleted(OGS::Http::CHttpResponseDelegate::CreateStatic(OnHttpRequestCompleted));
    Request->Run();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
}
