#pragma once

#include <cstdio>

#include "Services/ServiceContainer.h"


namespace OGS::Services::Accounts
{
    class CTestService : public CServiceBase
    {
    public:
        void Init() override;

        void DeInit() override;

        void PrintHello();
    };
}
