#pragma once

#include <Arduino.h>

#include "Utilities/Logger/ILogHandler.h"
#include "Settings/Logger.h"
#include "Utilities/SPIFFSManager.h"
#include "Utilities/Clock.h"

class SpiffsLogHandler : public ILogHandler {
public:
    SpiffsLogHandler(LogLevel minLevel = LogLevel::ERROR)
    : _minLevel(minLevel) {}

    // void logMessage(LogLevel level, const char* message) override {
    void logMessage(LogLevel level, String message) override {
        // Проверяем, стоит ли выводить этот уровень
        // static_cast явно указывает намерение “преобразовать тип А в тип B”
        if (static_cast<int>(level) <= static_cast<int>(_minLevel)) {
            // 1. Получаем дату 
            String date = Clock.getDate();
            
            // 2. Если за эту дату мы еще не делали запись, то пишем в новый файл с новым id
            if (date != _lastDate) {
                _lastDate = date;
                _lastFileId = SPIFFSManager.countFiles("/logs") + 1;
            }
            // 3. Имя файла
            String fileName = "/logs/" + String(_lastFileId) + " - " + date + ".txt";
            
            // 4. Запись в файл
            SPIFFSManager.writeFile(fileName.c_str(), message.c_str());
        }
    }
private:
    LogLevel _minLevel;
    // bool _firstWrite = true;
    String _lastDate = "";
    uint16_t _lastFileId = 0;
};
