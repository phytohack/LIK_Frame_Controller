#pragma once
#ifdef ESP8266
// Code specific to ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef ESP32
// Code specific to ESP32
#include <WiFi.h>
#endif

#include <WebSocketsServer.h>
//#include "Model/MessageHandler.h"
#include "Utilities/Logger.h"
//#include "ArduinoJson.h"

using IncomeMessageHandler = std::function<void(const String &)>;

class WebSocketManager_
{
public:
    static WebSocketManager_ &getInstance()
    {
        static WebSocketManager_ instance;
        return instance;
    };

    //void setup(String ssid, String pass);
    void setup(String ssid, String pass, IPAddress staticIP, IPAddress gateway, IPAddress subnet, IPAddress primaryDNS, IPAddress secondaryDNS);
    void loop();

    void setIncomeMessageHandler(IncomeMessageHandler incomeMessageHandler);

    void sendTXT(uint8_t num, const char *message, size_t length);
    void broadcastTXT(const char *message);

private:
    WebSocketManager_() : webSocket(81) {};

    // Deleted copy constructor and assignment operator to prevent copying.
    WebSocketManager_(const WebSocketManager_ &) = delete;
    WebSocketManager_ &operator=(const WebSocketManager_ &) = delete;

    static void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
    WebSocketsServer webSocket;
    IncomeMessageHandler incomeMessageHandler;
};


//void WebSocketManager_::setup(String ssid, String pass)
void WebSocketManager_::setup(String ssid, String pass, IPAddress staticIP, IPAddress gateway, IPAddress subnet, IPAddress primaryDNS, IPAddress secondaryDNS)
{
    if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Logger.println("STA Failed to configure");
    }

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Logger.println("Connecting to WiFi...");
    }
    Logger.println("Connected to WiFi");
    Logger.print("IP address: ");
    Logger.println(WiFi.localIP().toString());

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void WebSocketManager_::setIncomeMessageHandler(IncomeMessageHandler incomeMessageHandler)
{
    this->incomeMessageHandler = incomeMessageHandler;
}

void WebSocketManager_::loop()
{
    webSocket.loop();
}

void WebSocketManager_::sendTXT(uint8_t num, const char *message, size_t length)
{
    webSocket.sendTXT(num, message, length);
}

void WebSocketManager_::broadcastTXT(const char *message) {
    Logger.debug("BROADCAST ::: " + String(message));
    webSocket.broadcastTXT(message);
}

void WebSocketManager_::webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Logger.print(String(num));
        Logger.println(" disconnected!");
        break;
    case WStype_CONNECTED:
    {
        IPAddress ip = getInstance().webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        //if (getInstance().incomeMessageHandler)
        //    getInstance().incomeMessageHandler("{\"msg_type\" : \"new_client_connected\"}");
        break;
    }
    case WStype_TEXT:
        Serial.printf("[%u] Received: %s\n", num, payload);
        if (getInstance().incomeMessageHandler)
            getInstance().incomeMessageHandler(String((char *)payload));
        break;
    default:
        Logger.warn("WebSocketEvent:: unknown event type: " + String(type));
    }
}

extern WebSocketManager_ &WebSocketManager;
WebSocketManager_ &WebSocketManager = WebSocketManager.getInstance();
