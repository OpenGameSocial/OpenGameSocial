#include "TestService.h"

void OGS::Services::Accounts::CTestService::Init()
{}

void OGS::Services::Accounts::CTestService::DeInit()
{}

void OGS::Services::Accounts::CTestService::PrintHello()
{
    printf("Hello\n");
}

CAutoInit TestServiceInit(SERVICE_REGISTER(OGS::Services::Accounts::CTestService));