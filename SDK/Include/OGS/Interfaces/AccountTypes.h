#pragma once

#include "OGS/Result.h"


typedef void* OGS_TitleAccount;

#define INVALID_TITLE_ACCOUNT (nullptr)

struct OGS_Account_LoginWithPlatform_Options
{
    const char* IdentityService;
    const char* Token;
};

struct OGS_Account_LoginWithPlatform_CallbackData
{
    OGS_TitleAccount LocalAccount;
};

typedef void (*OGS_Account_LoginWithPlatform_Callback)(OGS_Result Result,
                                                     void* UserObject,
                                                     const OGS_Account_LoginWithPlatform_CallbackData* Data);

struct OGS_Account_LoginExpired_CallbackData
{
    OGS_TitleAccount LocalAccount;
};

typedef void (*OGS_Account_LoginExpired_Callback)(void* UserObject, const OGS_Account_LoginExpired_CallbackData* Data);