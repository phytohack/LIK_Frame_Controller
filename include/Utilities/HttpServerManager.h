#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "Utilities/SPIFFSManager.h"
#include "Utilities/Logger/Logger.h"

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
        // Маршрут для списка логов
        server.on("/logs", HTTP_GET, handleLogsList);
        
        // Маршрут для содержимого конкретного лога
        server.on("/logs/", HTTP_GET, handleLogsList); // Для папки /logs/
        server.on("/logs/*", HTTP_GET, handleLogFile);
        
        server.begin();
        Logger.info("HTTP server started.");
    }

    void handleClient() {
        server.handleClient();
    }

private:
    WebServer server;

    // Обработчик для списка логов
    void handleLogsList() {
        std::vector<String> files = SPIFFSManager.getFileList("/logs");
        
        String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Logs</title></head><body>";
        html += "<h1>Log Files</h1><ul>";
        
        for (const auto& file : files) {
            String filePath = "/logs/" + file;
            html += "<li><a href=\"" + filePath + "\">" + file + "</a></li>";
        }
        
        html += "</ul></body></html>";
        
        server.send(200, "text/html", html);
    }

    // Обработчик для содержимого конкретного лога
    void handleLogFile() {
        String path = server.uri();
        // Ожидаем формат /logs/filename.txt
        if (path.length() <= 6) { // "/logs/" имеет длину 6
            server.send(400, "text/plain", "Bad Request");
            return;
        }
        
        String fileName = path.substring(6); // Получаем "filename.txt"
        String filePath = "/log/" + fileName; // Предполагаем, что логи в папке /log/
        
        if (!SPIFFSManager.countFiles("/log")) { // Проверка наличия папки
            server.send(404, "text/plain", "Log directory not found.");
            return;
        }
        
        String content = SPIFFSManager.readFile(filePath.c_str());
        if (content.length() == 0) {
            server.send(404, "text/plain", "File not found or empty.");
            return;
        }
        
        String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>" + fileName + "</title></head><body>";
        html += "<h1>" + fileName + "</h1><pre>" + content + "</pre>";
        html += "<a href=\"/logs\">Back to Logs List</a>";
        html += "</body></html>";
        
        server.send(200, "text/html", html);
    }
};


// ------------- SINGLETON ---------------
extern HttpServerManager_ &HttpServerManager;
HttpServerManager_ &HttpServerManager = HttpServerManager.getInstance();
// ------------- SINGLETON ---------------