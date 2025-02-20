#pragma once
#include <ESPAsyncWebServer.h>

#include "Utilities/Logger/Logger.h"
#include "Utilities/Logger/IFileLogHandler.h"

class LogFilesPage {
public:
    static void handle(AsyncWebServerRequest *request);
    static void handleDelete(AsyncWebServerRequest *request);
    static void handleDeleteAll(AsyncWebServerRequest *request);
};


void LogFilesPage::handle(AsyncWebServerRequest *request) {
    if (!request->hasParam("handler")) {
        request->redirect("/logs");
        return;
    }
    String handlerId = request->getParam("handler")->value();
    
    // Поиск обработчика по id
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
    
    // Получаем список файлов
    std::vector<String> files = fileHandler->listLogFiles();
    
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Логи - " + fileHandler->getDisplayName() + "</title></head><body>";
    html += "<h1>" + fileHandler->getDisplayName() + "</h1>";
    html += "<form method='POST' action='/logs?handler=" + handlerId + "'>";
    
    html += "<ul>";
    for (const auto &fileName : files) {
        // Каждый файл – это ссылка для просмотра и чекбокс для удаления
        html += "<li>";
        html += "<input type='checkbox' name='file' value='" + fileName + "'> ";
        html += "<a href='/logs?handler=" + handlerId + "&file=" + fileName + "'>" + fileName + "</a>";
        html += "</li>";
    }
    html += "</ul>";
    
    html += "<button type='submit' name='action' value='delete'>Удалить выбранные</button> ";
    html += "<button type='submit' name='action' value='deleteAll'>Удалить все</button>";
    html += "</form>";
    
    html += "<a href='/logs'><button>Назад</button></a>";
    html += "</body></html>";
    
    request->send(200, "text/html", html);
}

void LogFilesPage::handleDelete(AsyncWebServerRequest *request) {
    // Удаление выбранных файлов
    if (!request->hasParam("handler", true)) return;
    String handlerId = request->getParam("handler", true)->value();
    
    IFileLogHandler* fileHandler = nullptr;
    auto handlers = Logger.getHandlers();
    for (auto handler : handlers) {
        IFileLogHandler* fh = static_cast<IFileLogHandler*>(handler);
        if (fh && fh->getId() == handlerId) {
            fileHandler = fh;
            break;
        }
    }
    if (!fileHandler) return;
    
    // Собираем выбранные имена файлов
    std::vector<String> filesToDelete;
    int count = request->params();
    for (int i = 0; i < count; i++) {
        const AsyncWebParameter* p = request->getParam(i);
        if (p->isPost() && p->name() == "file") {
            filesToDelete.push_back(p->value());
        }
    }
    // Вызываем метод удаления у обработчика
    fileHandler->deleteLogFiles(filesToDelete);
}

void LogFilesPage::handleDeleteAll(AsyncWebServerRequest *request) {
    // Удаление всех файлов
    if (!request->hasParam("handler", true)) return;
    String handlerId = request->getParam("handler", true)->value();
    
    IFileLogHandler* fileHandler = nullptr;
    auto handlers = Logger.getHandlers();
    for (auto handler : handlers) {
        IFileLogHandler* fh = static_cast<IFileLogHandler*>(handler);
        if (fh && fh->getId() == handlerId) {
            fileHandler = fh;
            break;
        }
    }
    if (!fileHandler) return;
    
    fileHandler->deleteAllLogFiles();
}
