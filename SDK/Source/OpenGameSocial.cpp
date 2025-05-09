#include "OpenGameSocial.h"

#include "Core/Treading/ThreadPool.h"


void OGS_Init(const OGS_Init_Options* Options)
{
    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
}
