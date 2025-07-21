#pragma once

#include <functional>
#include <memory>


namespace OGS
{
    class CDelegateHandle final
    {
    public:
        CDelegateHandle() = default;

        CDelegateHandle(uint32_t InDelegateSerial, uint32_t InSerial)
            : DelegateSerial(InDelegateSerial), Serial(InSerial)
        {
        }

        void Invalidate()
        {
            DelegateSerial = InvalidSerial;
            Serial = InvalidSerial;
        }

        [[nodiscard]] bool IsValid() const
        {
            return DelegateSerial != InvalidSerial && Serial != InvalidSerial;
        }

        [[nodiscard]] uint32_t GetDelegateSerial() const
        {
            return DelegateSerial;
        }

        [[nodiscard]] uint32_t GetSerial() const
        {
            return Serial;
        }

        [[nodiscard]] uint64_t ToUnsignedInt64() const
        {
            return static_cast<uint64_t>(Serial) << 32 | DelegateSerial;
        }

        [[nodiscard]] static CDelegateHandle FromUnsignedInt64(uint64_t InValue)
        {
            return {static_cast<uint32_t>(InValue & 0xFFFFFFFF),
                    static_cast<uint32_t>(InValue >> 32)};
        }

    public:
        static constexpr uint32_t InvalidSerial = std::numeric_limits<uint32_t>::max();

    private:
        uint32_t DelegateSerial = InvalidSerial;
        uint32_t Serial = InvalidSerial;
    };

    template <typename ... TArgs>
    class TMulticastDelegate
    {
        struct SCallableSlot
        {
        public:
            [[nodiscard]] bool IsValid() const
            {
                return Serial != CDelegateHandle::InvalidSerial && Callable;
            }

            void Invalidate()
            {
                Serial = CDelegateHandle::InvalidSerial;
                Callable = {};
            }

        public:
            uint32_t Serial = CDelegateHandle::InvalidSerial;
            std::function<bool(TArgs ...)> Callable;
        };

    public:
        void Broadcast(TArgs ... args)
        {
            size_t InvalidCount = 0;

            for (size_t Index = InvocationList.size(); Index-- > 0;)
            {
                SCallableSlot& Slot = InvocationList[Index];

                // Empty slot, erase it
                if (!Slot.Callable)
                {
                    ++InvalidCount;
                    continue;
                }

                if (Slot.Callable(std::forward<TArgs>(args) ...))
                {
                    continue;
                }

                Slot.Invalidate();
                ++InvalidCount;
            }

            if (InvalidCount == 0)
            {
                return;
            }

            InvocationList.erase(
                std::remove_if(InvocationList.begin(), InvocationList.end(),
                               [](const SCallableSlot& Slot)
                               {
                                   return !Slot.Callable;
                               }),
                InvocationList.end()
            );
        }

        bool Remove(CDelegateHandle& InHandle)
        {
            if (InHandle.IsValid() && Remove(InHandle.GetDelegateSerial(), InHandle.GetSerial()))
            {
                InHandle.Invalidate();
                return true;
            }

            return false;
        }

        bool Remove(const CDelegateHandle& InHandle)
        {
            return InHandle.IsValid() && Remove(InHandle.GetDelegateSerial(), InHandle.GetSerial());
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
                if (auto Lock = WeakOwner.lock())
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

            InvocationList.emplace_back(Serial, std::move(InCallable));

            return {DelegateSerial, Serial};
        }

        bool Remove(uint32_t InDelegateSerial, uint32_t InSerial)
        {
            if (InDelegateSerial != DelegateSerial)
            {
                // It is handle from different delegate
                return false;
            }

            for (size_t Index = 0; Index < InvocationList.size(); ++Index)
            {
                auto& Slot = InvocationList[Index];
                
                if (Slot.Serial == InSerial)
                {
                    Slot.Invalidate();
                    return true;
                }
            }

            return false;
        }

    private:
        static inline std::atomic<uint32_t> DelegateSerialCounter = 0;

        uint32_t DelegateSerial = ++DelegateSerialCounter;
        uint32_t SerialCounter = 0;

        std::vector<SCallableSlot> InvocationList;
    };

    using CSimpleMulticastDelegate = TMulticastDelegate<>;
}
