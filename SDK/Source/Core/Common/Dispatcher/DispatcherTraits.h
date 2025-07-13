#pragma once

#include <type_traits>

#include "DispatcherOperations.h"


namespace OGS::Dispatcher
{
    class COperationResult;

    namespace Traits
    {
        template <typename TOperation, bool IsAsync, bool IsQuery>
        struct TOperationResult;
    }

    template <typename TOperation>
    struct TOperationTraits
    {
    public:
        static inline constexpr bool IsOperation = std::is_base_of_v<SOperation, TOperation>;
        static inline constexpr bool IsAsync = std::is_base_of_v<SAsyncOperation, TOperation>;

        static inline consteval bool IsCommand = std::is_base_of_v<SCommand, TOperation>;
        static inline constexpr bool IsQuery = std::is_base_of_v<SQuery, TOperation>;

        using Result = typename Traits::TOperationResult<TOperation, IsAsync, IsQuery>::TResult;
    };

    namespace Traits
    {
        template <typename TOperation>
        struct TOperationResult<TOperation, true, true>
        {
            using TResult = typename TOperation::SResult;
        };

        template <typename TOperation>
        struct TOperationResult<TOperation, false, true>
        {
            using TResult = typename TOperation::SResult;
        };

        template <typename TOperation>
        struct TOperationResult<TOperation, true, false>
        {
            using TResult = typename TOperation::SResult;
        };

        template <typename TOperation>
        struct TOperationResult<TOperation, false, false>
        {
            using TResult = COperationResult;
        };
    }
}
