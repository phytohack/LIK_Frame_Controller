#pragma once
#include <Arduino.h>

const int basePositioningSpeed = 1000;
const int preciseBasePositioningSpeed = 200;
const int basePositioningAcceleration = 200;
const int preciseBasePositioningDistance = 200;

// ПОЛОЖЕНИЕ СТЕППЕРА
enum class StepperPosition { BASE,
                             END,
                             NONBASE,
                             UNKNOWN };
String StepperPositionName(StepperPosition position) {
  switch (position) {
    case StepperPosition::BASE: return "BASE";
    case StepperPosition::END: return "END";
    case StepperPosition::NONBASE: return "NONBASE";
    case StepperPosition::UNKNOWN: return "UNKNOWN";
    default: return "INVALID";
  }
}
String StepperPositionNameJSON(StepperPosition position) {
  switch (position) {
    case StepperPosition::BASE: return "Base";
    case StepperPosition::END: return "End";
    case StepperPosition::NONBASE: return "Non base";
    case StepperPosition::UNKNOWN: return "Unwknown";
    default: return "Invalid";
  }
}


// Коды ответа функции goToXo stepperController-а
enum class StepperAnswer { OK,
                           STOPPED_BY_STOPPER,
                           STOPPED_BY_LIMIT_SWITCHER };
