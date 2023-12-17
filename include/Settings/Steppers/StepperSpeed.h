#pragma once
#include <Arduino.h>
#include "Settings/Steppers/StepperDeviceEnum.h"

int StepperStepsPerTurn(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return 800*200;
    default:
      return 800*200;
  }
}

int StepperSpeed(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return 1000;
    default:
      return 1000;
  }
}

int StepperAcceleration(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return 200;
    default:
      return 200;
  }
}

int StepperBasingSpeed(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return 300;
    default:
      return 300;
  }
}

int StepperBasingAcceleration(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return 100;
    default:
      return 100;
  }
}

int StepperPreciseBasingSpeed(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return 50;
    default:
      return 50;
  }
}

int StepperPreciseBasingAcceleration(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return 50;
    default:
      return 50;
  }
}

int StepperPreciseBasingDistance(DeviceStepper stepper) {
  switch (stepper) {
    case DeviceStepper::THERMAL_CAMERA_STEPPER:
      return 100;
    default:
      return 100;
  }
}