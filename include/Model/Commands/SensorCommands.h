#pragma once
#include <Arduino.h>
#include "Model/Commands/Command.h"
#include "Devices/Sensor.h"

class SensorCommand : public Command {
public:
    SensorCommand(Sensor * sensor) : sensor(sensor) {};

    Sensor * sensor;
};

class SensorTurnOnCommand : public SensorCommand {
    public:
    SensorTurnOnCommand(Sensor * sensor) : SensorCommand(sensor) {};

    void execute() override {
        sensor->turnOn();
    }
};

class SensorTurnOffCommand : public SensorCommand {
    public:
    SensorTurnOffCommand(Sensor * sensor) : SensorCommand(sensor) {};

    void execute() override {
        sensor->turnOff();
    }
};