#include "ServiceContainer.h"


OGS::Services::CServiceContainer& OGS::Services::CServiceContainer::Get()
{
    static CServiceContainer Instance;
    return Instance;
}
