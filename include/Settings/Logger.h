#pragma once

#include <Arduino.h>

#include "Settings/Settings.h"

/*
Настройки логгера
*/

#define WRITE_LOG_TO_SERIAL true
#define WRITE_LOG_TO_SPIFFS true

enum LogLevel
{
    OFF,    // логи выключены
    FATAL,  // фатальные ошибки (аварийное завершение программы, перезагрузка)
    ERROR,  // ошибки (например, не удалось подключиться к серверу) - программа продолжает работу, но надо обратить внимание
    WARN,   // предупреждения (например, не удалось прочитать файл), нужно проверить
    INFO,   // информационные сообщения, о событиях и состояниях
    DEBUG   // отладочные сообщения, для отслеживания работы программы
};

const char* logLevelToString(LogLevel level) {
    switch(level) {
        case OFF:
            return "[OFF]";
        case FATAL:
            return "[FATAL]";
        case ERROR:
            return "[ERROR]";
        case WARN:
            return "[WARN]";
        case INFO:
            return "[INFO]";
        case DEBUG:
            return "[DEBUG]";
        default:
            return "[UNKNOWN]";
    }
}