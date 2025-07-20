#include "OpenGameSocial.h"
#include "Services/ServiceContainer.h"
#include "Services/Account/AccountService.h"
// #include "Http/WinHttpRequest.h"


OGS_Result OGS_Account_LoginWithOpenId(const OGS_Account_LoginWithOpenId_Options* Options,
                                       void* UserObject,
                                       OGS_Account_LoginWithOpenId_Callback Callback)
{
    using namespace OGS::Services;
    using namespace OGS::Services::Account;

    auto Service = CServiceContainer::GetService<CAccountService>();

    auto Lambda = [Callback, UserObject](OGS_TitleAccount AccountId, EAuthenticationStatus Status)
    {
        OGS_Account_LoginWithOpenId_CallbackData Data
        {
            .LocalAccount = AccountId
        };

        if (Status == EAuthenticationStatus::Authenticated)
        {
            Callback(OGS_Success, UserObject, &Data);
        }
        else
        {
            Callback(OGS_UnknownError, UserObject, &Data);
        }
    };

    auto Delegate = CAccountService::CAuthenticationCallback::CreateStaticLambda(Lambda);
    Service->Authenticate(Options->IdentityService, Options->Token, Delegate);

    return OGS_Success;
}
