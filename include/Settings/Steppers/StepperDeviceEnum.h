#pragma once
#include <Arduino.h>
//#include <map>

// СПИСОК СТЕППЕРОВ

enum class DeviceStepper { THERMAL_CAMERA_STEPPER };

int StepperDeviceID(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return 1;
    default:
      return -1;
  }
}

String StepperDeviceName(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return "THERMAL_CAMERA_STEPPER";
    default:
      return "INVALID";
  }
}

String StepperDeviceNameJSON(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return "thermal_camera_stepper";
    default:
      return "invalid";
  }
}