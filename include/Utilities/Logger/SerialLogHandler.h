#pragma once

#include <Arduino.h>

#include "Utilities/Logger/ILogHandler.h"
#include "Settings/Logger.h"

class SerialLogHandler : public ILogHandler {
public:
    SerialLogHandler(LogLevel minLevel = LogLevel::DEBUG)
    : _minLevel(minLevel) {}

    // void logMessage(LogLevel level, const char* message) override {
    void logMessage(LogLevel level, String message) override {
        // Проверяем, стоит ли выводить этот уровень
        // static_cast явно указывает намерение “преобразовать тип А в тип B”
        if (static_cast<int>(level) <= static_cast<int>(_minLevel)) {
            // Предположим, что Serial уже инициализирован:
            Serial.println(message);
        }
    }
private:
    LogLevel _minLevel;
};
