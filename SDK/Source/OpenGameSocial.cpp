#include "OpenGameSocial.h"

#include <iostream>
#include <date/date.h>

#include "Core/Threading/ThreadPool.h"

#include "Core/Logging/Logger.h"
#include "Services/ServiceContainer.h"


static OGS::TLogCategory LogOpenGameSocial("LogOpenGameSocial");

void OGS_Init(const OGS_Init_Options* Options)
{
    LogOpenGameSocial.Verbose("Initializing OpenGameSocial: %i", Options->ThreadPoolSize);

    RunAutoInit();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
    OGS::Services::CServiceContainer::Get().Init();
}

void OGS_SetLogger(OGS_ELogLevel MinLevel, bool bThreadSafe, OGS_LogCallback Callback)
{
    OGS::CLogger::Get().Init(MinLevel, bThreadSafe, Callback);
}
