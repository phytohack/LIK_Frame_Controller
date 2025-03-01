#pragma once

#include "Utilities/Network/AbsNetworkConnection.h"
#include "Utilities/Network/WiFiConnection.h"
#include "Utilities/Network/EthernetConnection.h"
#include "Utilities/Timer.h"
#include "Utilities/Logger/Logger.h"
#include "Settings/Settings.h"

class NetworkManager_ {
    // ------- SINGLETON --------
 public:
  static NetworkManager_ &getInstance() {
    static NetworkManager_ instance;
    return instance;
  };

 private:
  NetworkManager_(const NetworkManager_ &) = delete;
  NetworkManager_ &operator=(const NetworkManager_ &) = delete;
  // --------- SINGLETON ---------

 public:
  void begin();
  void maintainConnection();
  bool isConnected();
  IPAddress getIP();

 private:
  NetworkManager_() {};
  void _connect(); // ЗДЕСЬ ЛОГИКА ПОДКЛЮЧЕНИЯ - И ПЕРЕЗАГРУЗКИ ЕСЛИ НЕ ПОЛУЧАЕТСЯ ПОДКЛЮЧИТЬСЯ
  AbsNetworkConnection* _connection = nullptr; // Указатель на базовый абстрактный класс соединения
  Timer _connectionTimer = Timer(REBOOT_AFTER_DISCONNECT_TIMEOUT);
};


void NetworkManager_::begin() {
    if (CONNECTION_MODE == ConnectionMode::WIFI) {
        _connection = &WiFiConnection; 
    } else if (CONNECTION_MODE == ConnectionMode::ETH) { 
        _connection = &EthernetConnection; 
    } else {
        Logger.error("NetworkManager: Unknown connection mode!");
        return;
    }
  
    if (!_connection->setup()) {
        Logger.error("NetworkManager: setup() failed!");
    } else {
        Logger.info("NetworkManager: setup() success!");
    }

    _connect();
}

void NetworkManager_::_connect() {
    _connection->connect();
    _connectionTimer.startTimer();
    
    while (!_connection->isConnected()) {
      if (REBOOT_AFTER_CONNECTION_TIMEOUT_FLAG && _connectionTimer.isExpired()) {
        Logger.error("NetworkManager: Connection timeout! Rebooting...");
        ESP.restart();
      }

      delay(1000);
      Serial.print(".");
    }

    Logger.info("Network connected!");
    Logger.info("IP address: " + _connection->getIP().toString());
}

void NetworkManager_::maintainConnection() { 
  if (!_connection->isConnected()) {
    Logger.warn("Network disconnected! Try to reconnect...");
    _connect();
  }
}

bool NetworkManager_::isConnected() {
    if (_connection) {
        return _connection->isConnected();
    }
    return false;
}

IPAddress NetworkManager_::getIP() {
    if (_connection) {
        return _connection->getIP();
    }
    return IPAddress(); // 0.0.0.0
}

// ------- SINGLETON --------
extern NetworkManager_ &NetworkManager;
NetworkManager_ &NetworkManager = NetworkManager.getInstance();
// ------- SINGLETON --------
