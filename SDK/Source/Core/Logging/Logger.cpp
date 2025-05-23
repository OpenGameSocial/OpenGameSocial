#include "Logger.h"


thread_local std::string OGS::CLogger::Buffer;

OGS::CLogger& OGS::CLogger::Get()
{
    static CLogger Instance;
    return Instance;
}
