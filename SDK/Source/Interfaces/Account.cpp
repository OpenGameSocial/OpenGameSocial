#include "OpenGameSocial.h"
#include "Services/ServiceContainer.h"
#include "Services/Account/AccountService.h"


OGS_Result OGS_Account_LoginWithPlatform(const OGS_Account_LoginWithPlatform_Options* Options,
                                         void* UserObject,
                                         OGS_Account_LoginWithPlatform_Callback Callback)
{
    using namespace OGS::Services;
    using namespace OGS::Services::Account;

    auto Lambda = [Callback, UserObject](OGS_TitleAccount AccountId, EAuthenticationStatus Status)
    {
        OGS_Account_LoginWithPlatform_CallbackData Data
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

    const auto Service = CServiceContainer::GetService<CAccountService>();
    const auto Delegate = CAccountService::CAuthenticationCallback::CreateStaticLambda(Lambda);
    Service->Authenticate(Options->IdentityService, Options->Token, Delegate);

    return OGS_Success;
}

OGS_SubscriptionId OGS_Account_SubscribeLoginExpired(void* UserObject,
                                                     OGS_Account_LoginExpired_Callback Callback)
{
    using namespace OGS::Services;
    using namespace OGS::Services::Account;

    const auto Service = CServiceContainer::GetService<CAccountService>();

    auto Lambda = [UserObject, Callback]
    {
        OGS_Account_LoginExpired_CallbackData Data{};
        Callback(UserObject, &Data);
    };
    const auto Handle = Service->OnAuthenticationExpired().AddStaticLambda(Lambda);

    return Handle.ToUnsignedInt64();
}

OGS_Result OGS_Account_UnsubscribeLoginExpired(OGS_SubscriptionId* Id)
{
    using namespace OGS::Services;
    using namespace OGS::Services::Account;

    const auto Service = CServiceContainer::GetService<CAccountService>();

    auto Handle = OGS::CDelegateHandle::FromUnsignedInt64(*Id);

    if (!Handle.IsValid())
    {
        return OGS_UnknownError;
    }

    if (!Service->OnAuthenticationExpired().Remove(Handle))
    {
        return OGS_UnknownError;
    }

    *Id = INVALID_SUBSCRIPTION_ID;
    return OGS_Success;
}
