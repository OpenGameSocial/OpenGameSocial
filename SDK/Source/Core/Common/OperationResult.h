#pragma once

#include <string>


namespace OGS
{
    class COperationResult
    {
    public:
        static COperationResult Success()
        {
            return COperationResult();
        }

        static COperationResult Error(int32_t ErrorCode, const std::string& ErrorMessage)
        {
            return COperationResult(ErrorCode, ErrorMessage);
        }

        [[nodiscard]] int32_t GetErrorCode() const
        {
            return Code;
        }

        [[nodiscard]] const std::string& GetErrorMessage() const
        {
            return Message;
        }

        virtual ~COperationResult() = default;

    protected:
        COperationResult(int32_t InCode, std::string InMessage) : Code(InCode), Message(std::move(InMessage))
        {}

        COperationResult() : Succeeded(true)
        {}

    private:
        int32_t Code = 0;
        std::string Message;

        bool Succeeded = false;
    };

    template <typename T>
    class TOperationResult final : public COperationResult
    {
    public:
        static TOperationResult Success(T&& InValue)
        {
            return TOperationResult(std::move(InValue));
        }

        [[nodiscard]] const T& GetValue() const
        {
            return Result;
        }

        [[nodiscard]] T&& GetValue()
        {
            return std::move(Result);
        }

    private:
        explicit TOperationResult(T&& InValue) : Result(std::move(InValue))
        {}

    private:
        T Result;
    };
} // namespace OGS
