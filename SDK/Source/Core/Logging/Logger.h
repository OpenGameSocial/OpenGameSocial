#pragma once

#include <string>
#include <fmt/format.h>

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

            auto LogString = fmt::vformat(Format, fmt::make_format_args(Args...));

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

#define FORWARD_LOG_LEVEL(Level)                                                     \
    template <typename ... TArgs>                                                    \
    void Level(std::string_view Format, TArgs&& ... Args)                            \
    {                                                                                \
        Log<OGS_##Level>(Format, std::forward<TArgs>(Args) ...);                     \
    }                                                                                \
    template <typename ... TArgs>                                                    \
    void Level(const char* Format, TArgs&& ... Args)                                 \
    {                                                                                \
        Log<OGS_##Level>(std::string_view(Format), std::forward<TArgs>(Args) ...);   \
    }

    template <OGS_ELogLevel MinCompileLevel = OGS_Verbose, OGS_ELogLevel MaxCompileLevel = OGS_Critical>
    class TLogCategory final
    {
    public:
        explicit TLogCategory(const std::string_view InCategoryName) :
            CategoryName(InCategoryName)
        {}

        template <OGS_ELogLevel Level, typename... TArgs>
        void Log(std::string_view Format, TArgs&&... Args)
        {
            if constexpr (MinCompileLevel <= Level && MaxCompileLevel >= Level)
            {
                std::string_view LogLevelStr(LevelToString(Level));
                std::string FormattedString = fmt::vformat(Format, fmt::make_format_args(Args...));
                auto LogString = fmt::format("{}: [{}] {}", CategoryName,
                                             LogLevelStr,
                                             FormattedString);

                CLogger::Get().Log(Level, LogString);
            }
        }

        FORWARD_LOG_LEVEL(Verbose)
        FORWARD_LOG_LEVEL(Info)
        FORWARD_LOG_LEVEL(Warning)
        FORWARD_LOG_LEVEL(Error)
        FORWARD_LOG_LEVEL(Critical)

    private:
        std::string CategoryName;
    };

#undef FORWARD_LOG_LEVEL
}
