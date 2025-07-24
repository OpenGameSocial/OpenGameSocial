#pragma once

#include <string>


namespace OGS::Http
{
    class Uri final
    {
    public:
        Uri(std::string_view Uri);

        Uri(const std::string& Uri);

        Uri() = default;

        void Parse(std::string_view Uri);

        [[nodiscard]] bool IsValid() const
        {
            return Parsed;
        }

        [[nodiscard]] const std::string& ToString() const
        {
            return SourceUri;
        }

        [[nodiscard]] const std::string& GetProtocol() const
        {
            return Protocol;
        }

        [[nodiscard]] const std::string& GetUserInfo() const
        {
            return UserInfo;
        }

        [[nodiscard]] const std::string& GetHost() const
        {
            return Host;
        }

        [[nodiscard]] uint16_t GetPort() const
        {
            return Port;
        }

        [[nodiscard]] const std::string& GetPath() const
        {
            return Path;
        }

        [[nodiscard]] const std::string& GetQuery() const
        {
            return Query;
        }

        [[nodiscard]] const std::string& GetFragment() const
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
