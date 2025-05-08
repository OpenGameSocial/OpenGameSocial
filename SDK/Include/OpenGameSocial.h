#pragma once

#include <cstdint>

#include "OGS/Core.h"
#include "OGS/Result.h"

#include "OGS/Interfaces/AccountTypes.h"
#include "OGS/Interfaces/Account.h"

#include "OGS/Interfaces/Platform.h"


/**
 * OpenGameSocial initialization option
 */
struct OGS_Init_Options
{
    /**
     * [Required] Maximum threads in thread pool
     */
    uint32_t ThreadPoolSize;
};

/**
 * Initialize Open Game Social. This function should be called before any other OGS function.
 * @param Options Options
 */
OGS_API void OGS_Init(const OGS_Init_Options* Options);