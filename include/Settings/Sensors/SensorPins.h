#pragma once
#include <Arduino.h>

#include "Settings/MuxSettings.h"

/////////////////////////////////////////  THERMAL STEPPERS  /////////////////////////////////////////

const byte THERMAL_CAMERA__MUX_ADDRESS = MUX_OUT_ADDRESSES[0]; // MUX_OUT_1 {0x24}
#define THERMAL_CAMERA_TURN_ON_PIN 4