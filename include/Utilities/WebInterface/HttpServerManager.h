#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "Utilities/WebInterface/WebRouter.h"

// Класс-оболочка для создания и настройки веб-сервера с использованием WebServerRouter.
class HttpServerManager_ {
public:
// ------------- SINGLETON ---------------
  static HttpServerManager_& getInstance() {
      static HttpServerManager_ instance;
      return instance;
  }
private:
  HttpServerManager_()
      : _server(new AsyncWebServer(80)),
        _router(new WebServerRouter(_server))
    {}
  HttpServerManager_(const HttpServerManager_&) = delete;
  HttpServerManager_& operator=(const HttpServerManager_&) = delete;
  // ------------- SINGLETON ---------------

public:
    // Инициализация маршрутов и запуск веб-сервера
    void begin() {
        _router->setupRoutes();
        _server->begin();
        Logger.info("HTTP server started.");
    }
    
private:
    
    AsyncWebServer* _server;
    WebServerRouter* _router;
};

// ------------- SINGLETON ---------------
extern HttpServerManager_ &HttpServerManager;
HttpServerManager_ &HttpServerManager = HttpServerManager.getInstance();
// ------------- SINGLETON ---------------
