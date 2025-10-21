#include "OpenGameSocial.h"

#include "Core/Common/Ticker.h"
#include "Core/Threading/ThreadPool.h"

#include "Core/Logging/Logger.h"
#include "Core/Network/Socket.h"
#include "Services/ServiceContainer.h"


static OGS::TLogCategory LogOpenGameSocial("LogOpenGameSocial");

class CTestNetworkSocket : OGS::CTicker
{
public:
    CTestNetworkSocket() :
    Socket(OGS::Net::ESocketType::Udp, OGS::Net::EAddressFamily::IpV4)
    {
        Buffer.reserve(8196);

        if (!Socket.SetNonBlocking())
        {
            LogOpenGameSocial.Error("Failed to set socket to non-blocking mode");
            return;
        }

        OGS::Net::CNetAddress BindAddress("0.0.0.0", 9999);
        if (!Socket.Bind(BindAddress))
        {
            LogOpenGameSocial.Error("Failed to bind socket to address [{}]", BindAddress.ToString());
            return;
        }
    }

    void Tick(double ElapsedSeconds) override
    {
        Socket.Tick();

        while (Socket.HasData())
        {
            if (Socket.ReceiveFrom(Buffer, ClientAddress))
            {
                auto Msg = *reinterpret_cast<int*>(Buffer.data());
                LogOpenGameSocial.Verbose("Received msg [{0}]", Msg);

                Socket.SendTo(Buffer, ClientAddress);
            }

            Socket.Tick();
        }
    }

private:
    OGS::Net::CSocket Socket;
    std::vector<uint8_t> Buffer;
    OGS::Net::CNetAddress ClientAddress;
};

static CTestNetworkSocket TestSocket;

void OGS_Init(const OGS_Init_Options* Options)
{
#ifdef _DEBUG
    setvbuf(stdout, nullptr, _IONBF, 0);
#endif

    LogOpenGameSocial.Verbose("Initializing OpenGameSocial: [{}]", Options->ThreadPoolSize);

    RunAutoInit();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
    OGS::Services::CServiceContainer::Get().Init();
}

void OGS_SetLogger(OGS_ELogLevel MinLevel, bool bThreadSafe, OGS_LogCallback Callback)
{
    OGS::CLogger::Get().Init(MinLevel, bThreadSafe, Callback);
}
