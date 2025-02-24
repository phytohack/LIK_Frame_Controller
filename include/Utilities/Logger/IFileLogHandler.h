#pragma once
#include <Arduino.h>
#include <vector>
#include "Settings/Logger.h"
#include "Utilities/Clock.h"

// Абстрактный класс для файловых логов (SPIFFS, SD и т.д.), объединяющий общую функциональность для чтения, записи и удаления логов
class IFileLogHandler : public ILogHandler {
public:
    IFileLogHandler(LogLevel minLevel = LogLevel::ERROR)
        : ILogHandler(minLevel) {}
    
    IFileLogHandler(const String& id, const String& displayName, LogLevel minLevel = LogLevel::ERROR)
        : ILogHandler(minLevel), _id(id), _displayName(displayName) {}

    virtual ~IFileLogHandler() {}

    // Переопределение для указания, что это файловый лог-обработчик. Тогда можно получать списки файлов и их содержимое
    bool isFileLogHandler() const override { return true; }
   

    // Методы для получения информации о логе - для отображения в web-интерфейсе
    String getId() { return _id; }; // Возвращает уникальный идентификатор лога (например, "SPIFFS" или "SD")
    String getDisplayName() { return _displayName; }; // Возвращает отображаемое имя лога (например, "Файловая система SPIFFS" или "SD-карта")

    // Методы для чтения логов
    // Возвращает список файлов логов (например, файлы в директории /logs)
    virtual std::vector<String> listLogFiles() = 0;
    // Читает содержимое конкретного файла логов по его имени
    virtual String readLogFile(const String& fileName) = 0;

    // Методы для удаления логов, чтобы из web-интерфейса можно было управлять файлами
    // Удаляет конкретный лог-файл по его имени
    virtual bool deleteLogFile(const String& fileName) = 0;
    // Удаляет список лог-файлов; возвращает true, если все файлы удалены успешно
    virtual bool deleteLogFiles(const std::vector<String>& fileNames) = 0;
    // Удаляет все лог-файлы (например, очищает директорию /logs)
    virtual bool deleteAllLogFiles() = 0;

    virtual uint32_t getTotalSpace() const = 0;
    virtual uint32_t getFreeSpace() const = 0;
    virtual uint32_t getUsedSpace() const = 0;


    

protected:
    // Шаблонный метод логирования: генерирует имя файла и вызывает запись сообщения
    void _logMessageImpl(LogLevel level, const String& message) override {
        String fileName = _generateLogFileName();
        _writeToFile(fileName, message);
    }

    // Генерация имени файла с учетом текущей даты и уникального идентификатора
    String _generateLogFileName();

    // Получение следующего ID лога. Реализовано с использованием абстрактных операций работы с файлом
    virtual uint16_t _getNextLogId();

    // Чисто виртуальный метод записи данных в файл.
    // Конкретный класс (SPIFFS, SD) должен реализовать его с учётом специфики своей файловой системы.
    virtual void _writeToFile(const String& fileName, const String& message) = 0;

    // Абстрактные методы для работы с файловой системой.
    // Они позволяют получать информацию о файле, читать его содержимое, записывать данные и т.д.
    virtual bool _fileExists(const char* path) = 0;
    virtual String _readFromFile(const char* path) = 0;
    virtual bool _writeFile(const char* path, const String& data) = 0;
    
    String _lastDate = "";
    uint16_t _lastFileId = 0;

    String _id = "";
    String _displayName = "";
};

String IFileLogHandler::_generateLogFileName() {
    String currentDate = Clock.getDate();
    if (currentDate != _lastDate) {
        _lastDate = currentDate;
        _lastFileId = _getNextLogId();
    }
    char idStr[10]; 
    sprintf(idStr, "%05d", _lastFileId);
    return "/logs/" + String(idStr) + " - " + currentDate + ".txt";
}

uint16_t IFileLogHandler::_getNextLogId() {
    uint16_t lastId = 0;
    const char* idFilePath = "/logs/last_id.txt";
    if (_fileExists(idFilePath)) {
        String idStr = _readFromFile(idFilePath);
        if (idStr != "") {
            lastId = idStr.toInt();
        }
    }
    lastId++;
    if (!_writeFile(idFilePath, String(lastId))) {
        Serial.println("Не удалось записать новый ID в файл!");
    }
    return lastId;
}
