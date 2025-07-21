#pragma once

#include "Core/Common/Codegen.h"


namespace OGS::Services::Signal
{
    namespace DataModels
    {
        class CTransport
        {
        public:
            SERIALIZABLE()
            std::string Transport;

            SERIALIZABLE()
            std::vector<std::string> TransferFormats;

            SERIALIZE_METHODS(CTransport)
        };
    }

    class CNegotiate
    {
    public:
        class CRequest
        {
        public:
            SERIALIZE_METHODS(CRequest)
        };

        class CResponse
        {
        public:
            SERIALIZABLE()
            std::string ConnectionId;

            SERIALIZABLE()
            std::optional<std::string> ConnectionToken;

            SERIALIZABLE()
            std::vector<DataModels::CTransport> AvailableTransports;

            SERIALIZABLE()
            int32_t NegotiateVersion = 0;

            SERIALIZE_METHODS(CResponse)
        };
    };
}
