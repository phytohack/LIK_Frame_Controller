#pragma once
#include <Arduino.h>
#include <WiFi.h>

#include "Settings/Settings.h"

// Тип соединения
enum class ConnectionMode { WIFI, ETH };
// const ConnectionMode CONNECTION_MODE = ConnectionMode::ETH;
const ConnectionMode CONNECTION_MODE = ConnectionMode::WIFI;

// Настройки перезагрузки в случае отсутствия соединения
const bool REBOOT_AFTER_CONNECTION_TIMEOUT_FLAG = true;
const uint32_t REBOOT_AFTER_DISCONNECT_TIMEOUT = 30000;

// разделение на рабочие (там esp32) и домашние настройки (там esp8266)
#ifdef WORK_WIFI
const char* ssid = "KlimBioTech-LIK";
const char* password = "U35awfXMT3jnuxQJSMYY";
IPAddress staticIP(192, 168, 1, 58); // Static IP address you want
IPAddress gateway(192, 168, 1, 1);     // Gateway of your network
#endif

#ifdef HOME_WIFI
const char* ssid = "TP-Link_CFCD";
const char* password = "87988918";
IPAddress staticIP(192, 168, 0, 43); // Static IP address you want
IPAddress gateway(192, 168, 0, 1);     // Gateway of your network
#endif

IPAddress subnet(255, 255, 255, 0);    // Subnet mask of your network
IPAddress primaryDNS(8, 8, 8, 8);      // Primary DNS, you can use Google's DNS
IPAddress secondaryDNS(8, 8, 4, 4);    // Secondary DNS, you can use Google's DNS

