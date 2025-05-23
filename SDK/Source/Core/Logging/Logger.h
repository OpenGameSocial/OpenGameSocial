#pragma once

#include <string>

#include "Core/Common/Containers/NonBlockingQueue.h"
#include "OpenGameSocial.h"


namespace OGS
{
    namespace Details
    {
        struct SLogEntry final
        {
        public:
            OGS_ELogLevel Level = OGS_Verbose;
            std::string Message;
        };
    }

    class CLogger final
    {
    public:
        static CLogger& Get();

        void Init(OGS_ELogLevel InMinLevel, bool bInThreadSafe, OGS_LogCallback InCallback)
        {
            MinLevel = InMinLevel;
            Callback = InCallback;
            bThreadSafe = bInThreadSafe;
        }

        void Tick()
        {
            if (!bThreadSafe || !Callback)
            {
                return;
            }

            std::swap(FrontQueue, BackQueue);

            Details::SLogEntry Entry;

            while (FrontQueue->TryDequeue(Entry))
            {
                Callback(Entry.Level, Entry.Message.c_str());
            }
        }

        template <typename... TArgs>
        void Log(OGS_ELogLevel Level, const char* Format, TArgs... Args)
        {
            if (!Callback || MinLevel > Level)
            {
                return;
            }

            auto Size = SNPRINTF(nullptr, 0, Format, std::forward<TArgs>(Args)...);

            if (Size + 1 > Buffer.capacity())
            {
                Buffer.reserve(Size + 1);
            }

            Buffer.resize(Size);

            SNPRINTF(Buffer.data(), Size + 1, Format, std::forward<TArgs>(Args)...);

            if (!bThreadSafe)
            {
                Callback(Level, Buffer.data());
            }

            BackQueue->Enqueue({
                .Level = Level,
                .Message = Buffer
            });
        }

    private:
        CLogger() : FrontQueue(std::make_unique<CLogQueue>()), BackQueue(std::make_unique<CLogQueue>())
        {
        }

    private:
        using CLogQueue = TNonBlockingQueue<Details::SLogEntry>;
        using CLogQueuePtr = std::unique_ptr<CLogQueue>;

        static thread_local std::string Buffer;

        CLogQueuePtr FrontQueue;
        CLogQueuePtr BackQueue;

        OGS_ELogLevel MinLevel = OGS_Info;
        OGS_LogCallback Callback = nullptr;
        bool bThreadSafe = false;
    };

    template <typename... TArgs>
    inline void Log(OGS_ELogLevel Level, const char* Format, TArgs... Args)
    {
        CLogger::Get().Log(Level, Format, std::forward<TArgs>(Args)...);
    }
}
