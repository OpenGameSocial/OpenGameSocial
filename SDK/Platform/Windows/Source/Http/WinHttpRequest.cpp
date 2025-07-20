#include "WinHttpRequest.h"

#include <iostream>
#include <map>

#include "Core/Http/HttpManager.h"
#include "Internal/StringUtils.h"

#pragma comment(lib, "winhttp.lib")


using namespace OGS::Win64::Internal;

namespace OGS::Http::Win64
{
    CWinHttpRequest::CWinHttpRequest(const CGuid& InGuid)
        : Guid(InGuid)
    {
        SessionHandle = WinHttpOpen(L"Test", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
            nullptr, nullptr, WINHTTP_FLAG_ASYNC);

        if (WinHttpSetStatusCallback(SessionHandle, WinHttpCallback,
            WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS,
            0) == WINHTTP_INVALID_STATUS_CALLBACK)
        {
            __debugbreak();
        }
    }

    CWinHttpRequest::~CWinHttpRequest()
    {
        if (RequestHandle)
        {
            WinHttpCloseHandle(RequestHandle);
            RequestHandle = nullptr;
        }

        if (ConnectHandle)
        {
            WinHttpCloseHandle(ConnectHandle);
            ConnectHandle = nullptr;
        }

        if (SessionHandle)
        {
            WinHttpCloseHandle(SessionHandle);
            SessionHandle = nullptr;
        }
    }

    void CWinHttpRequest::SetMethod(EHttpMethod InMethod)
    {
#pragma push_macro("DELETE")
#undef DELETE

        switch (InMethod)
        {
        case EHttpMethod::Get:
            Method = L"GET";
            break;
        case EHttpMethod::Post:
            Method = L"POST";
            break;
        case EHttpMethod::Put:
            Method = L"PUT";
            break;
        case EHttpMethod::Delete:
            Method = L"DELETE";
            break;
        case EHttpMethod::Patch:
            Method = L"PATCH";
            break;
        }

#pragma pop_macro("DELETE")
    }

    void CWinHttpRequest::SetHeader(const std::string& Key, const std::string& Value)
    {
        const auto Header = Key + ": " + Value + "\r\n";
        Headers.append(Utf8StringToWideString(Header));
    }

    bool CWinHttpRequest::Run(const std::shared_ptr<CHttpRequest>& Owner)
    {
        if (ConnectHandle != nullptr)
        {
            // TODO: logme
            return false;
        }

        ConnectHandle = WinHttpConnect(SessionHandle, Utf8StringToWideString(Url.GetHost()).c_str(), Url.GetPort(), 0);

        if (ConnectHandle == nullptr)
        {
            // TODO: logme
            return false;
        }

        RequestHandle = WinHttpOpenRequest(ConnectHandle, Method.c_str(),
            Utf8StringToWideString(Url.GetPath()).c_str(),
            nullptr, nullptr, nullptr, 0);

        if (RequestHandle == nullptr)
        {
            // TODO: logme
            return false;
        }

        if (!Headers.empty() && !WinHttpAddRequestHeaders(RequestHandle, Headers.c_str(),
            static_cast<DWORD>(-1), WINHTTP_ADDREQ_FLAG_ADD))
        {
            // TODO: logme
            return false;
        }

        if (!WinHttpSendRequest(RequestHandle, nullptr, 0,
            Body.data(), Body.size(), Body.size(), reinterpret_cast<DWORD_PTR>(this)))
        {
            // TODO: logme
            return false;
        }

        auto& Mgr = CHttpManager::Get();
        Mgr.AddRequest(Owner);

        return true;
    }

    void CWinHttpRequest::WinHttpCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
        LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
    {
        auto& Mgr = CHttpManager::Get();
        auto* This = reinterpret_cast<CWinHttpRequest*>(dwContext);

        if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE)
        {
            WinHttpReceiveResponse(This->RequestHandle, nullptr);
            return;
        }

        if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_REQUEST_ERROR)
        {
            auto* ErrorInfo = static_cast<WINHTTP_ASYNC_RESULT*>(lpvStatusInformation);

            Mgr.CompleteRequest(This->Guid, GetFromWinError(ErrorInfo->dwError));

            return;
        }

        if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE)
        {
            DWORD StatusCode = 0;
            DWORD Size = sizeof(StatusCode);

            if (WinHttpQueryHeaders(This->RequestHandle, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                nullptr, &StatusCode, &Size, nullptr))
            {
                This->ResponseCode = static_cast<int32_t>(StatusCode);
            }

            WinHttpQueryDataAvailable(This->RequestHandle, nullptr);
            return;
        }

        if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_READ_COMPLETE)
        {
            // Can be ignored, we're reading synchronously
            return;
        }

        if (dwInternetStatus != WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE)
        {
            __debugbreak();
            return;
        }

        const DWORD AvailableData = *static_cast<LPDWORD>(lpvStatusInformation);

        if (AvailableData == 0)
        {
            Mgr.CompleteRequest(This->Guid, {
                This->ResponseCode,
                This->ResponseBody
            });

            WinHttpCloseHandle(This->RequestHandle);

            return;
        }

        const std::unique_ptr<char[]> Buffer(new char[AvailableData]);
        DWORD BytesRead = 0;

        if (WinHttpReadData(This->RequestHandle, Buffer.get(), AvailableData, &BytesRead) && BytesRead > 0)
        {
            This->ResponseBody.append(Buffer.get(), BytesRead);
        }

        WinHttpQueryDataAvailable(This->RequestHandle, nullptr);
    }

    CHttpResponse CWinHttpRequest::GetFromWinError(DWORD dwErrorCode)
    {
        static const std::map<DWORD, std::string> ErrorMap =
        {
            {ERROR_WINHTTP_TIMEOUT, "Request timed out"},
            {ERROR_WINHTTP_CONNECTION_ERROR, "Failed to connect to the server"},
            {ERROR_WINHTTP_NAME_NOT_RESOLVED, "DNS name could not be resolved"},
            {ERROR_WINHTTP_CANNOT_CONNECT, "Cannot connect to the server"},
            {ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED, "Client certificate is required"},
            {ERROR_WINHTTP_SECURE_FAILURE, "SSL/TLS handshake failed"},
            {ERROR_WINHTTP_INVALID_SERVER_RESPONSE, "Invalid response from server"},
            {ERROR_WINHTTP_OPERATION_CANCELLED, "Operation was canceled"},
            {ERROR_WINHTTP_REDIRECT_FAILED, "HTTP redirect failed"},
            {ERROR_WINHTTP_SECURE_CHANNEL_ERROR, "Secure channel error"}
        };

        auto It = ErrorMap.find(dwErrorCode);

        if (It != ErrorMap.end())
        {
            return CHttpResponse(0, It->second);
        }

        return CHttpResponse(0, "Unknown http error: " + std::to_string(dwErrorCode));
    }
}
