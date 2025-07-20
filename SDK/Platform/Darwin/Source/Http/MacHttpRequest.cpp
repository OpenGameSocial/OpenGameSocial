#include "MacHttpRequest.h"

#include "Core/Http/HttpManager.h"


using namespace OGS::Http::Mac;

void CMacHttpRequest::SetMethod(EHttpMethod InMethod)
{
    switch (InMethod)
    {
    case EHttpMethod::Get:
        Method = "GET";
        break;
    case EHttpMethod::Post:
        Method = "POST";
        break;
    case EHttpMethod::Put:
        Method = "PUT";
        break;
    case EHttpMethod::Delete:
        Method = "DELETE";
        break;
    case EHttpMethod::Patch:
        Method = "PATCH";
        break;
    }
}