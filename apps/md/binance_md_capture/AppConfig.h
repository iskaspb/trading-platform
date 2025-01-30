#pragma once
#include "MDCaptureConfig.h"
#include <app/LogConfig.h>

struct AppConfig
{
    app::LogConfig log;
    MDCaptureConfig mdCapture;
};
DEFINE_CONFIG(AppConfig, log, mdCapture);
