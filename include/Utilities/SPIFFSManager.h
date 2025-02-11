#pragma once

#include <Arduino.h>
#include <SPIFFS.h>
#include <vector>
#include <algorithm> // для std::sort

// #include "Utilities/Logger/Logger.h"

class SPIFFSManager_ {

// ------------- SINGLETON ---------------
public:
    static SPIFFSManager_ &getInstance()
    {
        static SPIFFSManager_ instance;
        return instance;
    }

private:
    SPIFFSManager_(){};
    SPIFFSManager_(const SPIFFSManager_ &) = delete;
    SPIFFSManager_ &operator=(const SPIFFSManager_ &) = delete;
// ------------- SINGLETON ---------------


public:
    bool begin();
    bool exists(const char* path) { return SPIFFS.exists(path);}
    bool appendFile(const char* path, const char* message) { return _writeFile(path, message, true); }; // Добавить в файл
    bool writeFile(const char* path, const char* message) { return _writeFile(path, message, false); }; // Записать в файл (перезаписать)
    String readFile(const char* path);
    
     // убедиться, что достаточно места в SPIFFS для записи файла и удалить старые файлы, если необходимо
    bool ensureFreeSpace(const char* folderPath, size_t requiredSize);

    std::vector<String> getFileList(const char* path);
    
    // int countFiles(const char* path);
    
    bool clearSPIFFS();  // Удаляет все файлы из SPIFFS ** не работает
    bool clearDir(const char* dirPath);


private:
    bool _writeFile(const char* path, const char* message, bool append); // Внутренний метод для записи в файл
    String _getDirectoryFromPath(const char* path); // Вспомогательный метод для извлечения директории из полного пути
    bool clearDirectory(const char* path); // Рекурсивное удаление файлов из указанной директории

};

bool SPIFFSManager_::begin() {
    if (!SPIFFS.begin(true)) { // Параметр 'true' включает форматирование при ошибке
        Serial.println("An error has occurred while mounting SPIFFS");
        return false;
    }
    Serial.println("SPIFFS mounted successfully");
    return true;
}

bool SPIFFSManager_::_writeFile(const char* path, const char* message, bool append) {
    // Определяем требуемый размер для записи (длина сообщения + небольшой запас)
    size_t requiredSize = strlen(message) + 10;
    // Убеждаемся, что в SPIFFS достаточно места для записи файла и удаляем старые файлы, если необходимо
    if (!ensureFreeSpace(_getDirectoryFromPath(path).c_str(), requiredSize)) {
        Serial.println("Failed to ensure free space in SPIFFS");
        return false;
    }

    File file = SPIFFS.open(path, append ? "a" : "w");
    // Открываем файл для записи (добавляем в конец или перезаписываем)
    if (!file) {
        // Если файл не удалось открыть, возвращаем false
        Serial.println("Failed to open file for writing");
        return false;
    }
    
    // Записываем сообщение в файл
    if (!(file.print(message) && file.print("\n"))) {
        // Если запись не удалась, закрываем файл и возвращаем false
        Serial.println("Write failed");
        return false;
    }
    
    file.close();
    return true;
}


String SPIFFSManager_::readFile(const char* path) {
    File file = SPIFFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return String();
    }
    String content;
    while (file.available()) {
        content += char(file.read());
    }
    file.close();
    return content;
}

bool SPIFFSManager_::ensureFreeSpace(const char* folderPath, size_t requiredSize) {
    uint32_t freeSpace = SPIFFS.totalBytes() - SPIFFS.usedBytes();;
    
    // Если свободного места достаточно – выходим
    if (freeSpace >= requiredSize)
        return true;
    
    // Получаем список файлов из указанной папки
    std::vector<String> fileList = getFileList(folderPath);
    if (fileList.empty()) {
        Serial.println("No files to remove in the folder: " + String(folderPath));
        return false;
    }
    
    // Сортируем файлы по имени (при условии, что имя файла начинается с числового ID с ведущими нулями)
    std::sort(fileList.begin(), fileList.end());
    
    // Удаляем файлы по одному до тех пор, пока не освободится место
    for (size_t i = 0; i < fileList.size(); i++) {
        String filePath = fileList[i];
        // Если в списке имя файла не содержит путь – добавляем его
        if (!filePath.startsWith(folderPath))
            filePath = String(folderPath) + "/" + filePath;
        Serial.printf("Removing file: %s\n", filePath.c_str());
        SPIFFS.remove(filePath.c_str());
        freeSpace = SPIFFS.totalBytes() - SPIFFS.usedBytes();
        if (freeSpace >= requiredSize)
            break;
    }

    return true;
}


std::vector<String> SPIFFSManager_::getFileList(const char* path) {
    std::vector<String> fileList;
    File dir = SPIFFS.open(path, "r");
    if (!dir || !dir.isDirectory()) {
        Serial.println("Directory does not exist or is not a directory.");
        return fileList;
    }

    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }
        if (!entry.isDirectory()) {
            fileList.push_back(String(entry.name()));
        }
        entry.close();
    }
    dir.close();

    return fileList;
}

String SPIFFSManager_::_getDirectoryFromPath(const char* path) {
    String fullPath = String(path);
    int lastSlash = fullPath.lastIndexOf('/');
    if (lastSlash == -1) {
        return "/";
    } else if (lastSlash == 0) {
        return "/";
    } else {
        return fullPath.substring(0, lastSlash);
    }
}

bool SPIFFSManager_::clearDirectory(const char* path) {
    bool success = true;
    
    // Открываем директорию по переданному пути (например, "/" или "/logs")
    File dir = SPIFFS.open(path, "r");
    if (!dir || !dir.isDirectory()) {
        Serial.printf("Directory %s not found or is not a directory.\n", path);
        return false;
    }
    
    // Готовим родительский путь – гарантируем, что он заканчивается слэшем, если не корень
    String parentPath = String(path);
    if (parentPath != "/" && !parentPath.endsWith("/")) {
        parentPath += "/";
    }
    
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;  // Больше элементов в данной директории
        }
        
        // Получаем имя элемента, которое может быть относительным (например, "31 - 2025-02-01.txt")
        // или абсолютным (например, "/31 - 2025-02-01.txt")
        String entryName = String(entry.name());
        String fullPath;
        
        // Если мы не в корневой директории, то проверяем,
        // начинается ли имя элемента уже с родительского пути.
        if (parentPath != "/" && !entryName.startsWith(parentPath)) {
            // Если имя начинается с "/" — удаляем его, чтобы потом правильно конкатенировать
            if (entryName.startsWith("/")) {
                entryName = entryName.substring(1);
            }
            fullPath = parentPath + entryName;
        } else {
            // Если мы в корневой директории или имя уже содержит родительский путь,
            // проверяем наличие ведущего слеша для корня
            if (String(path) == "/") {
                if (!entryName.startsWith("/")) {
                    fullPath = "/" + entryName;
                } else {
                    fullPath = entryName;
                }
            } else {
                fullPath = entryName;
            }
        }
        
        Serial.printf("Trying to remove: %s\n", fullPath.c_str());
        
        if (entry.isDirectory()) {
            // Рекурсивно удаляем содержимое поддиректории
            success &= clearDirectory(fullPath.c_str());
            // Обычно папки в SPIFFS являются виртуальными, поэтому удалять саму папку не требуется
        } else {
            // Удаляем файл по сформированному абсолютному пути
            if (!SPIFFS.remove(fullPath.c_str())) {
                Serial.printf("Failed to remove file: %s\n", fullPath.c_str());
                success = false;
            } else {
                Serial.printf("Removed file: %s\n", fullPath.c_str());
            }
        }
        entry.close();
    }
    dir.close();
    return success;
}

bool SPIFFSManager_::clearSPIFFS() {
    Serial.println("Clearing SPIFFS...");
    bool success = clearDirectory("/");
    
    // Сбрасываем кеш, так как содержимое изменилось
    // _isCacheValid = false;
    // _lastCountedPath = "";
    // _lastFileCount = 0;
    
    if (success) {
        Serial.println("SPIFFS cleared successfully.");
    } else {
        Serial.println("Error while clearing SPIFFS.");
    }
    return success;
}


bool SPIFFSManager_::clearDir(const char* dirPath) {
    bool success = true;

    // Открываем папку /logs
    File dir = SPIFFS.open(dirPath, "r");
    if (!dir || !dir.isDirectory()) {
        Serial.printf("Directory %s does not exist or is not a directory.\n", dirPath);
        return false;
    }

    // Перебираем все элементы в директории
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;  // Больше элементов нет
        }
        
        // Получаем имя файла. Предполагается, что SPIFFS возвращает имя файла как "/logs/filename.txt"
        // Но если вдруг имя возвращается без префикса, добавляем его вручную.
        String filePath = String(entry.name());
        if (!filePath.startsWith("/logs/")) {
            filePath = String("/logs/") + filePath;
        }

        // Если элемент является файлом, пробуем его удалить
        if (!entry.isDirectory()) {
            Serial.printf("Removing file: %s\n", filePath.c_str());
            if (!SPIFFS.remove(filePath.c_str())) {
                Serial.printf("Failed to remove file: %s\n", filePath.c_str());
                success = false;
            }
        } else {
            Serial.printf("Skipping directory: %s\n", filePath.c_str());
        }
        entry.close();
    }
    dir.close();
    return success;
}


// ------------- SINGLETON ---------------
extern SPIFFSManager_ &SPIFFSManager;
SPIFFSManager_ &SPIFFSManager = SPIFFSManager.getInstance();
// ------------- SINGLETON ---------------