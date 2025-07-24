#pragma once

#include <functional>
#include <memory>


namespace OGS
{
    template <typename... TArgs>
    class TDelegate
    {
    public:
        TDelegate() = default;

        bool TryExecute(TArgs... args) const
        {
            return IsBound() && Callable(std::forward<TArgs>(args)...);
        }

        void Execute(TArgs... args) const
        {
            if (!IsBound())
            {
                return;
            }

            Callable(std::forward<TArgs>(args)...);
        }

        [[nodiscard]] bool IsBound() const
        {
            return static_cast<bool>(Callable);
        }

        static TDelegate CreateStatic(void (*InCallable)(TArgs...))
        {
            TDelegate Result;

            Result.BindStatic(InCallable);

            return Result;
        }

        void BindStatic(void (*InCallable)(TArgs...))
        {
            Callable = [InCallable](TArgs... Args)
            {
                InCallable(std::forward<TArgs>(Args)...);
                return true;
            };
        }

        template <typename TCallable>
        static TDelegate CreateStaticLambda(TCallable&& InCallable)
        {
            TDelegate Result;

            Result.BindStaticLambda(std::forward<TCallable>(InCallable));

            return Result;
        }

        template <typename TCallable>
        void BindStaticLambda(TCallable&& InCallable)
        {
            Callable = [InCallable = std::forward<TCallable>(InCallable)](TArgs... Args)
            {
                InCallable(std::forward<TArgs>(Args)...);
                return true;
            };
        }

        template <typename TOwner>
        static TDelegate CreateShared(const std::shared_ptr<TOwner>& Owner, void (TOwner::*InMethod)(TArgs...))
        {
            TDelegate Result;

            Result.BindShared(Owner, InMethod);

            return Result;
        }

        template <typename TOwner, typename... TBoundArgs>
        void BindShared(std::shared_ptr<TOwner> Owner, void (TOwner::*InMethod)(TArgs..., TBoundArgs...), TBoundArgs&& ... BoundArgs)
        {
            std::weak_ptr<TOwner> WeakOwner = std::weak_ptr<TOwner>(Owner);

            Callable = [WeakOwner, InMethod, BoundArgs...](TArgs... Args)
            {
                if (std::shared_ptr<TOwner> OwnerPtr = WeakOwner.lock())
                {
                    (OwnerPtr.get()->*InMethod)(std::forward<TArgs>(Args)..., std::forward<TBoundArgs>(BoundArgs)...);
                    return true;
                }

                return false;
            };
        }

        template <typename TOwner, typename TCallable>
        static TDelegate CreateSharedLambda(const std::shared_ptr<TOwner>& Owner, TCallable&& InCallable)
        {
            TDelegate Result;

            Result.BindSharedLambda(Owner, std::forward<TCallable>(InCallable));

            return Result;
        }

        template <typename TOwner, typename TCallable>
        void BindSharedLambda(const std::shared_ptr<TOwner>& Owner, TCallable&& InCallable)
        {
            std::weak_ptr<TOwner> WeakOwner = std::weak_ptr<TOwner>(Owner);

            Callable = [WeakOwner, InCallable = std::forward<TCallable>(InCallable)](TArgs... Args)
            {
                if (std::shared_ptr<TOwner> _ = WeakOwner.lock())
                {
                    InCallable(std::forward<TArgs>(Args)...);
                    return true;
                }

                return false;
            };
        }

    private:
        std::function<bool(TArgs...)> Callable;
    };

    using CSimpleDelegate = TDelegate<>;
} // namespace OGS
