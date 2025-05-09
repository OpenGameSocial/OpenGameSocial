#include "WinHttpRequest.h"

#include <iostream>

#pragma comment(lib, "winhttp.lib")


using namespace OGS::Http::Win64;

// TODO: implement request manager

std::shared_ptr<CWinHttpRequest> CWinHttpRequest::Create()
{
    return std::shared_ptr<CWinHttpRequest>(new CWinHttpRequest());
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

bool CWinHttpRequest::Run()
{
    if (ConnectHandle != nullptr)
    {
        // TODO: logme
        return false;
    }
    
    ConnectHandle = WinHttpConnect(SessionHandle, Url.GetHost().c_str(), Url.GetPort(), 0);

    if (ConnectHandle == nullptr)
    {
        // TODO: logme
        return false;
    }

    RequestHandle = WinHttpOpenRequest(ConnectHandle, Method.c_str(), Url.GetPath().c_str(),
                                       nullptr, nullptr, nullptr, 0);

    if (RequestHandle == nullptr)
    {
        // TODO: logme
        return false;
    }

    if (!WinHttpSendRequest(RequestHandle, nullptr, 0,
                            nullptr, 0, 0, reinterpret_cast<DWORD_PTR>(this)))
    {
        // TODO: logme
        return false;
    }

    return true;
}

CWinHttpRequest::CWinHttpRequest()
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

void CWinHttpRequest::WinHttpCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
                                      LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
    if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE)
    {
        auto* This = reinterpret_cast<CWinHttpRequest*>(dwContext);
        WinHttpReceiveResponse(This->RequestHandle, nullptr);

        return;
    }

    if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_REQUEST_ERROR)
    {
        auto* ErrorInfo = static_cast<WINHTTP_ASYNC_RESULT*>(lpvStatusInformation);
        printf("Request error: %i\n", ErrorInfo->dwError);
        return;
    }

    if (dwInternetStatus == WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE)
    {
        auto* This = reinterpret_cast<CWinHttpRequest*>(dwContext);

        DWORD size = 0;
        DWORD sizeSize = sizeof(size);
        if (WinHttpQueryHeaders(This->RequestHandle, WINHTTP_QUERY_CONTENT_LENGTH, nullptr, &size, &sizeSize, nullptr))
        {
        }

        DWORD bytesRead = 0;
        std::unique_ptr<char[]> buffer(new char[1024]);

        if (WinHttpReadData(This->RequestHandle, buffer.get(), 1024, &bytesRead))
        {
            std::wcout << L"[INFO] Read " << bytesRead << L" bytes:\n";
            std::cout.write(buffer.get(), bytesRead);
            std::cout << std::endl;
        }

        return;
    }
}
