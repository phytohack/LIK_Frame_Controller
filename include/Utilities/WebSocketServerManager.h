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
// #include "Model/MessageHandler.h"
#include "Utilities/Logger.h"
// #include "ArduinoJson.h"

using IncomeMessageHandler = std::function<void(const int, const String &)>;

class WebSocketServerManager_ {
  // ------- SINGLETON --------
 public:
  static WebSocketServerManager_ &getInstance() {
    static WebSocketServerManager_ instance;
    return instance;
  };

 private:
  WebSocketServerManager_(const WebSocketServerManager_ &) = delete;
  WebSocketServerManager_ &operator=(const WebSocketServerManager_ &) = delete;
  // --------- SINGLETON ---------

 public:
  // void setup(String ssid, String pass);
  void setup(String ssid, String pass, IPAddress staticIP, IPAddress gateway,
             IPAddress subnet, IPAddress primaryDNS, IPAddress secondaryDNS);
  void loop();

  void setIncomeMessageHandler(IncomeMessageHandler incomeMessageHandler);
  void setMainControllerClientNum(int num);
  void send(uint8_t clientNum, String msg);
  void sendToMainController(String msg);

 private:
  WebSocketServerManager_() : webSocket(81){};

  static void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                             size_t length);
  WebSocketsServer webSocket;
  IncomeMessageHandler incomeMessageHandler;
  int mainControllerClientNum = -1;  // -1 indicates dispatcher is not connected
};

// void WebSocketServerManager_::setup(String ssid, String pass)
void WebSocketServerManager_::setup(String ssid, String pass,
                                    IPAddress staticIP, IPAddress gateway,
                                    IPAddress subnet, IPAddress primaryDNS,
                                    IPAddress secondaryDNS) {
  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Logger.println("STA Failed to configure");
  }

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Logger.println("Connecting to WiFi...");
  }
  Logger.println("Connected to WiFi");
  Logger.print("IP address: ");
  Logger.println(WiFi.localIP().toString());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void WebSocketServerManager_::setMainControllerClientNum(int num) {
  getInstance().mainControllerClientNum = num;
  Logger.debug("MainController connected!");
}

void WebSocketServerManager_::setIncomeMessageHandler(
    IncomeMessageHandler incomeMessageHandler) {
  this->incomeMessageHandler = incomeMessageHandler;
}

void WebSocketServerManager_::loop() { webSocket.loop(); }

void WebSocketServerManager_::send(uint8_t num, String msg) {
  Logger.debug("SENDING MESSAGE");
  Logger.debug("-----------------------");
  Logger.debug(msg);
  Logger.debug("-----------------------");
  webSocket.sendTXT(num, msg);
}

void WebSocketServerManager_::sendToMainController(String msg) {
  if (getInstance().mainControllerClientNum != -1)
    getInstance().send(getInstance().mainControllerClientNum, msg);
}

void WebSocketServerManager_::webSocketEvent(uint8_t num, WStype_t type,
                                             uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      if (num == getInstance().mainControllerClientNum) {
        getInstance().mainControllerClientNum = -1;
        Logger.println("MainController disconnected!");
      }
      break;
    case WStype_CONNECTED: {
      IPAddress ip = getInstance().webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0],
                    ip[1], ip[2], ip[3], payload);
      // Спросить кто это подключился
      String msg = "{ \"msg_type\": \"identity_request\" }";
      getInstance().send(num, msg);
      break;
    }
    case WStype_TEXT:
      Serial.printf("[%u] Received: %s\n", num, payload);
      if (getInstance().incomeMessageHandler)
        getInstance().incomeMessageHandler(num, String((char *)payload));
      break;
    default:
      Logger.warn("WebSocketEvent:: unknown event type: " + String(type));
  }
}

// ------- SINGLETON --------
extern WebSocketServerManager_ &WebSocketServerManager;
WebSocketServerManager_ &WebSocketServerManager = WebSocketServerManager.getInstance();
// ------- SINGLETON --------
