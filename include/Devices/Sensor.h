#pragma once
#include <Arduino.h>
#include "Settings/Settings.h"
#include "Devices/LowLevel/Multiplexer.h"


class Sensor {
    using StateChangeCallback = std::function<void(Sensor*)>;

    public:
    Sensor(DeviceSensor sensor, Multiplexer* mux, byte pin)
    :
    _sensor(sensor),
    _mux(mux),
    _pin(pin)
    {
        mux->setupAsOutput();
        turnOff();
    }

    public:
    void turnOn() {
        if (_state == SensorState::OFF) {
            _mux->digitalWrite(_pin, LOW);
            setState(SensorState::ON);
        }
    }
    void turnOff() {
        if (_state == SensorState::OFF) {
            _mux->digitalWrite(_pin, LOW);
            setState(SensorState::OFF);
        }
    }

    SensorState getState() { return _state; }
    DeviceSensor getDeviceSensor() { return _sensor; } 

    private:
    void setState(SensorState state) {
        if (_state != state) {
            _state = state;
            if (_stateChangeCallback) {
                _stateChangeCallback(this);
            }
        }
    }


    private:
    SensorState _state = SensorState::OFF;
    DeviceSensor _sensor;
    Multiplexer * _mux;
    byte _pin;
    
    StateChangeCallback _stateChangeCallback;

    public:
    void setStateChangeCallback(StateChangeCallback callback) {
        _stateChangeCallback = callback;
    }
}