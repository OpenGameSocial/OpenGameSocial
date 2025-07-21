#pragma once

#include "Core/Common/Date.h"
#include "Core/Common/Delegate.h"
#include "Core/Common/MulticastDelegate.h"
#include "Core/Common/OperationResult.h"
#include "Core/Common/Ticker.h"
#include "OGS/Interfaces/AccountTypes.h"
#include "Services/ServiceContainer.h"


namespace OGS::Services::Account
{
    enum class EAuthenticationStatus
    {
        Authenticated,
        NotAuthenticated,
    };

    class CAccountService final : public TService<CAccountService>, public CTicker
    {
    public:
        using CStatusChangedDelegate = TMulticastDelegate<EAuthenticationStatus /*OldStatus*/,
                                                          EAuthenticationStatus /*NewStatus*/>;
        using CAuthenticationExpiredDelegate = CSimpleMulticastDelegate;

        using CAuthenticationCallback = TDelegate<OGS_TitleAccount, EAuthenticationStatus>;

        void Init() override;
        void DeInit() override;

        void Tick(double ElapsedSeconds) override;

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

        CAuthenticationExpiredDelegate& OnAuthenticationExpired()
        {
            return AuthenticationExpired;
        }

    private:
        void ChangeStatus(EAuthenticationStatus InStatus);

    private:
        static constexpr auto RenewMinutes = 5;
        static constexpr auto RenewCheckSeconds = 60;

        EAuthenticationStatus Status = EAuthenticationStatus::NotAuthenticated;

        double RenewTime = 0;

        std::string Token;
        CDateTime ValidUntil;

        CStatusChangedDelegate StatusChanged;
        CAuthenticationExpiredDelegate AuthenticationExpired;
    };
} // namespace OGS::Services::Accounts
