#pragma once

#include <functional>
#include <memory>


namespace OGS
{
    class CDelegateHandle final
    {
    public:
        CDelegateHandle() = default;

        CDelegateHandle(int32_t InIndex, int32_t InSerial)
            : Index(InIndex),
              Serial(InSerial)
        {
        }

        void Reset()
        {
            *this = {};
        }

        [[nodiscard]] bool IsValid() const
        {
            return Index != -1;
        }

        [[nodiscard]] int32_t GetIndex() const
        {
            return Index;
        }

        [[nodiscard]] int32_t GetSerial() const
        {
            return Serial;
        }

    private:
        int32_t Index = -1;
        int32_t Serial = 0;
    };

    template <typename ... TArgs>
    class TMulticastDelegate
    {
        struct SCallableSlot
        {
        public:
            int32_t Serial = 0;
            std::function<bool(TArgs ...)> Callable;
        };

    public:
        void Broadcast(TArgs ... args)
        {
            for (size_t Index = InvocationList.size(); Index-- > 0;)
            {
                SCallableSlot& Slot = InvocationList[Index];

                // Empty slot, erase it
                if (!Slot.Callable)
                {
                    InvocationList.erase(InvocationList.begin() + Index);
                    continue;
                }

                if (Slot.Callable(std::forward<TArgs>(args) ...))
                {
                    continue;
                }

                // Callable has failed owner lifetime check, erase it
                InvocationList.erase(InvocationList.begin() + Index);
            }
        }

        bool Remove(CDelegateHandle& InHandle)
        {
            if (InHandle.IsValid() && Remove(InHandle.GetIndex(), InHandle.GetSerial()))
            {
                InHandle.Reset();
                return true;
            }

            return false;
        }

        bool Remove(const CDelegateHandle& InHandle)
        {
            return InHandle.IsValid() && Remove(InHandle.GetIndex(), InHandle.GetSerial());
        }

        CDelegateHandle AddStatic(void (*InCallable)(TArgs ...))
        {
            return EmplaceSlot([InCallable](TArgs ... Args)
            {
                InCallable(std::forward<TArgs>(Args) ...);
                return true;
            });
        }

        template <typename TCallable>
        CDelegateHandle AddStaticLambda(TCallable&& InCallable)
        {
            return EmplaceSlot([InCallable = std::forward<TCallable>(InCallable)](TArgs ... Args)
            {
                InCallable(std::forward<TArgs>(Args) ...);
                return true;
            });
        }

        template <typename TOwner>
        CDelegateHandle AddShared(const std::shared_ptr<TOwner>& Owner, void (TOwner::*InMethod)(TArgs ...))
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

            return EmplaceSlot(Callable);
        }

        template <typename TOwner, typename TCallable>
        CDelegateHandle AddSharedLambda(const std::shared_ptr<TOwner>& Owner, TCallable&& InCallable)
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

            return EmplaceSlot(Callable);
        }

    private:
        CDelegateHandle EmplaceSlot(std::function<bool(TArgs ...)>&& InCallable)
        {
            const auto Serial = ++SerialCounter;
            const auto Index = static_cast<int32_t>(InvocationList.size());

            InvocationList.emplace_back(Serial, std::move(InCallable));

            return {Index, Serial};
        }

        bool Remove(int32_t InIndex, int32_t InSerial)
        {
            if (InIndex < 0)
            {
                return false;
            }

            if (static_cast<size_t>(InIndex) >= InvocationList.size())
            {
                return false;
            }

            if (InvocationList[InIndex].Serial != InSerial)
            {
                return false;
            }

            InvocationList.erase(InvocationList.begin() + InIndex);

            return true;
        }

    private:
        int32_t SerialCounter = 0;
        std::vector<SCallableSlot> InvocationList;
    };

    using CSimpleMulticastDelegate = TMulticastDelegate<>;
}
