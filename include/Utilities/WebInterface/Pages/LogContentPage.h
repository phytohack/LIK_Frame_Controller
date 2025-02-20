#pragma once

#include <ESPAsyncWebServer.h>

#include "Utilities/Logger/Logger.h"
#include "Utilities/Logger/IFileLogHandler.h"

class LogContentPage {
public:
    static void handle(AsyncWebServerRequest *request);
};


void LogContentPage::handle(AsyncWebServerRequest *request) {
    if (!request->hasParam("handler") || !request->hasParam("file")) {
        request->redirect("/logs");
        return;
    }
    String handlerId = request->getParam("handler")->value();
    String fileName = request->getParam("file")->value();
    
    IFileLogHandler* fileHandler = nullptr;
    auto handlers = Logger.getHandlers();
    for (auto handler : handlers) {
        IFileLogHandler* fh = static_cast<IFileLogHandler*>(handler);
        if (fh && fh->getId() == handlerId) {
            fileHandler = fh;
            break;
        }
    }
    if (!fileHandler) {
        request->send(404, "text/plain", "Handler not found");
        return;
    }
    
    // Чтение содержимого файла
    String content = fileHandler->readLogFile(fileName);
    
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Лог: " + fileName + "</title></head><body>";
    html += "<h1>Лог файл: " + fileName + "</h1>";
    html += "<pre>" + content + "</pre>";
    html += "<a href='/logs?handler=" + handlerId + "'><button>Назад</button></a>";
    html += "</body></html>";
    
    request->send(200, "text/html", html);
}
