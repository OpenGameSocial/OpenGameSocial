#pragma once

#include "AccountRequests.h"
#include "Core/Common/Delegate.h"
#include "Core/Common/MulticastDelegate.h"
#include "Core/Common/OperationResult.h"
#include "Core/Http/HttpResponse.h"
#include "OGS/Interfaces/AccountTypes.h"
#include "Services/ServiceContainer.h"


namespace OGS::Services::Account
{
    enum class EAuthenticationStatus
    {
        Authenticated,
        NotAuthenticated,
    };

    class CAccountService final : public CServiceBase
    {
    public:
        using CStatusChangedDelegate = TMulticastDelegate<EAuthenticationStatus>;

        using CAuthenticationCallback = TDelegate<OGS_TitleAccount, EAuthenticationStatus>;

        void Init() override;
        void DeInit() override;

        void Authenticate(const std::string& Provider, const std::string& Token,
            CAuthenticationCallback Callback = {});

        const std::string& GetToken() const
        {
            return Token;
        }

        CStatusChangedDelegate& OnStatusChanged()
        {
            return StatusChanged;
        }

    private:
        void ChangeStatus(EAuthenticationStatus InStatus);

    private:
        EAuthenticationStatus Status = EAuthenticationStatus::NotAuthenticated;

        std::string Token;

        CStatusChangedDelegate StatusChanged;
    };
} // namespace OGS::Services::Accounts
