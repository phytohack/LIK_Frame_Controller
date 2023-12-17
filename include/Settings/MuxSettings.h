#pragma once 
#include <Arduino.h>

/////////////////////////////////////////  МУЛЬТИПЛЕКСЕРЫ  /////////////////////////////////////////
// пины i2c
#define SDA_PIN 4
#define SCL_PIN 5

// ПРАВИЛЬНЫЕ ПИНЫ С ФОРУМА https://www.kincony.com/forum/showthread.php?tid=1619
// Картинка платы https://www.kincony.com/esp32-board-16-channel-relay-hardware.html
// Set i2c address for output
// PCF8574 pcf8574_1(0x24);  // channel 1-8     adress:100100
// PCF8574 pcf8574_2(0x25);  //channel 9-16    adress:100101

// // Set i2c address for input
// PCF8574 pcf8574_1(0x22);  // channel 1-8     adress: 100010
// PCF8574 pcf8574_2(0x21);  //channel 9-16    adress: 100001

// i2c адреса
const byte MUX_OUT_ADDRESSES[] = {0x24, 0x25};
const byte MUX_IN_ADDRESSES[] = {0x21, 0x22};

// количество мультиплексеров
const int NUM_MUX_OUT = sizeof(MUX_OUT_ADDRESSES) / sizeof(MUX_OUT_ADDRESSES[0]);
const int NUM_MUX_IN = sizeof(MUX_IN_ADDRESSES) / sizeof(MUX_IN_ADDRESSES[0]);

// задержка между опросами значения сигнала на мультиплексере
uint8_t muxDelayTime = 12;