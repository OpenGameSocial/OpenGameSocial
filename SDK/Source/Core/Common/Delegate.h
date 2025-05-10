#pragma once

#include <functional>
#include <memory>


namespace OGS
{
    template <typename ... TArgs>
    class TDelegate
    {
    public:
        TDelegate() = default;

        bool Execute(TArgs ... args)
        {
            return IsBound() && Callable(std::forward<TArgs>(args) ...);
        }

        [[nodiscard]] bool IsBound() const
        {
            return static_cast<bool>(Callable);
        }

        static TDelegate CreateStatic(void (*InCallable)(TArgs ...))
        {
            return TDelegate([InCallable](TArgs ... Args)
            {
                InCallable(std::forward<TArgs>(Args) ...);
                return true;
            });
        }

        template <typename TCallable>
        static TDelegate CreateStaticLambda(TCallable&& InCallable)
        {
            return TDelegate([InCallable = std::forward<TCallable>(InCallable)](TArgs ... Args)
            {
                InCallable(std::forward<TArgs>(Args) ...);
                return true;
            });
        }

        template <typename TOwner>
        static TDelegate CreateShared(const std::shared_ptr<TOwner>& Owner, void (TOwner::*InMethod)(TArgs ...))
        {
            std::weak_ptr<TOwner> WeakOwner = std::weak_ptr<TOwner>(Owner);

            auto Callable = [WeakOwner, InMethod](TArgs ... Args)
            {
                if (std::shared_ptr<TOwner> OwnerPtr = WeakOwner.lock())
                {
                    (OwnerPtr.get()->*InMethod)(std::forward<TArgs>(Args) ...);
                    return true;
                }

                return false;
            };

            return TDelegate(Callable);
        }

        template <typename TOwner, typename TCallable>
        static TDelegate CreateSharedLambda(const std::shared_ptr<TOwner>& Owner, TCallable&& InCallable)
        {
            std::weak_ptr<TOwner> WeakOwner = std::weak_ptr<TOwner>(Owner);

            auto Callable = [WeakOwner, InCallable = std::forward<TCallable>(InCallable)](TArgs ... Args)
            {
                if (std::shared_ptr<TOwner> _ = WeakOwner.lock())
                {
                    InCallable(std::forward<TArgs>(Args) ...);
                    return true;
                }

                return false;
            };

            return TDelegate(Callable);
        }

    private:
        explicit TDelegate(std::function<bool(TArgs ...)>&& InCallable)
            : Callable(std::move(InCallable))
        {
        }

    private:
        std::function<bool(TArgs ...)> Callable;
    };

    using CSimpleDelegate = TDelegate<>;
}
