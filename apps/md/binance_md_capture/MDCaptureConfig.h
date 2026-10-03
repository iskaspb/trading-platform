#pragma once

#include <config/config.h>

struct MDCaptureConfig
{
    std::string fileStore, address;
    std::vector<std::string> symbols;
};

DEFINE_CONFIG(MDCaptureConfig, fileStore, address, symbols);

template <typename Config>
concept HasMDCaptureConfig = requires(Config config)
{
    config.mdCapture;
};
