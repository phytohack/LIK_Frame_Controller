#include <Arduino.h>
#include <ArduinoJson.h>

// #include "Devices/SensorStepperController.h"
#include "Settings/Settings.h"
#include "Utilities/WebServer.h"

#include "Devices/LowLevel/Multiplexer.h"
#include "Devices/Abstract/MultiplexerFactory.h"
#include "Devices/LowLevel/LimitSwitcher.h"
#include "Devices/StepperI2C.h"

#include "Settings/Settings.h"
// SensorStepperController *sensorStepper;

// const byte stepPin = 23;
// const byte dirPin = 21;
// const byte enaPin = 18;
// const int stepsPerTurn = 800 * 200;  // скорость - шагов на оборот
// const byte lsPin = 4;

void checkMonitor();
void handleIncomeMessage(String message);

void setup_stepper();

StepperI2C *_thermalCamStepper;

void setup()
{
  Serial.begin(115200);

  WebSocketManager.setup(ssid, password, staticIP, gateway, subnet, primaryDNS,
                         secondaryDNS);
  // цепочка обязанностей
  WebSocketManager.setIncomeMessageHandler(
      handleIncomeMessage); // куда вебсокет отправляет сообщения на обработку

  setup_stepper();
}

long timer = 0;
void loop()
{
  if (millis() - timer > 1000)
  {
    timer = millis();
    Serial.println('.');
  }

  checkMonitor();
  WebSocketManager.loop();
}

void checkMonitor()
{
  if (Serial.available())
  {
    String input = Serial.readStringUntil('\n');

    Serial.print(F("Input from Serial: "));
    Serial.println(input);

    // if (input.startsWith("get x"))
    //   Serial.println(_thermalCamStepper->getX());  // вывести текущую координату
    if (input.startsWith("goto"))
      _thermalCamStepper->goToX(
          input.substring(5, input.length())
              .toInt()); // отправить в координату, пример goto 1000
    else if (input.startsWith("basing")) _thermalCamStepper->basePositioning();
    else if (input.startsWith("precise")) _thermalCamStepper->preciseBasePositioning();
  }
}

void handleIncomeMessage(String message)
{
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);
  // Check for deserialization errors
  if (error)
  {
    Logger.error("MessageHandler::HandleIncomeMessage -- Failed to parse JSON");
    return;
  }

  const char *command_type = doc["command_type"];
  if (strcmp(command_type, "go_to_x") == 0)
  {
    int x = doc["x"].as<int>();
    // Check if 'speed' element exists
    if (doc.containsKey("speed") && !doc["speed"].isNull())
    {
      int speed = doc["speed"].as<int>();
      _thermalCamStepper->goToX(x, speed);
    }
    else
    {
      _thermalCamStepper->goToX(x);
    }
  }
  else if (strcmp(command_type, "basing") == 0) {
          _thermalCamStepper->basePositioning();
  }
  else if (strcmp(command_type, "precise_basing") == 0) {
          _thermalCamStepper->preciseBasePositioning();
  }
  // else if (strcmp(command_type, "set_x") == 0) {
  //   int x = doc["x"].as<int>();
  //   _thermalCamStepper->setX(x);
  // }
}

void setup_stepper()
{
  _thermalCamStepper = new StepperI2C(
      MultiplexerFactory.getMultiplexer(THERMAL_CAMERA_STEPPER_MUX_ADDRESS),
      THERMAL_CAMERA_STEPPER_STEPS_PER_TURN,
      THERMAL_CAMERA_STEPPER_STEP_PIN,
      THERMAL_CAMERA_STEPPER_DIR_PIN,
      THERMAL_CAMERA_STEPPER_ENA_PIN,
      new LimitSwitcher(MultiplexerFactory.getMultiplexer(THERMAL_CAMERA_STEPPER_BASE_LS_MUX_ADDRESS),
                        THERMAL_CAMERA_STEPPER_BASE_LS_PIN)
  );
}