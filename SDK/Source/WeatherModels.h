#pragma once

#include <cstdint>
#include <string>

#include "Core/Common/Codegen.h"


class CWeatherResponseItem
{
public:
    SERIALIZABLE()
    std::string Date;

    SERIALIZABLE()
    int32_t TemperatureC = 0;

    SERIALIZABLE()
    int32_t TemperatureF = 0;

    SERIALIZABLE()
    std::optional<std::string> Summary;

    SERIALIZE_METHODS(CWeatherResponseItem)
};

class CWeather
{
public:
    static constexpr std::string_view Endpoint = "WeatherForecast";

    class CRequest
    {
    public:
        SERIALIZABLE()
        int32_t Count = 0;

        SERIALIZE_METHODS(CRequest)
    };

    class CResponse
    {
    public:
        SERIALIZABLE()
        std::vector<CWeatherResponseItem> Data;

        SERIALIZE_METHODS(CResponse)
    };
};
