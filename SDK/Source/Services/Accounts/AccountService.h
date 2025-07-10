#pragma once

#include <utility>

#include "Services/ServiceContainer.h"
#include "Services/Accounts/TestService.h"


namespace OGS::Services::Accounts
{
    class CAccountService final : public CServiceBase
    {
    public:
        void Setup(std::shared_ptr<CTestService> InTestService)
        {
            TestService = std::move(InTestService);
        }

        void Init() override;
        void DeInit() override;

    private:
        std::shared_ptr<CTestService> TestService;
    };
}
