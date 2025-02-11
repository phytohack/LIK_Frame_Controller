#include <Arduino.h>
#include <ArduinoJson.h>

#include "Settings/Settings.h"
#include "Utilities/SPIFFSManager.h"
#include "Utilities/Logger/Logger.h"

#include "Utilities/Network/NetworkManager.h"
#include "Utilities/WebSocketServerManager.h"
#include "Utilities/Clock.h"
#include "Utilities/HttpServerManager.h"

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

// ВСЕ СТЕППЕРЫ БУДУТ ЗДЕСЬ (настройка - в setupSteppers(), вызывается из setup())
// 1. ТЕПЛОВИЗОР
StepperI2C *_thermalCamStepper;
Sensor *_thermalCamSensor;

void setup()
{
  Serial.begin(115200);
  SPIFFSManager.begin(); // инициализация SPIFFS
  // SPIFFSManager.clearDir("/logs"); // очистка папки с логами
  Clock.initialize(); // инициализация часов с нулевым временем
  
  // ЛОГЕР
  Logger.setup(); // добавляются Serial и SPIFFS логгеры

  // Тип подключения (WiFi или ETH) - в Settings/NetworkSettings.h
  NetworkManager.begin();
  Clock.synchronize(); // синхронизация времени с NTP сервером
  HttpServerManager.begin(); // запуск HTTP сервера

  // цепочка обязанностей
  WebSocketServerManager.setIncomeMessageHandler(MessageHandler.handleIncomeMessageToServer);

  setupSteppers();
  setupSensors();
  // MessageHandler.getInstance().setThermalStepper(_thermalCamStepper);
  MessageHandler.setThermalCamStepper(_thermalCamStepper);
  MessageHandler.setThermalCamSensor(_thermalCamSensor);
}

long timer = 0;
void loop()
{
  if (millis() - timer > 5000)
  {
    timer = millis();
    //WebSocketServerManager.printMainControllerConnectionState();
  }

  checkMonitor();
  NetworkManager.maintainConnection(); // поддержание соединения
  WebSocketServerManager.loop();
  HttpServerManager.handleClient();
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
    else if (input.startsWith("clear logs")) SPIFFSManager.clearDir("/logs");
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