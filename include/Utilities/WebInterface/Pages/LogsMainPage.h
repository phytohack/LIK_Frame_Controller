#pragma once 
#include <ESPAsyncWebServer.h>

#include "Utilities/Logger/Logger.h"
#include "Utilities/Logger/ILogHandler.h"
#include "Utilities/Logger/IFileLogHandler.h"

class LogsMainPage {
public:
    static void handle(AsyncWebServerRequest *request);
};

void LogsMainPage::handle(AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Логи - обработчики</title></head><body>";
    html += "<h1>Обработчики файловых логов</h1>";
    html += "<ul>";
    
    // Получаем все обработчики и выводим только IFileLogHandler
    auto handlers = Logger.getHandlers();
    for (auto handler : handlers) {
        if (handler->isFileLogHandler()) {
            IFileLogHandler* fileHandler = static_cast<IFileLogHandler*>(handler);
            html += "<li><a href='/logs?handler=" + fileHandler->getId() + "'>" + fileHandler->getDisplayName() + "</a></li>";
        }
    }
    html += "</ul>";
    html += "<a href='/'><button>Назад</button></a>";
    html += "</body></html>";
    request->send(200, "text/html", html);
}
