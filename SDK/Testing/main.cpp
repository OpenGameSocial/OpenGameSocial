#include <thread>

#include "OpenGameSocial.h"


static OGS_SubscriptionId LoginExpiredHandle = INVALID_SUBSCRIPTION_ID;

static void OnLoginWithOpenId(OGS_Result Result, void* UserObject,
                              const OGS_Account_LoginWithPlatform_CallbackData* Data)
{
    if (Result == OGS_Success)
    {
        printf("Authorization successful\n");
        return;
    }

    printf("Authorization failed\n");
}

static void Authenticate()
{
    OGS_Account_LoginWithPlatform_Options Options{};
    Options.IdentityService = "Test";
    Options.Token = "testtoken";

    OGS_Account_LoginWithPlatform(&Options, nullptr, OnLoginWithOpenId);
}

static void Relogin(void* UserObject, const OGS_Account_LoginExpired_CallbackData* Data)
{
    printf("Performing relogin\n");
    Authenticate();
}

static void DoLogging(OGS_ELogLevel Level, const char* Message)
{
    printf("%s\n", Message);
}

int main()
{
    OGS_SetLogger(OGS_Verbose, true, DoLogging);

    {
        OGS_Init_Options Options{};
        Options.ThreadPoolSize = 4;
        OGS_Init(&Options);
    }

    {
        LoginExpiredHandle = OGS_Account_SubscribeLoginExpired(nullptr, Relogin);
    }

    Authenticate();

    while (true)
    {
        OGS_Platform_Tick(0.1);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
