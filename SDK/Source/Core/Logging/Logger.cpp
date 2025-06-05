#include "Logger.h"


OGS::CLogger& OGS::CLogger::Get()
{
    static CLogger Instance;
    return Instance;
}
