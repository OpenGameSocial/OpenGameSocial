#include "SignalService.h"
#include "Requests.h"
#include "Backend/Http.h"
#include "Core/Http/Http.h"

namespace OGS::Services::Signal
{
    void CSignalService::Init()
    {}

    void CSignalService::DeInit()
    {}

    void CSignalService::Setup(std::weak_ptr<Account::CAccountService> InAccountService)
    {
        if (const auto Service = InAccountService.lock())
        {
            Service->OnStatusChanged().AddShared(AsShared(), &ThisClass::OnAuthenticationStatusChanged);
        }
    }

    void CSignalService::Connect(const std::string& Room)
    {
        using CNegotiateRequest = Http::THttpRequest<CNegotiate>;

        const auto Url = Backend::CBackendUrlProvider::GetUrl(Room + "/negotiate?negotiateVersion=1");

        const auto HttpRequest = CNegotiateRequest::CreateRequest();
        HttpRequest->SetMethod(Http::EHttpMethod::Post);
        HttpRequest->SetUrl(Url);
        Backend::CAuthenticatedMode::SetupRequest(*HttpRequest, CNegotiate::CRequest{});

        HttpRequest->OnComplete()
                   .BindShared(AsShared(), &ThisClass::OnNegotiationFinished);

        HttpRequest->Run();
    }

    void CSignalService::OnNegotiationFinished(Http::THttpResponse<CNegotiate>&& HttpResponse)
    {
        auto ConnectionId = HttpResponse->ConnectionId;

        if (HttpResponse->ConnectionToken)
        {
            ConnectionId = HttpResponse->ConnectionToken.value();
        }

        
    }

    void CSignalService::OnAuthenticationStatusChanged(Account::EAuthenticationStatus OldStatus,
                                                       Account::EAuthenticationStatus NewStatus)
    {
        if (OldStatus == Account::EAuthenticationStatus::NotAuthenticated &&
            NewStatus == Account::EAuthenticationStatus::Authenticated)
        {
            Connect("testHub");
        }
    }
}

CAutoInit SignalServiceInit(SERVICE_REGISTER(OGS::Services::Signal::CSignalService));
