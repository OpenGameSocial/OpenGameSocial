#include "OpenGameSocial.h"
#include "Core/Logging/Logger.h"
#include "Core/Threading/ThreadPool.h"


void OGS_Platform_Tick(double ElapsedSeconds)
{
    OGS::Threading::CThreadPool::Get().Tick();
    OGS::CLogger::Get().Tick();
}
