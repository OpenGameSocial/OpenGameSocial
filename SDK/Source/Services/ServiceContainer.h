#pragma once


#include <functional>
#include <memory>
#include <unordered_map>

#include "Core/Common/Codegen.h"
#include "Core/Common/TypeUtils.h"
#include "Services/ServiceTraits.h"

#define SERVICE_REGISTER(serviceTypeName) \
    new OGS::Services::CServiceInitializer<serviceTypeName>()

namespace OGS::Services
{
    class CServiceBase : public std::enable_shared_from_this<CServiceBase>
    {
    public:
        virtual ~CServiceBase() = default;

        virtual void Init() = 0;
        virtual void DeInit() = 0;
    };

    class CServiceContainer final
    {
        using CServicePtr = std::shared_ptr<CServiceBase>;

        struct SRegisteredService
        {
        public:
            CServicePtr Service;
            std::function<void(CServiceBase*)> Setup;
        };

    public:
        static CServiceContainer& Get();

        void Init();
        void DeInit();

        template <typename TService>
        void RegisterService()
        {
            static_assert(std::is_base_of_v<CServiceBase, TService>);

            SRegisteredService RegisteredService
            {
                .Service = std::make_shared<TService>(),
                .Setup = [](auto)
                {}
            };

            if constexpr (Traits::HasSetupFunc_v<TService>)
            {
                RegisteredService.Setup = CreateSetupFunction<TService>();
            }

            ServiceMap.emplace(GetTypeId<TService>(), std::move(RegisteredService));
        }

        template <typename TService>
        static std::shared_ptr<TService> GetService()
        {
            static_assert(std::is_base_of_v<CServiceBase, TService>);

            auto& Container = Get();

            const auto Found = Container.ServiceMap.find(GetTypeId<TService>());

            if (Found == Container.ServiceMap.end())
            {
                return {};
            }

            return std::static_pointer_cast<TService>(Found->second.Service);
        }

        template <typename TService>
        static std::weak_ptr<TService> GetServiceWeak()
        {
            return GetService<TService>();
        }

    private:
        template <typename TService>
        auto GetServiceForInject()
        {
            using ServiceType = typename Traits::ExtractServiceType<TService>::Type;
            return GetService<ServiceType>();
        }

        template <typename... TService>
        auto CreateServiceInjectionTuple()
        {
            return std::make_tuple(GetServiceForInject<TService>()...);
        }

        template <typename TService, typename TTuple, size_t... Is>
        auto CallSetupWithTuple(TService& Service, TTuple&& Tuple, std::index_sequence<Is...>)
        {
            return Service.Setup(std::get<Is>(Tuple)...);
        }

        template <typename TService, size_t... Is>
        auto InjectAndCallSetup(TService& Service, std::index_sequence<Is...>)
        {
            using traits = Traits::SetupTraits<decltype(&TService::Setup)>;
            auto services = CreateServiceInjectionTuple<typename traits::template ArgType<Is>...>();
            return CallSetupWithTuple(Service, services, std::index_sequence<Is...>{});
        }

        template <typename TService>
        std::function<void(CServiceBase*)> CreateSetupFunction()
        {
            constexpr size_t Arity = Traits::SetupTraits<decltype(&TService::Setup)>::Arity;

            return [this](CServiceBase* ServicePtr)
            {
                auto& Service = *static_cast<TService*>(ServicePtr);
                InjectAndCallSetup(Service, std::make_index_sequence<Arity>{});
            };
        }

    private:
        std::unordered_map<TypeId, SRegisteredService> ServiceMap;
    };

    template <typename TService>
    class CServiceInitializer final : public CAutoInitable
    {
    public:
        void Run() override
        {
            CServiceContainer::Get().RegisterService<TService>();
        }
    };
}
