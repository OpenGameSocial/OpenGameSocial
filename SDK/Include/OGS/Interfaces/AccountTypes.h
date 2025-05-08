#pragma once

#include "OGS/Result.h"


typedef void* OGS_TitleAccount;

#define INVALID_TITLE_ACCOUNT (nullptr)

struct OGS_Account_LoginWithOpenId_Options
{
    const char* IdentityService;
    const char* Token;
};

struct OGS_Account_LoginWithOpenId_CallbackData
{
    OGS_TitleAccount LocalAccount;
};

typedef void (*OGS_Account_LoginWithOpenId_Callback)(OGS_Result Result,
                                                     void* UserObject,
                                                     const OGS_Account_LoginWithOpenId_CallbackData* Data);
