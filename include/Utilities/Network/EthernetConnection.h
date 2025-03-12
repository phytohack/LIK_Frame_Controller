#pragma once

#include <SPI.h>
#include <Ethernet_Generic.h>  // Используем библиотеку khoih-prog/Ethernet_Generic
#include "Utilities/Network/AbsNetworkConnection.h"
#include "Utilities/Logger/Logger.h"
#include "Settings/Settings.h"

// --------- Параметры Ethernet для W5500 на ESP32-S3 ---------
#define W5500_CLK_PIN     42
#define W5500_MOSI_PIN    43
#define W5500_MISO_PIN    44
#define W5500_CS_PIN      15
#define W5500_INT_PIN     2
#define W5500_RST_PIN     1

class EthernetConnection_ : public AbsNetworkConnection {
public:
  // ------- SINGLETON --------
  static EthernetConnection_ &getInstance() {
    static EthernetConnection_ instance;
    return instance;
  }

private:
  EthernetConnection_(const EthernetConnection_ &) = delete;
  EthernetConnection_ &operator=(const EthernetConnection_ &) = delete;
  EthernetConnection_() {}

public:
  bool setup() override;
  void connect() override;
  void disconnect() override;
  bool isConnected() override;
  IPAddress getIP() override;

private:
  static void _handleInterrupt(); // При необходимости можно использовать обработчик прерываний
  bool _ethConnected = false;

  // MAC-адрес должен быть длиной ровно 6 байт
  byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
};

bool EthernetConnection_::setup() {
  // Небольшая задержка перед запуском (если требуется)
  delay(5000);
  Logger.info("Setting up Ethernet (W5500) using Ethernet_Generic...");

  // Инициализируем SPI с указанными пинами для W5500
  SPI.begin(W5500_CLK_PIN, W5500_MISO_PIN, W5500_MOSI_PIN);

  // Сброс W5500 через пин RESET
  pinMode(W5500_RST_PIN, OUTPUT);
  digitalWrite(W5500_RST_PIN, LOW);
  delay(100);
  digitalWrite(W5500_RST_PIN, HIGH);
  delay(100);

  // Настраиваем CS-пин
  pinMode(W5500_CS_PIN, OUTPUT);
  digitalWrite(W5500_CS_PIN, HIGH); // Неактивен

  // Инициализируем библиотеку Ethernet с указанием пина CS
  Ethernet.init(W5500_CS_PIN);

  // Настройка пина прерывания (если требуется)
  pinMode(W5500_INT_PIN, INPUT);
  // Если требуется, можно подключить обработчик прерывания:
  // attachInterrupt(digitalPinToInterrupt(W5500_INT_PIN), _handleInterrupt, FALLING);

  // Начало работы Ethernet с использованием статической IP-конфигурации.
  // Убедитесь, что в Settings.h определены: staticIP, gateway, subnet, primaryDNS
  Logger.info("Starting Ethernet with static IP configuration...");
  Ethernet.begin(mac, staticIP, primaryDNS, gateway, subnet);

  // Ожидание установления физической связи с таймаутом
  unsigned long startTime = millis();
  while (Ethernet.linkStatus() == LinkOFF) {
    if (millis() - startTime > 10000) {  // таймаут 10 секунд
      Logger.warn("Ethernet link not established within timeout");
      break;
    }
    delay(500);
    Logger.debug("Waiting for Ethernet link...");
  }

  if (Ethernet.linkStatus() == LinkON) {
    _ethConnected = true;
    Logger.info("Connected via Ethernet");
    Logger.info("IP address: " + String(Ethernet.localIP()));
  }
  else {
    _ethConnected = false;
  }

  return _ethConnected;
}

void EthernetConnection_::connect() {
  if (!isConnected()) {
    Logger.info("Ethernet connect() called. Reinitializing Ethernet...");
    setup();
  }
  else {
    Logger.info("Ethernet already connected");
  }
}

void EthernetConnection_::disconnect() {
  _ethConnected = false;
  Logger.info("Ethernet disconnected");
}

IPAddress EthernetConnection_::getIP() {
  return Ethernet.localIP();
}

bool EthernetConnection_::isConnected() {
  return _ethConnected;
}

void EthernetConnection_::_handleInterrupt() {
  // Здесь можно реализовать обработку прерываний от W5500, если это необходимо
}

// ------- SINGLETON --------
extern EthernetConnection_ &EthernetConnection;
EthernetConnection_ &EthernetConnection = EthernetConnection.getInstance();
