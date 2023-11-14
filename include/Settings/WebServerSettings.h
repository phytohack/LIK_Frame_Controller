#pragma once
#include <Arduino.h>

#ifdef ESP8266
// Code specific to ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef ESP32
// Code specific to ESP32
#include <WiFi.h>
#endif


// разделение на рабочие (там esp32) и домашние настройки (там esp8266)
#ifdef ESP32
const char* ssid = "KlimBioTech-LIK";
const char* password = "U35awfXMT3jnuxQJSMYY";
IPAddress staticIP(192, 168, 1, 58); // Static IP address you want
IPAddress gateway(192, 168, 1, 1);     // Gateway of your network
#endif

#ifdef ESP8266
const char* ssid = "TP-Link_CFCD";
const char* password = "87988918";
IPAddress staticIP(192, 168, 0, 43); // Static IP address you want
IPAddress gateway(192, 168, 0, 1);     // Gateway of your network
#endif

IPAddress subnet(255, 255, 255, 0);    // Subnet mask of your network
IPAddress primaryDNS(8, 8, 8, 8);      // Primary DNS, you can use Google's DNS
IPAddress secondaryDNS(8, 8, 4, 4);    // Secondary DNS, you can use Google's DNS

