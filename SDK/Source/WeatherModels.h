#pragma once

#include <string>
#include <cstdint>

#include "Core/Common/Codegen.h"


class WeatherResponse
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

    SERIALIZE_METHODS(WeatherResponse)
};

class WeatherRequest
{
public:
    SERIALIZABLE()
    int32_t Count = 0;

    SERIALIZE_METHODS(WeatherRequest)
};
