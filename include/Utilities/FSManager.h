#pragma once

#include <Arduino.h>
#include <SPIFFS.h>

#include "Utilities/Logger.h"

class FSManager {
public:
    FSManager();
    ~FSManager();

    bool begin();
    bool writeFile(const char* path, const char* message);
    String readFile(const char* path);
    void listFiles();

private:
    void printDirectory(File dir, int numTabs);
};
FSManager::FSManager() {
    // Constructor
}

FSManager::~FSManager() {
    // Destructor
}

bool FSManager::begin() {
    if (!SPIFFS.begin(true)) { // Параметр 'true' включает форматирование при ошибке
        Serial.println("An error has occurred while mounting SPIFFS");
        return false;
    }
    Serial.println("SPIFFS mounted successfully");
    return true;
}

bool FSManager::writeFile(const char* path, const char* message) {
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
    return true;
}

String FSManager::readFile(const char* path) {
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

void FSManager::listFiles() {
    File root = SPIFFS.open("/");
    printDirectory(root, 0);
    root.close();
}

void FSManager::printDirectory(File dir, int numTabs) {
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