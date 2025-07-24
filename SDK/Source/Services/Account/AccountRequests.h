#pragma once

#include "Core/Common/Codegen.h"
#include "Core/Common/Date.h"


namespace OGS::Services::Account
{
    class CAuthenticate
    {
    public:
        static constexpr std::string_view Endpoint = "Authenticate";

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
            CDateTime ValidUntil;

            SERIALIZABLE()
            std::string Token;

            SERIALIZABLE()
            std::string TokenType;

            SERIALIZE_METHODS(CResponse)
        };
    };
}
