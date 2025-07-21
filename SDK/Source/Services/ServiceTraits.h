#pragma once

#include <type_traits>


namespace OGS::Services::Traits
{
    template <typename T>
    class HasSetupFunc
    {
    private:
        template <typename U>
        static auto Test(int) -> decltype(&U::Setup, std::true_type{});

        template <typename>
        static std::false_type Test(...);

    public:
        static constexpr bool value = decltype(Test<T>(0))::value;
    };

    template <typename T>
    struct SetupTraits;

    template <typename C, typename R, typename... Args>
    struct SetupTraits<R(C::*)(Args...)>
    {
        using ReturnType = R;
        using ClassType = C;
        using ArgsTuple = std::tuple<Args...>;

        static constexpr size_t Arity = sizeof...(Args);

        template <size_t N>
        using ArgType = std::tuple_element_t<N, ArgsTuple>;
    };

    template <typename C, typename R, typename... Args>
    struct SetupTraits<R(C::*)(Args...) const>
    {
        using ReturnType = R;
        using ClassType = C;
        using ArgsTuple = std::tuple<Args...>;

        static constexpr size_t Arity = sizeof...(Args);

        template <size_t N>
        using ArgType = std::tuple_element_t<N, ArgsTuple>;
    };

    template <typename T>
    struct ExtractServiceType;

    template <typename T>
    struct ExtractServiceType
    {
        using Type = T;
    };

    template <typename T>
    struct ExtractServiceType<std::weak_ptr<T>>
    {
        using Type = T;
    };

    template <typename T>
    constexpr bool HasSetupFunc_v = HasSetupFunc<T>::value;
}
