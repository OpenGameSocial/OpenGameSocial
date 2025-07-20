#pragma once

#include <string>
#include <utility>

#include <nlohmann/json.hpp>


namespace OGS::Http
{
    class CHttpResponse
    {
    public:
        CHttpResponse() = default;

        CHttpResponse(const int32_t InCode, std::string InResult) : Code(InCode), ResultStr(std::move(InResult))
        {}

        virtual ~CHttpResponse() = default;

        [[nodiscard]] int32_t GetCode() const
        {
            return Code;
        }

        [[nodiscard]] const std::string& GetResultStr() const
        {
            return ResultStr;
        }

        [[nodiscard]] std::string&& GetResultStr()
        {
            return std::move(ResultStr);
        }

    protected:
        int32_t Code = 0;
        std::string ResultStr;
    };

    template <typename TResponse>
    class THttpResponse : public CHttpResponse
    {
    public:
        using CResponse = typename TResponse::CResponse;

        THttpResponse(CHttpResponse&& Response) : CHttpResponse(Response.GetCode(), std::move(Response.GetResultStr()))
        {
            Result = nlohmann::json::parse(ResultStr);
        }

        [[nodiscard]] CResponse&& GetResult()
        {
            return std::move(Result);
        }

        [[nodiscard]] const CResponse& GetResult() const
        {
            return Result;
        }

        [[nodiscard]] CResponse* operator->()
        {
            return &Result;
        }


        [[nodiscard]] const CResponse* operator->() const
        {
            return &Result;
        }

    private:
        CResponse Result;
    };
} // namespace OGS::Http
