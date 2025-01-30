#pragma once

#include <config/config.h>

struct MDCaptureConfig
{
    std::string store, source;
    std::vector<std::string> symbols;
};

DEFINE_CONFIG(MDCaptureConfig, store, source, symbols);

template <typename Config>
concept HasMDCaptureConfig = requires(Config config)
{
    config.mdCapture;
};
