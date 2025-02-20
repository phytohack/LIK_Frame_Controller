#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <algorithm> // для std::sort

#include "Utilities/Logger/Logger.h"
#include "Utilities/SPIFFSManager.h"

// Функция для декодирования URL (заменяет %20 на пробел и т.п.)
String urlDecode(const String &src) {
    String decoded = "";
    char temp[] = "0x00";
    unsigned int len = src.length();
    for (unsigned int i = 0; i < len; i++) {
        if (src[i] == '%') {
            if (i + 2 < len) {
                temp[2] = src[i + 1];
                temp[3] = src[i + 2];
                char decodedChar = strtol(temp, NULL, 16);
                decoded += decodedChar;
                i += 2;
            }
        } else if (src[i] == '+') {
            decoded += ' ';
        } else {
            decoded += src[i];
        }
    }
    return decoded;
}

class HttpServerManager_ {
public:
  // ------------- SINGLETON ---------------
  static HttpServerManager_& getInstance() {
      static HttpServerManager_ instance;
      return instance;
  }
private:
  HttpServerManager_() : server(80) {}
  HttpServerManager_(const HttpServerManager_&) = delete;
  HttpServerManager_& operator=(const HttpServerManager_&) = delete;
  // ------------- SINGLETON ---------------

public:
    void begin() {
        // Маршрут для списка логов (при запросе "/logs" или "/logs/")
        server.on("/logs", HTTP_GET, handleLogsList);
        server.on("/logs/", HTTP_GET, handleLogsList);
        
        // Обработчик для всех остальных запросов
        // Если URI начинается с "/logs/", то считаем, что запрошен конкретный лог
        server.onNotFound([](){
            String uri = getInstance().server.uri();
            if (uri.startsWith("/logs/")) {
                handleLogFile();
            } else {
                // Для остальных URI возвращаем стандартное сообщение 404.
                getInstance().server.send(404, "text/plain", "Not found");
            }
        });
        
        server.begin();
        Logger.info("HTTP server started.");
    }

    void handleClient() {
        server.handleClient();
    }

private:
    WebServer server;

    // Обработчик для списка логов
    static void handleLogsList() {
        std::vector<String> files = SPIFFSManager.listFiles("/logs");
        std::sort(files.begin(), files.end());
        
        // Сортируем список файлов по алфавиту
        // std::sort(files.begin(), files.end());
        
        String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Logs</title></head><body>";
        html += "<h1>Log Files</h1><ul>";
        
        for (const auto& file : files) {
            // Формируем ссылку на файл (обратите внимание – URL не декодируется, браузер сам отправляет закодированное значение)
            String filePath = "/logs/" + file;
            html += "<li><a href=\"" + filePath + "\">" + file + "</a></li>";
        }
        
        html += "</ul></body></html>";
        
        getInstance().server.send(200, "text/html", html);
    }

    // Обработчик для отображения содержимого конкретного лога
    static void handleLogFile() {
        Logger.info("Enter handleLogFile");
        String uri = getInstance().server.uri();
        Logger.info("Requested URI: " + uri);
        
        // Ожидаем формат "/logs/filename"
        if (uri.length() <= 6) { // "/logs/" имеет длину 6
            getInstance().server.send(400, "text/plain", "Bad Request");
            return;
        }
        
        // Декодируем URI, чтобы убрать символы %20 и т.п.
        String decodedUri = urlDecode(uri);
        // Получаем имя файла (убираем префикс "/logs/")
        String fileName = decodedUri.substring(6);
        String filePath = "/logs/" + fileName;
        Logger.info("Try to find file: " + filePath);
        
        // Проверяем наличие папки /logs/ (или можно проверить существование конкретного файла)
        if (!SPIFFSManager.exists(filePath.c_str())) {
            Logger.info("Log file not found on path: " + filePath);
            getInstance().server.send(404, "text/plain", "Log file not found on path: " + filePath);
            return;
        }
        
        // Читаем содержимое файла
        String content = SPIFFSManager.readFile(filePath.c_str());
        if (content.length() == 0) {
            Logger.info("No content or file not found.");
            getInstance().server.send(404, "text/plain", "File not found or empty.");
            return;
        }
        
        Logger.info("Making HTML for file: " + fileName);
        String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>" + fileName + "</title></head><body>";
        html += "<h1>" + fileName + "</h1><pre>" + content + "</pre>";
        html += "<a href=\"/logs\">Back to Logs List</a>";
        html += "</body></html>";
        
        getInstance().server.send(200, "text/html", html);
    }
};

// ------------- SINGLETON ---------------
extern HttpServerManager_ &HttpServerManager;
HttpServerManager_ &HttpServerManager = HttpServerManager.getInstance();
// ------------- SINGLETON ---------------
