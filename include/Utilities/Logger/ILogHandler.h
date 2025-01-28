#pragma once
#include <Arduino.h>

#include "Settings/Logger.h"

class ILogHandler {
public:
    virtual ~ILogHandler() {}

    // Устанавливаем минимум, с какого уровня обрабатывать
    virtual void setMinLevel(LogLevel level) { _minLevel = level; }
    virtual LogLevel getMinLevel() { return _minLevel; }

    // Собственно метод для приёма сообщений
    virtual void logMessage(LogLevel level, const char* message) = 0;
protected:
    LogLevel _minLevel = LogLevel::DEBUG;
};
