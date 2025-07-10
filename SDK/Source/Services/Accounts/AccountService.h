#pragma once

#include "Services/ServiceContainer.h"


namespace OGS::Services::Accounts
{
    class CAccountService final : public CServiceBase
    {
    public:
        void Init() override;
        void DeInit() override;
    };
}
