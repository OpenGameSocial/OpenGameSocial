#include "MacHttpRequest.h"

#include "Core/Http/HttpManager.h"


using namespace OGS::Http::Mac;

void CMacHttpRequest::SetMethod(EHttpMethod InMethod)
{
    switch (InMethod)
    {
    case EHttpMethod::GET:
        Method = "GET";
        break;
    case EHttpMethod::POST:
        Method = "POST";
        break;
    case EHttpMethod::PUT:
        Method = "PUT";
        break;
    case EHttpMethod::DELETE:
        Method = "DELETE";
        break;
    case EHttpMethod::PATCH:
        Method = "PATCH";
        break;
    }
}