#pragma once
#include <Arduino.h>

enum class StepperCommandType { GO_TO_X, BASING, PRECISE_BASING, STOP };

String StepperCommandTypeJSON(StepperCommandType command) {
  switch (command) {
    case StepperCommandType::GO_TO_X:
      return "go_to_x";
    case StepperCommandType::BASING:
      return "basing";
    case StepperCommandType::PRECISE_BASING:
      return "precise_basing";
    case StepperCommandType::STOP:
      return "stop";
    default:
      return "invalid";
  }
}