#pragma once

#include "Core/Common/MulticastDelegate.h"
#include "Core/Http/HttpResponse.h"
#include "Services/ServiceContainer.h"
#include "Services/Account/AccountService.h"
#include "Services/Signal/Requests.h"


namespace OGS::Services::Signal
{
    class CSignalService final : public TService<CSignalService>
    {
    public:
        using CConnectedDelegate = TMulticastDelegate<const std::string& /*RoomName*/>;

        void Init() override;
        void DeInit() override;

        void Setup(std::weak_ptr<Account::CAccountService> InAccountService);

        void Connect(const std::string& Room);

    private:
        void OnNegotiationFinished(Http::THttpResponse<CNegotiate>&& HttpResponse);
        void OnAuthenticationStatusChanged(Account::EAuthenticationStatus OldStatus, Account::EAuthenticationStatus NewStatus);
    };
}
