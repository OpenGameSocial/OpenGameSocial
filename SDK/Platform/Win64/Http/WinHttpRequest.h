#pragma once

#include <memory>
#include <string>
#include <windows.h>
#include <winhttp.h>

#include "Core/Http/Uri.h"


namespace OGS::Http
{
    class CWinHttpRequest final
    {
    public:
        static std::shared_ptr<CWinHttpRequest> Create();

        ~CWinHttpRequest();

        void SetUrl(const std::wstring& InUrl)
        {
            Url.Parse(InUrl);
        }

        void SetMethod(const std::wstring& InMethod)
        {
            Method = InMethod;
        }

        bool Run();

    private:
        CWinHttpRequest();

        static void WinHttpCallback(
            HINTERNET hInternet,
            DWORD_PTR dwContext,
            DWORD dwInternetStatus,
            LPVOID lpvStatusInformation,
            DWORD dwStatusInformationLength
        );

    private:
        HINTERNET SessionHandle = nullptr;
        HINTERNET ConnectHandle = nullptr;
        HINTERNET RequestHandle = nullptr;

        Uri Url;
        std::wstring Method;

        friend std::shared_ptr<CWinHttpRequest>;
    };

    using CPlatformHttpRequest = CWinHttpRequest;
}
