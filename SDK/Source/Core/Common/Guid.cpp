#include "Guid.h"

#include "OGS/Core.h"


OGS::CGuid::CGuid()
{
    std::random_device Device;
    std::mt19937 Generator(Device());
    std::uniform_int_distribution Distribution(std::numeric_limits<uint32_t>::min(),
                                               std::numeric_limits<uint32_t>::max());

    auto* Data32 = reinterpret_cast<uint32_t*>(Data.data());

    for (int Round = 0; Round < 4; ++Round)
    {
        Data32[Round] = Distribution(Generator);
    }

    Data[6] = (Data[6] & 0x0F) | 0x40;
    Data[8] = (Data[8] & 0x3F) | 0x80;
}

std::optional<OGS::CGuid> OGS::CGuid::FromString(const std::string& Str)
{
    CGuid Guid;

    auto ScanResult = std::sscanf(Str.c_str(),
                               "%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                               &Guid.Data[0], &Guid.Data[1], &Guid.Data[2], &Guid.Data[3],
                               &Guid.Data[4], &Guid.Data[5], &Guid.Data[6], &Guid.Data[7],
                               &Guid.Data[8], &Guid.Data[9], &Guid.Data[10], &Guid.Data[11],
                               &Guid.Data[12], &Guid.Data[13], &Guid.Data[14], &Guid.Data[15]);

    if (ScanResult != 16)
    {
        return std::nullopt;
    }

    return Guid;
}

std::string OGS::CGuid::ToString() const
{
    // 16 bytes * 2 chars per byte + null
    char Buffer[37];

    std::snprintf(Buffer, sizeof(Buffer),
             "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             Data[0], Data[1], Data[2], Data[3],
             Data[4], Data[5], Data[6], Data[7],
             Data[8], Data[9], Data[10], Data[11],
             Data[12], Data[13], Data[14], Data[15]);

    return Buffer;
}

const std::array<uint8_t, 16>& OGS::CGuid::GetData() const
{
    return Data;
}

bool OGS::CGuid::operator==(const CGuid& Other) const
{
    return Data == Other.Data;
}

bool OGS::CGuid::operator!=(const CGuid& Other) const
{
    return !(*this == Other);
}
