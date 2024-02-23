#pragma once
#ifdef ESP8266
// Code specific to ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef ESP32
// Code specific to ESP32
#include <WiFi.h>
#include <ETH.h>
#endif

#include <WebSocketsServer.h>
// #include "Model/MessageHandler.h"
#include "Utilities/Logger.h"
#include "Utilities/Timer.h"
// #include "ArduinoJson.h"

enum class ConnectionMode { WIFI, ETH };

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
  void setupWiFi(String ssid, String pass, IPAddress staticIP, IPAddress gateway, IPAddress subnet, IPAddress primaryDNS, IPAddress secondaryDNS);
  void setupETH(IPAddress staticIP, IPAddress gateway, IPAddress subnet, IPAddress primaryDNS, IPAddress secondaryDNS);
  void loop();
  
  void setIncomeMessageHandler(IncomeMessageHandler incomeMessageHandler);
  void setMainControllerClientNum(int num);
  void send(uint8_t clientNum, String msg);
  void sendToMainController(String msg);
  
  void printMainControllerConnectionState();
  void disconnectClient();

 private:
  WebSocketServerManager_() : webSocket(81){};
  String _ssid;
  String _pass;

  ConnectionMode _mode = ConnectionMode::WIFI;
  void connectToWiFi();
  void connectToETH();


  static void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
  WebSocketsServer webSocket;
  IncomeMessageHandler incomeMessageHandler;
  int mainControllerClientNum = -1;  // -1 indicates dispatcher is not connected

  Timer disconnectedRebootTimer;
  uint16_t rebootAfterDisconnectInterval = 30000;
  bool rebootIfWiFiDisconnectedTimeout = true;
};

void WebSocketServerManager_::setupETH(IPAddress staticIP, IPAddress gateway,
                                    IPAddress subnet, IPAddress primaryDNS,
                                    IPAddress secondaryDNS) {
  _mode = ConnectionMode::ETH;

  if (!ETH.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Logger.println("ETH STATIC IP Failed to configure");
  }

  disconnectedRebootTimer.setTimer(rebootAfterDisconnectInterval);

  connectToETH();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
void WebSocketServerManager_::setupWiFi(String ssid, String pass,
                                    IPAddress staticIP, IPAddress gateway,
                                    IPAddress subnet, IPAddress primaryDNS,
                                    IPAddress secondaryDNS) {
  _mode = ConnectionMode::WIFI;
  _ssid = ssid;
  _pass = pass;

  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Logger.println("STA Failed to configure");
  }
  
  disconnectedRebootTimer.setTimer(rebootAfterDisconnectInterval);

  connectToWiFi();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void WebSocketServerManager_::disconnectClient() {
  webSocket.disconnect(0);
  Serial.println("Client disconnection ok");
}
void WebSocketServerManager_::printMainControllerConnectionState() {
  if (  getInstance().mainControllerClientNum == -1) {
    Logger.println("! -- MAIN CONTROLLER NOT CONNECTED -- !");
  }
  // Serial.print("WiFi Status: ");
  // Serial.println(WiFi.isConnected());
  Serial.print("Clients connected: ");
  Serial.println(webSocket.connectedClients());
}

void WebSocketServerManager_::setMainControllerClientNum(int num) {
  getInstance().mainControllerClientNum = num;
  Logger.debug("MainController connected!");
}

void WebSocketServerManager_::setIncomeMessageHandler(
    IncomeMessageHandler incomeMessageHandler) {
  this->incomeMessageHandler = incomeMessageHandler;
}

void WebSocketServerManager_::connectToWiFi() {
  disconnectedRebootTimer.startTimer();

  WiFi.begin(_ssid, _pass);
  while (WiFi.status() != WL_CONNECTED) {  
    if (rebootIfWiFiDisconnectedTimeout && disconnectedRebootTimer.isItTime()) {
      Serial.printf("\n\n ------ CAN'T CONNECT FOR 30 SECONDS. RESTART CONTROLLER ------ \n\n");
      ESP.restart();
    }
    
    delay(2000);
    Serial.println("Connecting to WiFi...");
    Serial.printf("SSID: %s \n PASS: %s \n", _ssid, _pass);
  }
  Logger.println("Connected to WiFi");
  Logger.print("IP address: ");
  Logger.println(WiFi.localIP().toString());
}

void WebSocketServerManager_::connectToETH() {
  disconnectedRebootTimer.startTimer();

  ETH.begin();
  while (!ETH.linkUp()) {  
    if (rebootIfWiFiDisconnectedTimeout && disconnectedRebootTimer.isItTime()) {
      Serial.printf("\n\n ------ CAN'T CONNECT FOR 30 SECONDS. RESTART CONTROLLER ------ \n\n");
      ESP.restart();
    }
    Serial.println("Waiting for Ethernet link...");
    delay(2000);
  }
  Logger.println("Connected via Ethernet");
  Logger.print("IP address: ");
  Logger.println(ETH.localIP().toString());
}

void WebSocketServerManager_::loop() { 
  webSocket.loop(); 
  if (_mode == ConnectionMode::WIFI && !WiFi.isConnected()) {
    Serial.println("WIFI DISCONNECTED!! TRY TO RECONNECT!!!");
    connectToWiFi();
  }
  else if (_mode == ConnectionMode::ETH && !ETH.linkUp()) {
    Serial.println("ETHERNET DISCONNECTED!! TRY TO RECONNECT!!!");
    connectToETH();
  }
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
      //Serial.printf("[%u] Received: %s\n", num, payload);
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
