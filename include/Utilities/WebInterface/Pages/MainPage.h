#pragma once
#include <ESPAsyncWebServer.h>

class MainPage {
public:
    static void handle(AsyncWebServerRequest *request);
};

void MainPage::handle(AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Main ESP32 Control</title></head><body>";
    html += "<h1>Main ESP32 Control</h1>";
    html += "<a href='/logs'><button>Логи</button></a>";
    html += "</body></html>";
    request->send(200, "text/html", html);
}
