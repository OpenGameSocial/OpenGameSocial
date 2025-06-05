#pragma once

#include <string>


namespace OGS::Http
{
    class Uri final
    {
    public:
        Uri(const std::string& Uri);
        
        Uri() = default;

        void Parse(const std::string& Uri);

        [[nodiscard]] bool IsValid() const
        {
            return Parsed;
        }

        [[nodiscard]] const std::string& ToString() const
        {
            return SourceUri;
        }

        [[nodiscard]] std::string GetProtocol() const
        {
            return Protocol;
        }

        [[nodiscard]] std::string GetUserInfo() const
        {
            return UserInfo;
        }

        [[nodiscard]] std::string GetHost() const
        {
            return Host;
        }

        [[nodiscard]] uint16_t GetPort() const
        {
            return Port;
        }

        [[nodiscard]] std::string GetPath() const
        {
            return Path;
        }

        [[nodiscard]] std::string GetQuery() const
        {
            return Query;
        }

        [[nodiscard]] std::string GetFragment() const
        {
            return Fragment;
        }

    private:
        bool Parsed = false;

        std::string SourceUri;

        std::string Protocol;

        std::string UserInfo;

        std::string Host;
        uint16_t Port = 0;

        std::string Path;
        std::string Query;

        std::string Fragment;
    };
}
