#include <Arduino.h>
#include <ArduinoJson.h>

// #include "Devices/SensorStepperController.h"
#include "Settings/Settings.h"
#include "Utilities/WebSocketServerManager.h"

#include "Devices/LowLevel/Multiplexer.h"
#include "Devices/Abstract/MultiplexerFactory.h"
#include "Devices/LowLevel/LimitSwitcher.h"
#include "Devices/StepperI2C.h"
#include "Devices/Sensor.h"

#include "Model/MessageHandler.h"

#include "Settings/Settings.h"


void checkMonitor();
void setupSteppers();
void setupSensors();
// void handleIncomeMessage(int clientNum, String message);

// ВСЕ СТЕППЕРЫ БУДУТ ЗДЕСЬ.
// Настройка - в setupSteppers()
StepperI2C *_thermalCamStepper;
Sensor *_thermalCamSensor;

void setup()
{
  Serial.begin(115200);

  WebSocketServerManager.setupWiFi(ssid, password, staticIP, gateway, subnet, primaryDNS, secondaryDNS);
  // WebSocketServerManager.setupETH(staticIP, gateway, subnet, primaryDNS, secondaryDNS);
  
  // цепочка обязанностей
  WebSocketServerManager.setIncomeMessageHandler(MessageHandler.handleIncomeMessageToServer);

  setupSteppers();
  setupSensors();
  // MessageHandler.getInstance().setThermalStepper(_thermalCamStepper);
  MessageHandler.setThermalStepper(_thermalCamStepper);
  MessageHandler.setThermalCamSensor(_thermalCamSensor);
}

long timer = 0;
void loop()
{
  if (millis() - timer > 5000)
  {
    timer = millis();
    WebSocketServerManager.printMainControllerConnectionState();
  }

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
    else if (input.startsWith("thermal cam on")) _thermalCamSensor->turnOn();
    else if (input.startsWith("thermal cam off")) _thermalCamSensor->turnOff();
    else if (input.startsWith("client disconnect")) WebSocketServerManager.disconnectClient();
    else if (input.startsWith("reboot")) ESP.restart();
  }
}

void setupSteppers()
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

void setupSensors() 
{
  _thermalCamSensor = new Sensor(
    DeviceSensor::THERMAL_CAMERA,
    MultiplexerFactory.getMultiplexer(THERMAL_CAMERA__MUX_ADDRESS),
    THERMAL_CAMERA_TURN_ON_PIN);
  _thermalCamSensor->setStateChangeCallback(MessageHandler.sendSensorStateToMainController);
}