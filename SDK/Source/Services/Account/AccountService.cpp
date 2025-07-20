#include "AccountService.h"
#include "Backend/Http.h"


namespace OGS::Services::Account
{
    void CAccountService::Init()
    {
        Token = "Test test-token";
    }

    void CAccountService::DeInit()
    {}

    void CAccountService::Authenticate(const std::string& Provider, const std::string& Token,
                                       CAuthenticationCallback Callback)
    {
        using namespace OGS::Backend;

        const auto SharedThis = std::static_pointer_cast<CAccountService>(shared_from_this());

        auto RequestCallback = [this, Callback](Http::THttpResponse<CAuthenticate>&& Response)
        {
            if (!Response)
            {
                this->Token.resize(0);
                ChangeStatus(EAuthenticationStatus::NotAuthenticated);
                Callback.Execute(INVALID_TITLE_ACCOUNT, EAuthenticationStatus::NotAuthenticated);

                return;
            }

            this->Token = Response->Token;
            ChangeStatus(EAuthenticationStatus::Authenticated);
            Callback.Execute(reinterpret_cast<OGS_TitleAccount>(1), EAuthenticationStatus::Authenticated);
        };

        Http::Post<CAuthenticate, CBackendUrlProvider, CAuthenticatedMode>({
            .Provider = Provider,
            .Data = Token
        }).BindSharedLambda(SharedThis, RequestCallback);
    }

    void CAccountService::ChangeStatus(EAuthenticationStatus InStatus)
    {
        Status = InStatus;
        StatusChanged.Broadcast(Status);
    }
} // namespace OGS::Services::Account

CAutoInit AccountServiceInit(SERVICE_REGISTER(OGS::Services::Account::CAccountService));
