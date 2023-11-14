#include <Arduino.h>
#include <ArduinoJson.h>

#include "Devices/SensorStepperController.h"
#include "Settings/Settings.h"
#include "Utilities/WebServer.h"

SensorStepperController *sensorStepper;

const byte stepPin = 23;
const byte dirPin = 21;
const byte enaPin = 18;
const int stepsPerTurn = 800 * 200;  // скорость - шагов на оборот
const byte lsPin = 4;

void checkMonitor();
void handleIncomeMessage(String message);

void setup() {
  Serial.begin(115200);

  WebSocketManager.setup(ssid, password, staticIP, gateway, subnet, primaryDNS,
                         secondaryDNS);
  // цепочка обязанностей
  WebSocketManager.setIncomeMessageHandler(
      handleIncomeMessage);  // куда вебсокет отправляет сообщения на обработку

  // sensorStepper = new SensorStepperController(stepsPerTurn, stepPin, dirPin,
  // enaPin, lsPin);
  sensorStepper =
      new SensorStepperController(stepsPerTurn, stepPin, dirPin, enaPin);
}

long timer = 0;
void loop() {
  if (millis() - timer > 1000) {
    timer = millis();
    Serial.println('.');
  }

  checkMonitor();
}

void checkMonitor() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    Serial.print(F("Input from Serial: "));
    Serial.println(input);

    if (input.startsWith("get x"))
      Serial.println(sensorStepper->getX());  // вывести текущую координату
    if (input.startsWith("goto"))
      sensorStepper->goToX(
          input.substring(5, input.length())
              .toInt());  // отправить в координату, пример goto 1000
  }
}

void handleIncomeMessage(String message) {
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);
  // Check for deserialization errors
  if (error) {
    Logger.error("MessageHandler::HandleIncomeMessage -- Failed to parse JSON");
    return;
  }

  // const char *type = doc["msg_type"];
  const char *command_type = doc["command_type"];
  if (strcmp(command_type, "go_to_x") == 0) {
    int x = doc["x"].as<int>();
    sensorStepper->goToX(x);
  }
  else if (strcmp(command_type, "set_x") == 0) {
    int x = doc["x"].as<int>();
    sensorStepper->setX(x);
  }
}