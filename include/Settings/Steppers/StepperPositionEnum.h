#pragma once
#include <Arduino.h>

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
    case StepperPosition::BASE: return "base";
    case StepperPosition::END: return "end";
    case StepperPosition::NONBASE: return "non_base";
    case StepperPosition::UNKNOWN: return "unwknown";
    default: return "invalid";
  }
}

// Коды ответа функции goToXo stepperController-а
enum class StepperAnswer { OK,
                           STOPPED_BY_STOPPER,
                           STOPPED_BY_LIMIT_SWITCHER };
