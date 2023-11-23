#pragma once
#include <Arduino.h>

class LimitSwitcherNoMux {
 public:
  LimitSwitcherNoMux(){};
  LimitSwitcherNoMux(byte pin) { setPin(pin); }

  void setPin(byte pin) {
    _pin = pin;
    pinMode(_pin, INPUT);
  }

  bool isPushed() {
    if (_pin == -1)
      return false;  // чтобы пользоваться методом если пин концевика не задан
    return digitalRead(_pin);
  };

  bool isPinSet() { return (_pin != -1) ? true : false; }

 private:
  byte _pin = -1;
};