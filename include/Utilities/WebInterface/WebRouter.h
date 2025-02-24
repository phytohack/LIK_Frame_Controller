#pragma once
#include <ESPAsyncWebServer.h>

#include "Utilities/WebInterface/Pages/MainPage.h"
#include "Utilities/WebInterface/Pages/LogsMainPage.h"
#include "Utilities/WebInterface/Pages/LogFilesPage.h"
#include "Utilities/WebInterface/Pages/LogContentPage.h"

class WebServerRouter {
public:
    explicit WebServerRouter(AsyncWebServer* server);
    void setupRoutes();
private:
    AsyncWebServer* _server;
};

WebServerRouter::WebServerRouter(AsyncWebServer* server) : _server(server) {}

void WebServerRouter::setupRoutes() {
    // Главная страница
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        MainPage::handle(request);
    });
    
    // Маршрут для работы с логами
    // Если отсутствуют параметры, выводим список обработчиков (LogsMainPage)
    // Если присутствует параметр "handler", выводим список файлов (LogFilesPage)
    // Если есть ещё параметр "file", выводим содержимое файла (LogContentPage)
    _server->on("/logs", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("handler") && !request->hasParam("file")) {
            LogFilesPage::handle(request);
        } else if (request->hasParam("handler") && request->hasParam("file")) {
            LogContentPage::handle(request);
        } else {
            LogsMainPage::handle(request);
        }
    });
    
    // Обработка POST-запросов для удаления файлов
    _server->on("/logs", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasParam("handler")) {
            String handlerId = request->getParam("handler")->value();
            if (request->hasParam("action", true)) { // true - искать в POST параметрах
                String action = request->getParam("action", true)->value();
                if (action == "delete") {
                    LogFilesPage::handleDelete(request);
                } else if (action == "deleteAll") {
                    LogFilesPage::handleDeleteAll(request);
                }
            }
        }
        // Перенаправляем обратно на страницу со списком файлов
        if (request->hasParam("handler")) {
            String redirectUrl = "/logs?handler=" + request->getParam("handler")->value();
            request->redirect(redirectUrl);
        } else {
            request->redirect("/logs");
        }
    });
    
    // Обработка неизвестных URL
    _server->onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Not found");
    });
}
