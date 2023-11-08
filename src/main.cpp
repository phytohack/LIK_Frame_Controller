#include <Arduino.h>
#include "Devices/SensorStepperController.h"

SensorStepperController *sensorStepper;

const byte stepPin = 23;
const byte dirPin = 21;
const byte enaPin = 18;
const int stepsPerTurn = 800 * 200;  // скорость - шагов на оборот
const byte lsPin = 4;
void check_monitor();

void setup() {
  Serial.begin(115200);
  Serial.println("_SETUP_");

  //sensorStepper = new SensorStepperController(stepsPerTurn, stepPin, dirPin, enaPin, lsPin);
  sensorStepper = new SensorStepperController(stepsPerTurn, stepPin, dirPin, enaPin);
}

long timer = 0;
void loop() {
  
  if(millis() - timer > 1000) {
    timer = millis();
    Serial.println('.');
  }

  check_monitor();
}




void check_monitor() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    Serial.print(F("Input from Serial: "));
    Serial.println(input);

    if (input.startsWith("get x")) Serial.println(sensorStepper->getX());                                        // вывести текущую координату
    if (input.startsWith("goto")) sensorStepper->goToX(input.substring(5, input.length()).toInt());                            // отправить в координату, пример goto 1000    
  }
}