#pragma once

#include <string>
#include <cstdint>
#include <optional>

#include "Core/Common/Codegen.h"


struct SWeatherForecast
{
public:
    SERIALIZABLE()
    std::string Date
        {"Hello"};

    SERIALIZABLE()
    int32_t TemperatureC = 0;

    SERIALIZABLE()
    int32_t TemperatureF{0};

    SERIALIZABLE()
    std::optional<std::string> Summary;

    friend void to_json(nlohmann::json& output, const SWeatherForecast& input);
    friend void from_json(const nlohmann::json& input, SWeatherForecast& output);
};
