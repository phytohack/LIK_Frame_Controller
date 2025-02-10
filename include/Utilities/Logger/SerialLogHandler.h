#pragma once

#include <Arduino.h>

#include "Utilities/Logger/ILogHandler.h"
#include "Settings/Logger.h"

class SerialLogHandler : public ILogHandler {
public:
    SerialLogHandler(LogLevel minLevel = LogLevel::DEBUG)
    : ILogHandler(minLevel) {}

private:
    void _logMessageImpl(LogLevel level, const String& message) override {
        Serial.println(message);
    };
};
