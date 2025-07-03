#include <thread>

#include "testHeader.h"

#include "OpenGameSocial.h"


static void OnLoginWithOpenId(OGS_Result Result, void* UserObject, const OGS_Account_LoginWithOpenId_CallbackData* Data)
{
    // DEBUG_BREAK();
}

static void DoLogging(OGS_ELogLevel Level, const char* Message)
{
    printf("%s\n", Message);
}

int main()
{
    PrintHello();
    OGS_SetLogger(OGS_Verbose, true, DoLogging);

    {
        OGS_Init_Options Options{};
        Options.ThreadPoolSize = 4;
        OGS_Init(&Options);
    }

    {
        OGS_Account_LoginWithOpenId_Options Options{};
        Options.IdentityService = "Test";
        Options.Token = "testtoken";

        OGS_Account_LoginWithOpenId(&Options, nullptr, OnLoginWithOpenId);
    }

    while (true)
    {
        OGS_Platform_Tick(0.1);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
