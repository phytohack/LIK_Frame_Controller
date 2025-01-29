#pragma once

#include <WiFi.h> // Для WiFiEvent_t и WiFi.onEvent(...) - да, в ESP32 Ethernet завязано на WiFiGeneric
#include <ETH.h>
#include <SPI.h>

#include "Utilities/Network/AbsNetworkConnection.h"
#include "Utilities/Logger/Logger.h"
#include "Settings/Settings.h"

// --------- Параметры Ethernet (https://www.kincony.com/how-to-programming.html)
#define ETH_TYPE       ETH_PHY_LAN8720
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_MDC_PIN 23
#define ETH_MDIO_PIN 18
#define ETH_POWER_PIN -1
#define ETH_ADDR 0
#define NRST 5

class EthernetConnection_ : public AbsNetworkConnection {
public:
    // ------- SINGLETON --------
    static EthernetConnection_ &getInstance() {
        static EthernetConnection_ instance;
        return instance;
    };

private:
    EthernetConnection_(const EthernetConnection_ &) = delete;
    EthernetConnection_ &operator=(const EthernetConnection_ &) = delete;

    // --------- SINGLETON ---------
    EthernetConnection_() {};

 public:
    bool setup() override;
    void connect() override;
    void disconnect() override;
    bool isConnected() override;
    IPAddress getIP() override;

private:
    static void _ethEventHandler(WiFiEvent_t event);
    bool _ethConnected = false;

};

bool EthernetConnection_::setup() {
  // 1. Настраиваем callback на события Ethernet (в ESP32 оно "WiFiEvent" по имени, но относится и к ETH)
  WiFi.onEvent(_ethEventHandler); 

  // 2. Сброс чипа (NRST) (https://www.kincony.com/how-to-programming.html)
  pinMode(NRST, OUTPUT);
  digitalWrite(NRST, 0); delay(200);
  digitalWrite(NRST, 1); delay(200);
  digitalWrite(NRST, 0); delay(200);
  digitalWrite(NRST, 1);
  
  // 3. Запускаем ETH
  // bool ethBegin = ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
//   delay(1000);
//   Serial.print("ETHERNET BEGIN RESULT : ");
//   Serial.println(ethBegin);

  if (!ETH.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Logger.warn("ETH Static IP Failed to configure");
  } else {
    Logger.info("ETH Static IP Configured Successfully");
  }
 return true;
}

void EthernetConnection_::connect() {
  Logger.info("Connecting with Ethernet...");
  ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
}

void EthernetConnection_::disconnect() {
    // ETH.end(); // старая версия? нет такой функции
    _ethConnected = false;
}

IPAddress EthernetConnection_::getIP() {
    return ETH.localIP();
}

bool EthernetConnection_::isConnected() {
    _ethConnected = ETH.linkUp();
    return _ethConnected;
}

// ------------------ Статический callback на события Ethernet ------------------

void EthernetConnection_::_ethEventHandler(WiFiEvent_t event) {
    String log_msg = "";
    switch (event) {
        case SYSTEM_EVENT_ETH_START:
            Logger.info("ETH Started");
            ETH.setHostname("esp32-ethernet");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Logger.info("ETH Connected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            log_msg = "ETH MAC: " + String(ETH.macAddress()) + ", IPv4: " + String(ETH.localIP());
            if (ETH.fullDuplex()) {
                log_msg += ", FULL_DUPLEX";
            }
            log_msg += ", " + String(ETH.linkSpeed()) + "Mbps";
            Logger.info(log_msg);
            // Обновим флаг в синглтоне
            EthernetConnection_::getInstance()._ethConnected = true;
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Logger.warn("ETH Disconnected");
            EthernetConnection_::getInstance()._ethConnected = false;
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Logger.warn("ETH Stopped");
            EthernetConnection_::getInstance()._ethConnected = false;
            break;
        default:
            break;
    }
}

// ------- SINGLETON --------
extern EthernetConnection_ &EthernetConnection; 
EthernetConnection_ &EthernetConnection = EthernetConnection.getInstance();
// ------- SINGLETON --------
