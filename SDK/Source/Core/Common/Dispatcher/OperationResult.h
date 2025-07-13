#pragma once

#include <cstdint>
#include <string>


namespace OGS::Dispatcher
{
    class COperationResult
    {
    public:
        static COperationResult Success()
        {
            return COperationResult(0, "Success");
        }

        static COperationResult Error(int32_t InCode, const std::string& InMessage)
        {
            return COperationResult(InCode, InMessage);
        }

        operator bool() const
        {
            return Code != 0;
        }

        int32_t GetCode() const
        {
            return Code;
        }

        const std::string& GetMessage() const
        {
            return Message;
        }

    private:
        COperationResult(int32_t InCode, const std::string& InMessage) :
        Code(InCode),
        Message(InMessage)
        {}

    private:
        int32_t Code = 0;
        std::string Message;
    };
}
