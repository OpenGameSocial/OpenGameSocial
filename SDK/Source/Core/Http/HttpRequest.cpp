#include "HttpRequest.h"

#include "HttpManager.h"

using namespace OGS::Http;


std::shared_ptr<CHttpRequest> CHttpRequest::CreateRequest()
{
    auto Result =  std::shared_ptr<CHttpRequest>(new CHttpRequest());

    return Result;
}

void CHttpRequest::SetUrl(const std::string& Url)
{
    PlatformRequest.SetUrl(Url);
}

void CHttpRequest::SetMethod(EHttpMethod Method)
{
    PlatformRequest.SetMethod(Method);
}

void CHttpRequest::SetHeader(const std::string& Key, const std::string& Value)
{
    PlatformRequest.SetHeader(Key, Value);
}

void CHttpRequest::SetBody(const std::string& Body)
{
    PlatformRequest.SetBody(Body);
}

bool CHttpRequest::Run()
{
    return PlatformRequest.Run(shared_from_this());
}

CHttpRequest::CHttpRequest() : PlatformRequest(Guid)
{
}

void CHttpRequest::Complete(CHttpResponse Response)
{
    OnComplete.Execute(Response);
}
