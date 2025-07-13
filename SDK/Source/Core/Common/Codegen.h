#pragma once


#include <memory>
#include <vector>

// Mark field as serializable
#define SERIALIZABLE()

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

class CAutoInitRegistry
{
public:
    void RunInit()
    {
        if (bInitialized)
        {
            return;
        }

        RegisterInitializers();

        bInitialized = true;

        for (const auto& Init : Initializers)
        {
            Init->Run();
        }
    }

private:
    void Register(CAutoInit* Initializer)
    {
        Initializers.push_back(std::unique_ptr<CAutoInit>(Initializer));
    }

    // Generated
    void RegisterInitializers();

private:
    bool bInitialized = false;
    std::vector<std::unique_ptr<CAutoInit>> Initializers;
};
