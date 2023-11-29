#pragma once
#include <PCF8575.h>

#include "Utilities/Logger.h"

class Multiplexer : public PCF8575 {
 private:
  int _mode = -1;  // OUTPUT = 0x01, INPUT = 0x00
 public:
  Multiplexer(byte address, byte sda, byte scl) : PCF8575(address, sda, scl) {
    begin();
  }
  Multiplexer(byte address, byte sda, byte scl, byte mode)
      : PCF8575(address, sda, scl) {
    begin();
    if (mode == INPUT)
      setupAsInput();
    else if (mode == OUTPUT)
      setupAsOutput();
    else
      return;
    _mode = mode;
  }

  void setupAsOutput() {
    if (_mode == OUTPUT) return;
    for (byte i = 0; i < 16; i++) {
      pinMode(i, OUTPUT);
      //  перещелкиваем все, чтобы не залипали в начале работы
      digitalWrite(i, LOW);
      digitalWrite(i, HIGH);
    }
  }

  void setupAsInput() {
    if (_mode == INPUT) return;
    for (byte i = 0; i < 16; i++) {
      pinMode(i, INPUT);
    }
  }

  void setupAsOutput(byte i) {
    pinMode(i, OUTPUT);
    // перещелкиваем все, чтобы не залипали в начале работы
    digitalWrite(i, LOW);
    digitalWrite(i, HIGH);
  }

  void setupAsInput(byte i) { pinMode(i, INPUT); }
};
