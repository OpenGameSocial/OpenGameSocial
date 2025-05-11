#pragma once

#include <string>


namespace OGS::Http
{
    class CHttpResponse final
    {
    public:
        CHttpResponse() = default;

        CHttpResponse(const int32_t InCode, const std::string& InResult)
            : Code(InCode),
              Result(InResult)
        {
        }

        [[nodiscard]] int32_t GetCode() const
        {
            return Code;
        }

        [[nodiscard]] const std::string& GetResult() const
        {
            return Result;
        }

    private:
        int32_t Code;
        std::string Result;
    };
}
