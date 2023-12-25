#include <Arduino.h>
#include <ArduinoJson.h>

// #include "Devices/SensorStepperController.h"
#include "Settings/Settings.h"
#include "Utilities/WebSocketServerManager.h"

#include "Devices/LowLevel/Multiplexer.h"
#include "Devices/Abstract/MultiplexerFactory.h"
#include "Devices/LowLevel/LimitSwitcher.h"
#include "Devices/StepperI2C.h"

#include "Model/MessageHandler.h"

#include "Settings/Settings.h"


void checkMonitor();
void setup_steppers();
// void handleIncomeMessage(int clientNum, String message);

StepperI2C *_thermalCamStepper;

void setup()
{
  Serial.begin(115200);

  WebSocketServerManager.setup(ssid, password, staticIP, gateway, subnet, primaryDNS,
                         secondaryDNS);
  
  // цепочка обязанностей
  WebSocketServerManager.setIncomeMessageHandler(MessageHandler.handleIncomeMessageToServer);

  setup_steppers();
  MessageHandler.getInstance().setThermalStepper(_thermalCamStepper);
}

long timer = 0;
void loop()
{
  // if (millis() - timer > 1000)
  // {
  //   timer = millis();
  //   Serial.println('.');
  // }

  checkMonitor();
  WebSocketServerManager.loop();
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

// void handleIncomeMessage(int clientNum, String message)
// {
//   DynamicJsonDocument doc(1024);
//   DeserializationError error = deserializeJson(doc, message);
//   // Check for deserialization errors
//   if (error)
//   {
//     Logger.error("MessageHandler::HandleIncomeMessage -- Failed to parse JSON");
//     return;
//   }

//   const char *msg_type = doc["msg_type"];
  
//   // Сообщение типа "Подключился Я, Главный Контроллер"
//   // { 
//   // "msg_type": "identity_response",
//   // "role": "dispatcher" 
//   // }

//   // если подключился main_controller
//   if (strcmp(msg_type, "identity_response") == 0) {
//     if (strcmp(doc["role"], "main_controller") == 0) {
//       WebSocketServerManager.setMainControllerClientNum(clientNum);
//     }
//     return;
//   }

//   // Сообщение типа "Послать такой-то степпер туда-то"
//   // {
//   //  'msg_type': 'stepper_command', 
//   //  'stepper_name': 'thermal_camera_stepper', 
//   //  'command_type': 'go_to_x', 
//   //  'x': '0', 'speed': 1000
//   // }
//   const char *command_type = doc["command_type"];

//   if (strcmp(command_type, "go_to_x") == 0)
//   {
//     int x = doc["x"].as<int>();
//     // Check if 'speed' element exists
//     if (doc.containsKey("speed") && !doc["speed"].isNull())
//     {
//       int speed = doc["speed"].as<int>();
//       _thermalCamStepper->goToX(x, speed);
//     }
//     else
//     {
//       _thermalCamStepper->goToX(x);
//     }
//   }
//   else if (strcmp(command_type, "basing") == 0) {
//           _thermalCamStepper->basePositioning();
//   }
//   else if (strcmp(command_type, "precise_basing") == 0) {
//           _thermalCamStepper->preciseBasePositioning();
//   }
// }

void setup_steppers()
{
  _thermalCamStepper = new StepperI2C(
      DeviceStepper::THERMAL_CAMERA_STEPPER,
      MultiplexerFactory.getMultiplexer(THERMAL_CAMERA_STEPPER_MUX_ADDRESS),
      StepperStepsPerTurn(DeviceStepper::THERMAL_CAMERA_STEPPER),
      THERMAL_CAMERA_STEPPER_STEP_PIN,
      THERMAL_CAMERA_STEPPER_DIR_PIN,
      THERMAL_CAMERA_STEPPER_ENA_PIN,
      new LimitSwitcher(MultiplexerFactory.getMultiplexer(THERMAL_CAMERA_STEPPER_BASE_LS_MUX_ADDRESS),
                        THERMAL_CAMERA_STEPPER_BASE_LS_PIN)
  );
  _thermalCamStepper->setPropertiesChangeCallback(MessageHandler.sendStepperPropertiesToMainController);
}