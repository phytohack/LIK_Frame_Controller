#pragma once

#include <Arduino.h>


#include "Utilities/Logger/ILogHandler.h"
#include "Settings/Logger.h"
#include "Utilities/FSManager.h"
#include "Utilities/Clock"

class SpiffsLogHandler : public ILogHandler {
public:
    SpiffsLogHandler(LogLevel minLevel = LogLevel::ERROR)
    : _minLevel(minLevel) {}

    void logMessage(LogLevel level, const char* message) override {
        // Проверяем, стоит ли выводить этот уровень
        // static_cast явно указывает намерение “преобразовать тип А в тип B”
        if (static_cast<int>(level) >= static_cast<int>(_minLevel)) {
            FSManager.write
        }
    }
private:
    LogLevel _minLevel;
};
