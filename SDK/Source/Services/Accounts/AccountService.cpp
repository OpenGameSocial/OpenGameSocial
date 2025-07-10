#include "AccountService.h"
#include "TestService.h"

void OGS::Services::Accounts::CAccountService::Init()
{
    TestService->PrintHello();
}

void OGS::Services::Accounts::CAccountService::DeInit()
{}
