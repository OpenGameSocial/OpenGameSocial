#pragma once

#include "Core/Common/Codegen.h"


namespace OGS::Services::Account
{
    class CAuthenticate
    {
    public:
        static constexpr const char* Endpoint = "Authenticate";

        class CRequest
        {
        public:
            SERIALIZABLE()
            std::string Provider;

            SERIALIZABLE()
            std::string Data;

            SERIALIZE_METHODS(CRequest)
        };

        class CResponse
        {
        public:
            SERIALIZABLE()
            std::string Token;

            SERIALIZE_METHODS(CResponse)
        };
    };
}
