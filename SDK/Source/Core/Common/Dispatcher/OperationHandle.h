#pragma once

#include <functional>
#include <memory>


namespace OGS::Dispatcher
{
    template <typename TResult>
    class TContinuation
    {
    public:
        using ContinuationFunc = std::function<void(TResult)>;

        static TContinuation CreateStatic(void (*Func)(TResult))
        {
            return TContinuation(Func);
        }

        template <typename TLambda>
        static TContinuation CreateStaticLambda(TLambda&& Lambda)
        {
            return TContinuation(Lambda);
        }

        template <typename TOwner, typename TMethod>
        static TContinuation CreateShared(TOwner&& Owner, TMethod&& InMethod)
        {
            return TContinuation([Accessor = CreateOwnerAccessor(Owner), InMethod](TResult Result)
            {
                if (!Accessor)
                {
                    return;
                }

                if (auto OwnerPtr = Accessor())
                {
                    (OwnerPtr.get()->*InMethod)(Result);
                }
            });
        }

        template <typename TOwner, typename TFunc>
        static TContinuation CreateSharedLambda(const TOwner& Owner, TFunc&& Func)
        {
            return TContinuation([Accessor = CreateOwnerAccessor(Owner), Func](TResult Result)
            {
                if (!Accessor)
                {
                    return;
                }

                if (auto OwnerPtr = Accessor())
                {
                    Func(Result);
                }
            });
        }

        void Execute(TResult Result)
        {
            if (!Continuation)
            {
                return;
            }

            Continuation(Result);
        }

    private:
        explicit TContinuation(ContinuationFunc InContinuation) :
        Continuation(InContinuation)
        {}

        template <typename TClass>
        static std::function<std::shared_ptr<TClass>()> CreateOwnerAccessor(const std::shared_ptr<TClass>& Owner)
        {
            std::weak_ptr<TClass> WeakOwner = std::weak_ptr<TClass>(Owner);

            return [WeakOwner]
            {
                return WeakOwner.lock();
            };
        }

        template <typename TClass>
        static std::function<std::shared_ptr<TClass>()> CreateOwnerAccessor(const std::weak_ptr<TClass>& WeakOwner)
        {
            return [WeakOwner]
            {
                return WeakOwner.lock();
            };
        }

        template <typename TOwner>
        static std::function<std::shared_ptr<const TOwner>()> CreateOwnerAccessor(
            const std::enable_shared_from_this<TOwner>* Owner)
        {
            if (Owner == nullptr)
            {
                return {};
            }

            auto WeakOwner = Owner->weak_from_this();

            return [WeakOwner]
            {
                return WeakOwner.lock();
            };
        }

        template <typename TOwner>
        static std::function<std::shared_ptr<TOwner>()> CreateOwnerAccessor(
            std::enable_shared_from_this<TOwner>* Owner)
        {
            if (Owner == nullptr)
            {
                return {};
            }

            auto WeakOwner = Owner->weak_from_this();

            return [WeakOwner]
            {
                return WeakOwner.lock();
            };
        }

    private:
        ContinuationFunc Continuation;
    };

    template <typename TResult>
    class TOperationHandle
    {
    public:

    private:

    };
}
