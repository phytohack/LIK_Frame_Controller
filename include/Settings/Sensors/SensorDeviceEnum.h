#pragma once
#include <Arduino.h>

enum class DeviceSensor { THERMAL_CAMERA };

int SensorDeviceID(DeviceSensor sensor) {
  switch (sensor) {
    case DeviceSensor::THERMAL_CAMERA:
      return 1;
    default:
      return -1;
  }
}

String SensorDeviceName(DeviceSensor sensor) {
  switch (sensor) {
    case DeviceSensor::THERMAL_CAMERA:
      return "THERMAL_CAMERA";
    default:
      return "INVALID";
  }
}

String SensorDeviceNameJSON(DeviceSensor sensor) {
  switch (sensor) {
    case DeviceSensor::THERMAL_CAMERA:
      return "thermal_camera";
    default:
      return "invalid";
  }
}