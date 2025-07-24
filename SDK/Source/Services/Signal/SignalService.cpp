#include "SignalService.h"

#include "Requests.h"
#include "Backend/Http.h"
#include "Core/Http/Http.h"
#include "WebSocket/WebSocket.h"

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
                   .BindShared(AsShared(), &ThisClass::OnNegotiationFinished, Room);

        HttpRequest->Run();
    }

    void CSignalService::OnWebSocketConnectionChanged(WebSocket::EConnectionState State)
    {
        if (State == WebSocket::EConnectionState::Connected)
        {
            SendHandshake();
        }
    }

    void CSignalService::OnNegotiationFinished(Http::THttpResponse<CNegotiate>&& HttpResponse, const std::string& Room)
    {
        auto ConnectionId = HttpResponse->ConnectionId;

        if (HttpResponse->ConnectionToken)
        {
            ConnectionId = HttpResponse->ConnectionToken.value();
        }

        auto Url = Backend::CBackendUrlProvider::GetUrl(Room);

        constexpr std::string_view from = "http";
        constexpr std::string_view to = "ws";

        if (Url.starts_with(from))
        {
            Url.replace(0, from.length(), to);
        }

        const auto AccountService = CServiceContainer::GetService<Account::CAccountService>();

        WebSocket = WebSocket::CWebSocketManager::Get().CreateSocket();
        WebSocket->SetHeader("Authorization", AccountService->GetToken());
        WebSocket->OnConnectionStateChanged.AddShared(AsShared(), &ThisClass::OnWebSocketConnectionChanged);
        WebSocket->Connect(Url);
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

    void CSignalService::SendHandshake()
    {
        static std::string Handshake = R"({"protocol":"json","version":1})";
        const std::string Message = Handshake + '\x1e';
        WebSocket->Send(Message);
    }
}

CAutoInit SignalServiceInit(SERVICE_REGISTER(OGS::Services::Signal::CSignalService));
