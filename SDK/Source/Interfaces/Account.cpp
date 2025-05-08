#include "OpenGameSocial.h"
#include "Http/WinHttpRequest.h"


OGS_Result OGS_Account_LoginWithOpenId(const OGS_Account_LoginWithOpenId_Options* Options,
                                       void* UserObject,
                                       OGS_Account_LoginWithOpenId_Callback Callback)
{
    static auto Request = OGS::Http::CPlatformHttpRequest::Create();
    Request->SetUrl(L"https://api.ipify.org/");
    Request->SetMethod(L"GET");
    Request->Run();
    
    if (Callback)
    {
        OGS_Account_LoginWithOpenId_CallbackData Data
        {
            .LocalAccount = INVALID_TITLE_ACCOUNT
        };
        Callback(OGS_Success, UserObject, &Data);
    }

    return OGS_Success;
}
