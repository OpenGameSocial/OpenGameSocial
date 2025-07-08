#pragma once

#include <string>


namespace OGS::Win64::Internal
{
    std::string WideStringToUtf8(const std::wstring& InString);
    std::wstring Utf8StringToWideString(const std::string& InString);
}
