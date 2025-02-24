#pragma once
#include <ESPAsyncWebServer.h>

#include "Utilities/Logger/Logger.h"
#include "Utilities/Logger/IFileLogHandler.h"

#include "Utilities/WebInterface/Pages/BasePage.h"

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
    
    // Вычисление памяти и процентов
    uint32_t totalSpace = fileHandler->getTotalSpace();
    uint32_t usedSpace = fileHandler->getUsedSpace();
    float usedPercent = (totalSpace > 0) ? (100.0 * usedSpace / totalSpace) : 0.0;
    // Форматируем числа для отображения в килобайтах
    String usedKB = String(usedSpace / 1024);
    String totalKB = String(totalSpace / 1024);

    // Header
    String html = BasePage::getHeader("Логи - " + fileHandler->getDisplayName());    
    html += "<h3> Logs " + fileHandler->getDisplayName() + "</h3>";

    // Информация о памяти
    // Контейнер прогресс-бара
    html += "<div style='width:100%; background:#eee; border:1px solid #ccc; border-radius:5px; height:25px; margin-bottom:15px;'>";
    // Заполненная часть прогресс-бара
    html += "<div style='width:" + String(usedPercent) + "%; background:#337ab7; height:100%; border-radius:5px;'></div>";
    html += "</div>";
    html += "<p style='text-align:right; font-style:italic;'>" + usedKB + " KB использовано из " + totalKB + " KB</p>";

    // Список файлов
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
    html += BasePage::getFooter();
    
    request->send(200, "text/html", html);
}

void LogFilesPage::handleDelete(AsyncWebServerRequest *request) {
    // Удаление выбранных файлов
    // if (!request->hasParam("handler", true)) return;
    if (!request->hasParam("handler")) return;
    // String handlerId = request->getParam("handler", true)->value();
    String handlerId = request->getParam("handler")->value();
    
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
    if (!request->hasParam("handler")) return;
    String handlerId = request->getParam("handler")->value();
    
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
