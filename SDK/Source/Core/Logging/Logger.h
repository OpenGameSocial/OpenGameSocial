#pragma once

#include <string>

#include "Core/Common/Containers/NonBlockingQueue.h"
#include "OpenGameSocial.h"
#include "Core/Common/StringUtils.h"


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

        void Init(const OGS_ELogLevel InMinLevel, const bool bInThreadSafe, const OGS_LogCallback InCallback)
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

                if (Entry.Level == OGS_Critical)
                {
                    DEBUG_BREAK();
                }
            }
        }

        void Log(const OGS_ELogLevel Level, std::string& LogString) const
        {
            if (!Callback || MinLevel > Level)
            {
                return;
            }

            if (!bThreadSafe)
            {
                Callback(Level, LogString.data());

                if (Level == OGS_Critical)
                {
                    DEBUG_BREAK();
                }
            }

            BackQueue->Enqueue({
                .Level = Level,
                .Message = std::move(LogString)
            });
        }

        template <typename... TArgs>
        void LogFormat(OGS_ELogLevel Level, const char* Format, TArgs&&... Args)
        {
            if (!Callback || MinLevel > Level)
            {
                return;
            }

            auto LogString = Printf(Format, std::forward<TArgs>(Args)...);

            Log(LogString);
        }

    private:
        CLogger() : FrontQueue(std::make_unique<CLogQueue>()), BackQueue(std::make_unique<CLogQueue>())
        {
        }

    private:
        using CLogQueue = TNonBlockingQueue<Details::SLogEntry>;
        using CLogQueuePtr = std::unique_ptr<CLogQueue>;

        CLogQueuePtr FrontQueue;
        CLogQueuePtr BackQueue;

        OGS_ELogLevel MinLevel = OGS_Info;
        OGS_LogCallback Callback = nullptr;
        bool bThreadSafe = false;
    };

#define FORWARD_LOG_LEVEL(Level)                                   \
    template <typename ... TArgs>                                  \
    void Level(const char* Format, TArgs&& ... Args)               \
    {                                                              \
        Log<OGS_##Level>(Format, std::forward<TArgs>(Args) ...);   \
    }

    template <OGS_ELogLevel MinCompileLevel = OGS_Verbose, OGS_ELogLevel MaxCompileLevel = OGS_Critical>
    class TLogCategory final
    {
    public:
        explicit TLogCategory(const char* InCategoryName) : CategoryName
            (InCategoryName)
        {
        }

        template <OGS_ELogLevel Level, typename... TArgs>
        void Log(const char* Format, TArgs&&... Args)
        {
            if constexpr (MinCompileLevel <= Level && MaxCompileLevel >= Level)
            {
                auto FormattedString = Printf(Format, std::forward<TArgs>(Args)...);
                auto LogString = Printf("%s: [%s] %s", CategoryName, LevelToString(Level), FormattedString.c_str());

                CLogger::Get().Log(Level, LogString);
            }
        }

        FORWARD_LOG_LEVEL(Verbose)
        FORWARD_LOG_LEVEL(Info)
        FORWARD_LOG_LEVEL(Warning)
        FORWARD_LOG_LEVEL(Error)
        FORWARD_LOG_LEVEL(Critical)

    private:
        const char* CategoryName;
    };

#undef FORWARD_LOG_LEVEL
}
