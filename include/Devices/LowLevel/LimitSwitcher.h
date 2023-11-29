#pragma once
#include <PCF8575.h>
#include "Devices/LowLevel/Multiplexer.h"
#include "Utilities/Logger.h"
#include "Settings/Settings.h"

class LimitSwitcher {
public:
    LimitSwitcher(Multiplexer* expander, int pin) : expander(expander), pin(pin) {expander->setupAsInput();};

    bool isPushed() {
        // проверка, что последний раз смотрели значение не раньше чем сколько-то (12) мс назад.
        // иначе выдает рандом
        // скорость проверки 7 мкс (0.007 мс)
        while (millis() - _lastTimeCheck < muxDelayTime) {}
        _lastTimeCheck = millis();
        bool res = !expander->digitalRead(pin);
        return res;
    };

protected:
    Multiplexer* expander;
    int pin;

    unsigned long _lastTimeCheck = 0;

};