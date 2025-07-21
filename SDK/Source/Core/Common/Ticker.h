#pragma once

#include <unordered_set>


namespace OGS
{
    class CTicker
    {
    public:
        CTicker();
        virtual ~CTicker();

        virtual void Tick(double ElapsedSeconds) = 0;
    };

    class CTickerManager final
    {
    public:
        static CTickerManager& Get();

        void AddTicker(CTicker* Ticker);
        void RemoveTicker(CTicker* Ticker);

        void Tick(double ElapsedSeconds) const;
    private:
        std::unordered_set<CTicker*> Tickers;
    };
}
