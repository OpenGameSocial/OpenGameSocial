#pragma once


namespace OGS::Dispatcher
{
    struct SOperation
    {};

    struct SAsyncOperation
    {};

    struct SCommand : SOperation
    {};

    struct SQuery : SOperation
    {};

    struct SAsyncCommand : SCommand, SAsyncOperation
    {};

    struct SAsyncQuery : SQuery, SAsyncOperation
    {};
}
