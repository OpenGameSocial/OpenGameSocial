#include "Ticker.h"


OGS::CTicker::CTicker()
{
    CTickerManager::Get().AddTicker(this);
}

OGS::CTicker::~CTicker()
{
    CTickerManager::Get().RemoveTicker(this);
}

OGS::CTickerManager& OGS::CTickerManager::Get()
{
    static CTickerManager Instance;
    return Instance;
}

void OGS::CTickerManager::AddTicker(CTicker* Ticker)
{
    Tickers.emplace(Ticker);
}

void OGS::CTickerManager::RemoveTicker(CTicker* Ticker)
{
    Tickers.erase(Ticker);
}

void OGS::CTickerManager::Tick(double ElapsedSeconds) const
{
    for (auto* Ticker : Tickers)
    {
        Ticker->Tick(ElapsedSeconds);
    }
}
