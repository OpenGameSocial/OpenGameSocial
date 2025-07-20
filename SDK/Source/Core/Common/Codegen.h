#pragma once

#include <optional>
#include <vector>

#include <nlohmann/json.hpp>


// Mark class/struct member as serializable
#define SERIALIZABLE()

#define SERIALIZE_METHODS(type)                                      \
    friend void to_json(nlohmann::json& output, const type& input);  \
    friend void from_json(const nlohmann::json& input, type& output);

template <typename T>
struct TRealType;

template <typename T>
struct TRealType
{
    using Type = std::remove_const_t<std::remove_reference_t<std::remove_pointer_t<T>>>;
};

template <typename T>
struct TRealType<std::optional<T>>
{
    using Type = std::remove_const_t<std::remove_reference_t<std::remove_pointer_t<T>>>;
};

template <typename T>
struct TRealType<std::shared_ptr<T>>
{
    using Type = std::remove_const_t<std::remove_reference_t<std::remove_pointer_t<T>>>;
};

template <typename T>
struct TRealType<std::enable_shared_from_this<T>>
{
    using Type = std::remove_const_t<std::remove_reference_t<std::remove_pointer_t<T>>>;
};

class CAutoInitable
{
public:
    virtual ~CAutoInitable() = default;

    virtual void Run() = 0;
};

class CAutoInit
{
public:
    explicit CAutoInit(CAutoInitable* InInitable) :
    Initable(InInitable)
    {}

    void Run() const
    {
        Initable->Run();
    }

private:
    CAutoInitable* Initable = nullptr;
};

void RunAutoInit();
