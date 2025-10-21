#pragma once

#include <atomic>
#include <memory>


namespace OGS
{
    namespace Details
    {
        template <typename TData>
        struct SNonBlockingQueueNode final
        {
        public:
            explicit SNonBlockingQueueNode(TData&& InData)
                : Data(std::move(InData))
            {
            }

            ~SNonBlockingQueueNode() = default;

            SNonBlockingQueueNode(const SNonBlockingQueueNode& other) = delete;
            SNonBlockingQueueNode(SNonBlockingQueueNode&& other) noexcept = delete;
            SNonBlockingQueueNode& operator=(const SNonBlockingQueueNode& other) = delete;
            SNonBlockingQueueNode& operator=(SNonBlockingQueueNode&& other) noexcept = delete;

        public:
            std::atomic<SNonBlockingQueueNode*> Next;
            TData Data;
        };
    }

    template <typename TData>
    class TNonBlockingQueue
    {
    public:
        TNonBlockingQueue() : Head(nullptr), Tail(nullptr)
        {
        }

        TNonBlockingQueue(const TNonBlockingQueue& other) = delete;
        TNonBlockingQueue(TNonBlockingQueue&& other) noexcept = delete;
        TNonBlockingQueue& operator=(const TNonBlockingQueue& other) = delete;
        TNonBlockingQueue& operator=(TNonBlockingQueue&& other) noexcept = delete;

        void Enqueue(TData&& InData)
        {
            auto* NewNode = new CNode(std::move(InData));
            NewNode->Next.store(nullptr);

            auto* OldTail = Tail.load(std::memory_order_acquire);

            while (true)
            {
                if (OldTail == nullptr)
                {
                    if (Tail.compare_exchange_weak(OldTail, NewNode,
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed))
                    {
                        auto* Expected = OldTail;
                        Head.compare_exchange_strong(Expected, NewNode,
                                                     std::memory_order_release,
                                                     std::memory_order_relaxed);
                        return;
                    }

                    continue;
                }

                auto* Next = OldTail->Next.load();
                if (Next == nullptr)
                {
                    if (OldTail->Next.compare_exchange_weak(Next, NewNode,
                                                            std::memory_order_release,
                                                            std::memory_order_relaxed))
                    {
                        Tail.compare_exchange_weak(OldTail, NewNode,
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed);
                        return;
                    }
                }
                else
                {
                    Tail.compare_exchange_weak(OldTail, Next,
                                               std::memory_order_release,
                                               std::memory_order_relaxed);
                }

                OldTail = Tail.load(std::memory_order_relaxed);
            }
        }

        bool TryDequeue(TData& OutData)
        {
            while (true)
            {
                auto* OldHead = Head.load(std::memory_order_relaxed);

                if (OldHead == nullptr)
                {
                    return false;
                }

                auto* Next = OldHead->Next.load(std::memory_order_acquire);

                if (Head.compare_exchange_weak(OldHead, Next,
                                               std::memory_order_release,
                                               std::memory_order_relaxed))
                {
                    OutData = std::move(OldHead->Data);

                    if (Next == nullptr)
                    {
                        Tail.compare_exchange_weak(OldHead, nullptr,
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed);
                    }

                    delete OldHead;

                    return true;
                }
            }
        }

        ~TNonBlockingQueue()
        {
            auto* Node = Head.load();

            while (Node)
            {
                const auto* Temp = Node;
                Node = Node->Next.load();
                delete Temp;
            }
        }

    private:
        using CNode = Details::SNonBlockingQueueNode<TData>;

        std::atomic<CNode*> Head;
        std::atomic<CNode*> Tail;
    };
}
