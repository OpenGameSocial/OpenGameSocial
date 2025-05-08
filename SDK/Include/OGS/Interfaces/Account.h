#pragma once

#include "AccountTypes.h"

#include "OGS/Core.h"
#include "OGS/Result.h"


OGS_API OGS_Result OGS_Account_LoginWithOpenId(const OGS_Account_LoginWithOpenId_Options* Options,
                                               void* UserObject,
                                               OGS_Account_LoginWithOpenId_Callback Callback);
