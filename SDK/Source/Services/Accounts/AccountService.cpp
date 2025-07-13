#include "AccountService.h"


namespace OGS::Services::Accounts
{
    void CAccountService::Init()
    {}

    void CAccountService::DeInit()
    {}
}

CAutoInit AccountServiceInit(SERVICE_REGISTER(OGS::Services::Accounts::CAccountService));
