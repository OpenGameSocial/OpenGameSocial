#pragma once

#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <nlohmann/json.hpp>


namespace OGS
{
    using CTimeSpan = std::chrono::microseconds;
    using CDateTime = std::chrono::sys_time<CTimeSpan>;

    inline CDateTime UtcNow()
    {
        using namespace std::chrono;
        return floor<microseconds>(system_clock::now());
    }

    inline CDateTime Now()
    {
        using namespace date;

        const auto Zoned = make_zoned(std::chrono::current_zone(), UtcNow());
        return Zoned.get_sys_time();
    }
}

template <>
struct nlohmann::adl_serializer<OGS::CDateTime>
{
    static void to_json(json& j, const OGS::CDateTime& DateTime)
    {
        j = date::format("%FT%T", DateTime);
    }

    static void from_json(const json& j, OGS::CDateTime& DateTime)
    {
        std::string String = j;
        std::istringstream Stream{String};

        Stream >> date::parse("%FT%T%Ez", DateTime);
    }
};