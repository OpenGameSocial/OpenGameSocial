#include "StringUtils.h"

#include <windows.h>


namespace OGS::Win64::Internal
{
    std::string WideStringToUtf8(const std::wstring& InString)
    {
        return {};
    }

    std::wstring Utf8StringToWideString(const std::string& InString)
    {
        std::wstring Result;

        const int32_t InSize = static_cast<int32_t>(InString.size());

        if (InSize == 0)
        {
            return Result;
        }

        const int32_t RequiredLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                                           InString.c_str(), InSize,
                                                           nullptr, 0);

        if (RequiredLength == 0)
        {
            return Result;
        }

        Result.resize(RequiredLength);
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, InString.c_str(),
                            InSize, Result.data(), RequiredLength);

        return Result;
    }
}
