#include "HttpRequest.h"

#include "HttpManager.h"

using namespace OGS::Http;


std::shared_ptr<CHttpRequest> CHttpRequest::CreateRequest()
{
    return std::shared_ptr<CHttpRequest>(new CHttpRequest());
}

void CHttpRequest::SetUrl(const Uri& Url)
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

void CHttpRequest::Complete(CHttpResponse&& Response)
{
    OnComplete.Execute(Response);
}
