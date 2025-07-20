#pragma once

#include <string>

#include "Core/Common/Codegen.h"


class CTestAuth
{
public:
    static constexpr const char* Endpoint = "TestAuth";

    class CRequest
    {};

    class CResponse
    {
    public:
        SERIALIZABLE()
        std::string Message;

        SERIALIZE_METHODS(CResponse)
    };
};
