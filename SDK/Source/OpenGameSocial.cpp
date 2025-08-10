#include "OpenGameSocial.h"

#include "Core/Threading/ThreadPool.h"

#include "Core/Logging/Logger.h"
#include "Services/ServiceContainer.h"


static OGS::TLogCategory LogOpenGameSocial("LogOpenGameSocial");

void OGS_Init(const OGS_Init_Options* Options)
{
#ifdef _DEBUG
    setvbuf(stdout, nullptr, _IONBF, 0);
#endif

    LogOpenGameSocial.Verbose("Initializing OpenGameSocial: [{}]", Options->ThreadPoolSize);

    RunAutoInit();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
    OGS::Services::CServiceContainer::Get().Init();
}

void OGS_SetLogger(OGS_ELogLevel MinLevel, bool bThreadSafe, OGS_LogCallback Callback)
{
    OGS::CLogger::Get().Init(MinLevel, bThreadSafe, Callback);
}
