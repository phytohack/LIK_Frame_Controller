#pragma once
#include <ESPAsyncWebServer.h>

#include "Utilities/WebInterface/Pages/BasePage.h"

class MainPage {
public:
    static void handle(AsyncWebServerRequest *request);
};

// void MainPage::handle(AsyncWebServerRequest *request) {
//     String html = BasePage::getHeader("Main ESP32 Control");

//     html += "<h3>Frame ESP32 Control</h3>";
//     html += "<a href='/logs'><button>Logs</button></a>";
//     html += BasePage::getFooter();
    
//     request->send(200, "text/html", html);
// }

void MainPage::handle(AsyncWebServerRequest *request) {
    String html = BasePage::getHeader("Main ESP32 Control");

    // Оборачиваем содержимое в контейнер для центрирования
    html += "<div style='text-align:center; margin-top:50px;'>";
    html += "<h1>ESP32 Control Panel</h1>";
    html += "<br>";
    
    // Крупные кнопки для разделов
    html += "<a href='/logs'><button style='font-size:1.5em; padding:20px 40px; margin:10px;'>Logs</button></a>";
    html += "<button style='font-size:1.5em; padding:20px 40px; margin:10px;' disabled>Settings</button>";
    
    html += "</div>";
    html += BasePage::getFooter();
    
    request->send(200, "text/html", html);
}
