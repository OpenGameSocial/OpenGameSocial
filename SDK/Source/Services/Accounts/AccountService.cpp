#include "AccountService.h"
#include "TestService.h"


namespace OGS::Services::Accounts
{
    void CAccountService::Init()
    {
        TestService->PrintHello();
    }

    void CAccountService::DeInit()
    {}
}

CAutoInit AccountServiceInit(SERVICE_REGISTER(OGS::Services::Accounts::CAccountService));