#pragma once

#include <ESPAsyncWebServer.h>

#include "Utilities/Logger/Logger.h"
#include "Utilities/Logger/IFileLogHandler.h"

#include "Utilities/WebInterface/Pages/BasePage.h"  

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
    
    // Большие файлы через String не передаются!! (макс 64 Кб)
    // Поэтому пока пока передается потоком, без красоты (последняя строка метода)
    
    // Чтение содержимого файла
    // String content = fileHandler->readLogFile(fileName);
    // if (content == "") {
    //     request->send(404, "text/plain", "File is empty or not found");
    //     return;
    // }
    // Serial.println("CONTENT::::: Read content length: " + String(content.length()));
    
    // String html = BasePage::getHeader("Лог: " + fileName);
    // html += "<h3>Log file: " + fileName + "</h3>";
    // html += "<pre>" + content + "</pre>";
    // html += "<a href='/logs?handler=" + handlerId + "'><button>Назад</button></a>";
    // html += BasePage::getFooter();
    
    // request->send(200, "text/html", html);
    request->send(LittleFS, "/logs/" + fileName, "text/plain");
}
