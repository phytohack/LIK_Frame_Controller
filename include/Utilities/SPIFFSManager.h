#pragma once

#include <Arduino.h>
#include <SPIFFS.h>

#include "Utilities/Logger/Logger.h"

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
    bool writeFile(const char* path, const char* message);
    String readFile(const char* path);
    void listFiles();
    std::vector<String> getFileList(const char* path); 
    int countFiles(const char* path); // Новый метод


private:
    void printDirectory(File dir, int numTabs);
    String getDirectory(const char* path); // Вспомогательный метод для извлечения директории

    // Кеширование
    String _lastCountedPath = "";
    int _lastFileCount = 0;
    bool _isCacheValid = false;
};

bool SPIFFSManager_::begin() {
    if (!SPIFFS.begin(true)) { // Параметр 'true' включает форматирование при ошибке
        Serial.println("An error has occurred while mounting SPIFFS");
        return false;
    }
    Serial.println("SPIFFS mounted successfully");
    return true;
}

bool SPIFFSManager_::writeFile(const char* path, const char* message) {
    bool fileExists = SPIFFS.exists(path);

    
    File file = SPIFFS.open(path, "a");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    if (file.print(message)) {
        Serial.println("File written successfully");
    } else {
        Serial.println("Write failed");
        return false;
    }
    file.close();

    // Если файл не существовал до записи, сбрасываем кеш
    if (!fileExists) {
        String dir = getDirectory(path);
        if (dir.equals(_lastCountedPath)) {
            _isCacheValid = false;
        }
    }

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

void SPIFFSManager_::listFiles() {
    File root = SPIFFS.open("/");
    printDirectory(root, 0);
    root.close();
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

void SPIFFSManager_::printDirectory(File dir, int numTabs) {
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }
        for (int i = 0; i < numTabs; i++) {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory()) {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        } else {
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}

String SPIFFSManager_::getDirectory(const char* path) {
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

int SPIFFSManager_::countFiles(const char* path) {
    String dirPath = String(path);
    if (_isCacheValid && dirPath.equals(_lastCountedPath)) {
        Serial.printf("Returning cached count for '%s': %d\n", path, _lastFileCount);
        return _lastFileCount;
    }

    File dir = SPIFFS.open(path, "r");
    if (!dir || !dir.isDirectory()) {
        Serial.println("Directory does not exist or is not a directory.");
        return 0;
    }

    int fileCount = 0;
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }
        if (!entry.isDirectory()) {
            fileCount++;
        }
        entry.close();
    }
    dir.close();

    // Обновляем кеш
    _lastCountedPath = dirPath;
    _lastFileCount = fileCount;
    _isCacheValid = true;

    Serial.printf("Number of files in '%s': %d\n", path, fileCount);
    return fileCount;
}


// ------------- SINGLETON ---------------
extern SPIFFSManager_ &SPIFFSManager;
SPIFFSManager_ &SPIFFSManager = SPIFFSManager.getInstance();
// ------------- SINGLETON ---------------