#pragma once
#include <Arduino.h>

#include "Settings/MuxSettings.h"

/////////////////////////////////////////  THERMAL STEPPERS  /////////////////////////////////////////

const byte THERMAL_CAMERA_STEPPER_MUX_ADDRESS = MUX_OUT_ADDRESSES[0]; // MUX_OUT_1 {0x24}
#define THERMAL_CAMERA_STEPPER_STEP_PIN 0
#define THERMAL_CAMERA_STEPPER_DIR_PIN 1
#define THERMAL_CAMERA_STEPPER_ENA_PIN 2

const byte THERMAL_CAMERA_STEPPER_BASE_LS_MUX_ADDRESS = MUX_IN_ADDRESSES[0]; // {0x21, 0x22};
#define THERMAL_CAMERA_STEPPER_BASE_LS_PIN  7

const byte THERMAL_CAMERA_STEPPER_END_LS_MUX_ADDRESS = MUX_IN_ADDRESSES[0]; // {0x21, 0x22};
#define THERMAL_CAMERA_STEPPER_END_LS_PIN  15