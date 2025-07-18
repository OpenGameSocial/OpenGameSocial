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
struct TFieldRealType;

template <typename T>
struct TFieldRealType<std::optional<T>>
{
    using Type = T;
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
