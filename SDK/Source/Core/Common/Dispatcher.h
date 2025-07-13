#pragma once


namespace OGS
{
    class CDispatcher
    {
    public:
        static CDispatcher& Get();

        template <typename TOperation>
        void Run()
        {
            
        }
    };
}
