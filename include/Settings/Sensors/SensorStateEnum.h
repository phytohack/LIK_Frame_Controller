#pragma once
#include <Arduino.h>

// СОСТОЯНИЯ ДВИЖЕНИЯ СТЕППЕРА
enum class SensorState 
{
    ON,
    OFF
};
                          
String SensorStateName(SensorState state) {
  switch (state) {
    case SensorState::ON: return "ON";
    case SensorState::OFF: return "OFF";
    default: return "INVALID";
  }
}

String SensorStateNameJSON(SensorState state) {
  switch(state) {
    case SensorState::ON: return "on";
    case SensorState::OFF: return "off";
    default: return "invalid";
  }
}

