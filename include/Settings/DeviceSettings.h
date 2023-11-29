#include <Arduino.h>
/////////////////////////////////////////  МУЛЬТИПЛЕКСЕРЫ  /////////////////////////////////////////
// пины i2c
#define SDA_PIN 4
#define SCL_PIN 16

//const byte PCF8575_R1_ADDRESS = 0x24; 
//const byte PCF8575_R2_ADDRESS = 0x25;
//const byte PCF8575_IN1_ADDRESS = 0x21; // IN1 0x21, IN2 0x22, IN3 0x23, IN4 0x26
//const byte PCF8575_IN2_ADDRESS = 0x22;

// i2c адреса
const byte MUX_OUT_ADDRESSES[] = {0x24, 0x25};
const byte MUX_IN_ADDRESSES[] = {0x21, 0x22};

// количество мультиплексеров
const int NUM_MUX_OUT = sizeof(MUX_OUT_ADDRESSES) / sizeof(MUX_OUT_ADDRESSES[0]);
const int NUM_MUX_IN = sizeof(MUX_IN_ADDRESSES) / sizeof(MUX_IN_ADDRESSES[0]);

// задержка между опросами значения сигнала на мультиплексере
uint8_t muxDelayTime = 12;


/////////////////////////////////////////  FRAME STEPPERS  /////////////////////////////////////////

const byte THERMAL_CAMERA_STEPPER_MUX_ADDRESS = MUX_OUT_ADDRESSES[1]; // MUX_OUT_1 {0x24}
#define THERMAL_CAMERA_STEPPER_STEP_PIN 13
#define THERMAL_CAMERA_STEPPER_DIR_PIN 14
#define THERMAL_CAMERA_STEPPER_ENA_PIN 15

const byte THERMAL_CAMERA_STEPPER_BASE_LS_MUX_ADDRESS = MUX_IN_ADDRESSES[0]; // {0x21, 0x22};
#define THERMAL_CAMERA_STEPPER_BASE_LS_PIN  14
const byte THERMAL_CAMERA_STEPPER_END_LS_MUX_ADDRESS = MUX_IN_ADDRESSES[0]; // {0x21, 0x22};
#define THERMAL_CAMERA_STEPPER_END_LS_PIN  15

int THERMAL_CAMERA_STEPPER_AUTO_SPEED = 3000;
int THERMAL_CAMERA_STEPPER_AUTO_ACCELERATION = 200;
const int THERMAL_CAMERA_STEPPER_STEPS_PER_TURN = 800 * 200;  // скорость - шагов на оборот

int THERMAL_CAMERA_BASE_POSITIONING_SPEED = 1000;
int THERMAL_CAMERA_BASE_POSITIONING_ACCELERATION = 200;

const int THERMAL_CAMERA_PRECISE_BASE_POSITIONING_SPEED = 200;
const int THERMAL_CAMERA_PRECISE_BASE_POSITIONING_DISTANCE = 1000;

