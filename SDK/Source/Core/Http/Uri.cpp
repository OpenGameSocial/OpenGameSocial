#include "Uri.h"

#include <charconv>

using namespace OGS::Http;


Uri::Uri(std::string_view Uri)
{
    Parse(Uri);
}

Uri::Uri(const std::string& Uri)
{
    Parse(Uri);
}

void Uri::Parse(std::string_view Uri)
{
    Parsed = false;

    SourceUri = Uri;

    const size_t SchemeEnd = Uri.find("://");

    if (SchemeEnd == std::string::npos)
    {
        return;
    }

    Protocol = Uri.substr(0, SchemeEnd);

    size_t AuthorityStart = SchemeEnd + 3;
    size_t PathStart = Uri.find('/', AuthorityStart);
    size_t AuthorityEnd = (PathStart == std::string::npos) ? Uri.size() : PathStart;

    std::string_view authority = Uri.substr(AuthorityStart, AuthorityEnd - AuthorityStart);
    size_t at_pos = authority.find('@');
    if (at_pos != std::string::npos)
    {
        UserInfo = authority.substr(0, at_pos);
        authority = authority.substr(at_pos + 1);
    }

    size_t ColonPos = authority.find(':');
    if (ColonPos != std::string::npos)
    {
        Host = authority.substr(0, ColonPos);
        std::from_chars(authority.data() + ColonPos + 1, authority.data() + authority.size(), Port);
    }
    else
    {
        Host = authority;
    }

    if (PathStart != std::string::npos)
    {
        size_t QueryStart = Uri.find('?', PathStart);
        size_t FragmentStart = Uri.find('#', PathStart);

        Path = Uri.substr(PathStart, QueryStart - PathStart);

        if (QueryStart != std::string::npos)
        {
            Query = Uri.substr(QueryStart + 1, FragmentStart - QueryStart - 1);
        }

        if (FragmentStart != std::string::npos)
        {
            Fragment = Uri.substr(FragmentStart + 1);
        }
    }

    Parsed = true;
}
