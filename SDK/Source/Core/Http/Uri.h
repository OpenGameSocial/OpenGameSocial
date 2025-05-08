#pragma once

#include <string>


namespace OGS::Http
{
    class Uri final
    {
    public:
        explicit Uri(const std::wstring& Uri);
        
        Uri() = default;

        void Parse(const std::wstring& Uri);

        [[nodiscard]] bool IsValid() const
        {
            return Parsed;
        }

        [[nodiscard]] std::wstring GetProtocol() const
        {
            return Protocol;
        }

        [[nodiscard]] std::wstring GetUserInfo() const
        {
            return UserInfo;
        }

        [[nodiscard]] std::wstring GetHost() const
        {
            return Host;
        }

        [[nodiscard]] uint16_t GetPort() const
        {
            return Port;
        }

        [[nodiscard]] std::wstring GetPath() const
        {
            return Path;
        }

        [[nodiscard]] std::wstring GetQuery() const
        {
            return Query;
        }

        [[nodiscard]] std::wstring GetFragment() const
        {
            return Fragment;
        }

    private:
        bool Parsed = false;

        std::wstring Protocol;

        std::wstring UserInfo;

        std::wstring Host;
        uint16_t Port = 0;

        std::wstring Path;
        std::wstring Query;

        std::wstring Fragment;
    };
}
