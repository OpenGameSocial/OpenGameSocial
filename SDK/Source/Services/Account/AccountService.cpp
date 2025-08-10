#include "AccountService.h"

#include "AccountRequests.h"
#include "Backend/Http.h"
#include "Core/Logging/Logger.h"


static OGS::TLogCategory LogAccountService("LogAccountService");

namespace OGS::Services::Account
{
    void CAccountService::Init()
    {}

    void CAccountService::DeInit()
    {}

    void CAccountService::Tick(double ElapsedSeconds)
    {
        RenewTime += ElapsedSeconds;

        if (RenewTime < RenewCheckSeconds)
        {
            return;
        }

        RenewTime = 0;

        if (Token.empty())
        {
            return;
        }

        auto Minutes = std::chrono::duration_cast<std::chrono::minutes>(ValidUntil - UtcNow());

        if (Minutes.count() > RenewMinutes)
        {
            return;
        }

        AuthenticationExpired.Broadcast();
    }

    void CAccountService::Authenticate(const std::string& Provider, const std::string& Token,
                                       CAuthenticationCallback Callback)
    {
        using namespace OGS::Backend;

        auto RequestCallback = [this, Callback](Http::THttpResponse<CAuthenticate>&& Response)
        {
            if (!Response)
            {
                this->Token.resize(0);
                ChangeStatus(EAuthenticationStatus::NotAuthenticated);
                Callback.Execute(INVALID_TITLE_ACCOUNT, EAuthenticationStatus::NotAuthenticated);

                return;
            }

            this->Token = Response->TokenType + " " + Response->Token;
            this->ValidUntil = Response->ValidUntil;

            const auto Seconds = std::chrono::duration_cast<std::chrono::seconds>(ValidUntil - UtcNow());
            LogAccountService.Verbose("Authenticated successfully. Token valid for [{}] seconds.", Seconds.count());

            ChangeStatus(EAuthenticationStatus::Authenticated);
            Callback.Execute(reinterpret_cast<OGS_TitleAccount>(1), EAuthenticationStatus::Authenticated);
        };

        LogAccountService.Verbose("Started authentication using [{}] provider.", Provider);

        Http::Post<CAuthenticate, CBackendUrlProvider, CAuthenticatedMode>({
            .Provider = Provider,
            .Data = Token
        }).BindSharedLambda(AsShared(), RequestCallback);
    }

    void CAccountService::ChangeStatus(EAuthenticationStatus InStatus)
    {
        auto OldStatus = Status;
        Status = InStatus;
        StatusChanged.Broadcast(OldStatus, Status);
    }
} // namespace OGS::Services::Account

CAutoInit AccountServiceInit(SERVICE_REGISTER(OGS::Services::Account::CAccountService));
