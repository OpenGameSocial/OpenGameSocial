#pragma once

#include <memory>
#include <unordered_map>

#include "Core/Common/TypeUtils.h"


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
        using ServicePtr = std::shared_ptr<CServiceBase>;

    public:
        static CServiceContainer& Get();

        template <typename TService>
        void RegisterService()
        {
            static_assert(std::is_base_of_v<CServiceBase, TService>);

            ServiceMap.emplace(GetTypeId<TService>(), std::make_shared<TService>());
        }

        template <typename TService>
        std::shared_ptr<TService> GetService()
        {
            static_assert(std::is_base_of_v<CServiceBase, TService>);

            const auto Found = ServiceMap.find(GetTypeId<TService>());

            if (Found == ServiceMap.end())
            {
                return {};
            }

            return std::static_pointer_cast<TService>(Found->second);
        }

    private:
        std::unordered_map<TypeId, ServicePtr> ServiceMap;
    };
}
