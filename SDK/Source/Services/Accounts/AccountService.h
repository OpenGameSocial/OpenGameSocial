#pragma once

#include <utility>

#include "Services/ServiceContainer.h"


namespace OGS::Services::Accounts
{
    class CAccountService final : public CServiceBase
    {
    public:
        void Init() override;
        void DeInit() override;

        const std::string& GetToken() const
        {
            return Token;
        }

    private:
        std::string Token;
    };
}
