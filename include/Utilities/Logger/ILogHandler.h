#pragma once
#include <Arduino.h>

#include "Settings/Logger.h"

class ILogHandler {
public:
    ILogHandler() {}
    ILogHandler(LogLevel minLevel) : _minLevel(minLevel) {}

    virtual ~ILogHandler() {}

    // Устанавливаем минимум, с какого уровня обрабатывать
    void setMinLevel(LogLevel level) { _minLevel = level; }
    LogLevel getMinLevel() { return _minLevel; }

    void logMessage(LogLevel level, const String& message) {
        // Проверяем, стоит ли выводить этот уровень
        if (static_cast<int>(level) <= static_cast<int>(_minLevel)) {
            // Вызываем специфичный метод
            _logMessageImpl(level, message);
        }
    }

protected:
    LogLevel _minLevel = LogLevel::DEBUG;

    // Чисто виртуальный метод для конкретной реализации в наследниках.
    // Он вызовется только если проверка уровня прошла успешно.
    virtual void _logMessageImpl(LogLevel level, const String& message) = 0;
};
