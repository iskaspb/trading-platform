#pragma once
#include "SymbolProviderConfig.h"

#include <app/LogConfig.h>

struct AppConfig
{
    app::LogConfig log;
    SymbolProviderConfig symbolProvider;
};
DEFINE_CONFIG(AppConfig, log, symbolProvider);
