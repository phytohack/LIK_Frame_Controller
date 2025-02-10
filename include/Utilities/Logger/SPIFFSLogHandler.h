#pragma once

#include <Arduino.h>

#include "Settings/Logger.h"
#include "Utilities/SPIFFSManager.h"
#include "Utilities/Logger/ILogHandler.h"
#include "Utilities/Clock.h"

class SpiffsLogHandler : public ILogHandler {
public:
    SpiffsLogHandler(LogLevel minLevel = LogLevel::ERROR)
    : ILogHandler(minLevel) {}


private:
    void _logMessageImpl(LogLevel level, const String& message) override;
    
    uint16_t _getNextLogId();
    String  _getLogFileName();
    String _lastDate = "";
    uint16_t _lastFileId = 0;

};

void SpiffsLogHandler::_logMessageImpl(LogLevel level, const String& message) {
    // 1. Получаем имя файла
    String fileName = _getLogFileName();
    
    // 2. Запись в файл
    SPIFFSManager.appendFile(fileName.c_str(), message.c_str());
}

String SpiffsLogHandler::_getLogFileName() {
    // 1. Получаем дату
    String date = Clock.getDate();
    
    // 2. Если за эту дату мы еще не делали запись, то пишем в новый файл с новым id
    if (date != _lastDate) {
        _lastDate = date;
        _lastFileId = _getNextLogId();
    }
    
    // 3. Формирование имени файла.
    //    Используем sprintf с форматом "%05d", чтобы ID выводился с минимум 5 знаками.
    //    Если _lastFileId меньше 100000, то, например, 21 будет выведен как "00021".
    //    Если _lastFileId равен 100000, то выведется "100000".
    char idStr[10]; // Достаточно места для значений до 10 знаков
    sprintf(idStr, "%05d", _lastFileId);
    return "/logs/" + String(idStr) + " - " + date + ".txt";
}

uint16_t SpiffsLogHandler::_getNextLogId() {
    uint16_t lastId = 0;
    const char* idFilePath = "/logs/last_id.txt";

    // Если файл существует – считываем последний id
    if (SPIFFSManager.exists(idFilePath)) {
        String idFileStr = SPIFFSManager.readFile(idFilePath);
        if (idFileStr != "") {
            lastId = idFileStr.toInt();
        }
    }

    // Инкрементируем id
    lastId++;

    // Сохраняем новое значение id в файл
    if(!SPIFFSManager.writeFile(idFilePath, String(lastId).c_str())) {
        Serial.println("Can't write last id to file!");
    }
    return lastId;
}
