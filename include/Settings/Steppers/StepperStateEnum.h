#pragma once
#include <Arduino.h>

// СОСТОЯНИЯ ДВИЖЕНИЯ СТЕППЕРА
enum class StepperState { HOLD,
                          MOVING_FORWARD,
                          MOVING_BACKWARD };
                          
String StepperStateName(StepperState state) {
  switch (state) {
    case StepperState::HOLD: return "HOLD";
    case StepperState::MOVING_FORWARD: return "MOVING_FORWARD";
    case StepperState::MOVING_BACKWARD: return "MOVING_BACKWARD";
    default: return "INVALID";
  }
}

String StepperStateNameJSON(StepperState state) {
  switch(state) {
    case StepperState::HOLD: return "hold";
    case StepperState::MOVING_FORWARD: return "moving_forward";
    case StepperState::MOVING_BACKWARD: return "moving_backward";
    default: return "invalid";
  }
}

