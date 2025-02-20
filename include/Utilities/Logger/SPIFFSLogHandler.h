#pragma once

#include <Arduino.h>

#include "Settings/Logger.h"
#include <vector>
#include "IFileLogHandler.h"
#include "Utilities/SPIFFSManager.h"

class SpiffsLogHandler : public IFileLogHandler {
public:
    SpiffsLogHandler(LogLevel minLevel = LogLevel::ERROR)
    : IFileLogHandler("SPIFFS", "SPIFFS", minLevel) {}

    // Реализация метода получения списка лог-файлов из директории /logs
    std::vector<String> listLogFiles() override {
        return SPIFFSManager.listFiles("/logs");
    }

    // Реализация метода чтения содержимого конкретного лог-файла
    String readLogFile(const String& fileName) override {
        return SPIFFSManager.readFile(fileName.c_str());
    }

    // Реализация методов удаления логов

    // Удаляет конкретный лог-файл по его имени
    bool deleteLogFile(const String& fileName) override {
        return SPIFFSManager.deleteFile(fileName.c_str());
    }

    // Удаляет список лог-файлов; возвращает true, если все файлы удалены успешно
    bool deleteLogFiles(const std::vector<String>& fileNames) override {
        bool allDeleted = true;
        for (const auto& fileName : fileNames) {
            if (!SPIFFSManager.deleteFile(fileName.c_str())) {
                allDeleted = false;
            }
        }
        return allDeleted;
    }

    // Удаляет все лог-файлы в директории /logs
    bool deleteAllLogFiles() override {
        std::vector<String> files = SPIFFSManager.listFiles("/logs");
        return deleteLogFiles(files);
    }


protected:
    // Записывает сообщение в лог-файл с помощью SPIFFSManager
    void _writeToFile(const String& fileName, const String& message) override {
        SPIFFSManager.appendFile(fileName.c_str(), message.c_str());
    }
    
    // Абстрактные методы для работы с файловой системой – реализованы через SPIFFSManager
    bool _fileExists(const char* path) override {
        return SPIFFSManager.exists(path);
    }

    String _readFromFile(const char* path) override {
        return SPIFFSManager.readFile(path);
    }

    bool _writeFile(const char* path, const String& data) override {
        return SPIFFSManager.writeFile(path, data.c_str());
    }

};

