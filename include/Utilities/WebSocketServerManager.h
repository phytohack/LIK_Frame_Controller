#pragma once

#include <WebSocketsServer.h>

#include "Utilities/Logger/Logger.h"
#include "Utilities/Timer.h"

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
  void begin();
  void loop();
  
  void setIncomeMessageHandler(IncomeMessageHandler incomeMessageHandler);
  void setMainControllerClientNum(int num);
  void send(uint8_t clientNum, String msg);
  void sendToMainController(String msg);
  
  void printMainControllerConnectionState();
  void disconnectClient();

 private:
  WebSocketServerManager_() : webSocket(81){};

  static void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
  WebSocketsServer webSocket;
  IncomeMessageHandler incomeMessageHandler;
  int mainControllerClientNum = -1;  // -1 indicates dispatcher is not connected
};

void WebSocketServerManager_::begin() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void WebSocketServerManager_::disconnectClient() {
  webSocket.disconnect(0);
  Logger.debug("Client disconnected successfully!");
}

void WebSocketServerManager_::printMainControllerConnectionState() {
  if (  getInstance().mainControllerClientNum == -1) {
    Logger.warn("! -- MAIN CONTROLLER NOT CONNECTED -- !");
  } else {
    Logger.debug("Clients connected: " + String(webSocket.connectedClients()));
  }
}

void WebSocketServerManager_::setMainControllerClientNum(int num) {
  getInstance().mainControllerClientNum = num;
  Logger.debug("MainController connected!");
}

void WebSocketServerManager_::setIncomeMessageHandler(
    IncomeMessageHandler incomeMessageHandler) {
  this->incomeMessageHandler = incomeMessageHandler;
}

void WebSocketServerManager_::loop() { 
  webSocket.loop(); 
}

void WebSocketServerManager_::send(uint8_t num, String msg) {
  Logger.debug("");
  Logger.debug("[MAIN CONTROLLER]    <---    [FRAME CONTROLLER]");
  Logger.debug("----------------------------");
  Logger.debug(msg);
  Logger.debug("----------------------------");
  webSocket.sendTXT(num, msg);
}

void WebSocketServerManager_::sendToMainController(String msg) {
  if (getInstance().mainControllerClientNum != -1) {
    getInstance().send(getInstance().mainControllerClientNum, msg);
  }
}

void WebSocketServerManager_::webSocketEvent(uint8_t num, WStype_t type,
                                             uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      if (num == getInstance().mainControllerClientNum) {
        getInstance().mainControllerClientNum = -1;
        Logger.warn("MainController disconnected!");
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
      if (getInstance().incomeMessageHandler)
        getInstance().incomeMessageHandler(num, String((char *)payload));
      break;
    case WStype_PING: {
    // PING обрабатывается автоматически
      break;
    }
    default:
      Logger.warn("WebSocketEvent:: unknown event type: " + String(type));
  }
}

// ------- SINGLETON --------
extern WebSocketServerManager_ &WebSocketServerManager;
WebSocketServerManager_ &WebSocketServerManager = WebSocketServerManager.getInstance();
// ------- SINGLETON --------
