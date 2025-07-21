#pragma once

#include "AccountTypes.h"

#include "OGS/Core.h"
#include "OGS/Result.h"


OGS_API OGS_Result OGS_Account_LoginWithPlatform(const OGS_Account_LoginWithPlatform_Options* Options,
                                                 void* UserObject,
                                                 OGS_Account_LoginWithPlatform_Callback Callback);

OGS_API OGS_SubscriptionId OGS_Account_SubscribeLoginExpired(void* UserObject,
                                                             OGS_Account_LoginExpired_Callback Callback);

OGS_API OGS_Result OGS_Account_UnsubscribeLoginExpired(OGS_SubscriptionId* Id);
