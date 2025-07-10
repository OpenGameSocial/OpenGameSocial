#pragma once

#include <cstdint>

#include "StringUtils.h"

#if defined(__clang__) || defined(__GNUC__)
#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define FUNCTION_SIGNATURE __FUNCSIG__
#else
#define FUNCTION_SIGNATURE __func__
#endif

namespace OGS
{
    using TypeId = uint32_t;

    template <typename>
    constexpr TypeId GetTypeId()
    {
        static constexpr TypeId Result = JoaatHash(FUNCTION_SIGNATURE);
        return Result;
    }
}
