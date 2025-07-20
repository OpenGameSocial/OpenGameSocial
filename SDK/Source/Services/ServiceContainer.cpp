#include "ServiceContainer.h"

#include "Services/ServiceTraits.h"
#include "Services/Accounts/AccountService.h"


OGS::Services::CServiceContainer& OGS::Services::CServiceContainer::Get()
{
    static CServiceContainer Instance;
    return Instance;
}

void OGS::Services::CServiceContainer::Init()
{
    for (auto& [Type, Service] : ServiceMap)
    {
        Service.Setup(Service.Service.get());
    }

    for (auto& [Type, Service] : ServiceMap)
    {
        Service.Service->Init();
    }
}

void OGS::Services::CServiceContainer::DeInit()
{
    for (auto& [Type, Service] : ServiceMap)
    {
        Service.Service->DeInit();
    }
}
