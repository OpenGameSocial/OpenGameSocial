#include "OpenGameSocial.h"

#include "Core/Common/Delegate.h"
#include "Core/Common/MulticastDelegate.h"
#include "Core/Treading/ThreadPool.h"


class CTestClass : public std::enable_shared_from_this<CTestClass>
{
public:
    void PrintShit()
    {
        printf("Hello, World!\n");
    }
};

void OGS_Init(const OGS_Init_Options* Options)
{
    auto Ptr = std::make_shared<CTestClass>();

    OGS::CSimpleMulticastDelegate Delegate;

    
    auto ValidHandle = Delegate.AddShared(Ptr, &CTestClass::PrintShit);

    OGS::CDelegateHandle InvalidHandle(0, 123);
    Delegate.Remove(InvalidHandle);

    Delegate.Broadcast();

    Ptr.reset();
    
    Delegate.Broadcast();

    OGS::Threading::CThreadPool::Get().Init(Options->ThreadPoolSize);
}
