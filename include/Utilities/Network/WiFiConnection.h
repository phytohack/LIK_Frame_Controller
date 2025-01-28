#pragma once

#include <WiFi.h>
#include "Utilities/Network/AbsNetworkConnection.h"

#include "Utilities/Logger.h"
#include "Utilities/Timer.h"
#include "Settings/Settings.h"


class WiFiConnection_ : public AbsNetworkConnection {
    // ------- SINGLETON --------
 public:
  static WiFiConnection_ &getInstance() {
    static WiFiConnection_ instance;
    return instance;
  };

 private:
  WiFiConnection_(const WiFiConnection_ &) = delete;
  WiFiConnection_ &operator=(const WiFiConnection_ &) = delete;
  // --------- SINGLETON ---------

 public:
    bool setup() override;
    void connect() override;
    bool isConnected() override;
    void disconnect() override;
    IPAddress getIP() override;
 
 private:
  WiFiConnection_() {};
};

bool WiFiConnection_::setup() {
  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Logger.println("STA Failed to configure");
    return false;
  }
  return true;
}

void WiFiConnection_::connect() {
  Serial.println("Connecting to WiFi...");
  Serial.printf("SSID: %s \n PASS: %s \n", ssid, password);
  WiFi.begin(ssid, password);
}

void WiFiConnection_::disconnect() {
  WiFi.disconnect();
}

bool WiFiConnection_::isConnected() {
  return WiFi.isConnected();
}

IPAddress WiFiConnection_::getIP() {
  return WiFi.localIP();
}

// ------- SINGLETON --------
extern WiFiConnection_ &WiFiConnection;
WiFiConnection_ &WiFiConnection = WiFiConnection.getInstance();
// ------- SINGLETON --------
