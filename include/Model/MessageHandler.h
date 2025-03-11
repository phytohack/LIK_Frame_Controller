#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

#include "Devices/StepperI2C.h"
#include "Devices/Sensor.h"
#include "Model/Commands/StepperCommands.h"
#include "Model/Commands/SensorCommands.h"
#include "Model/Message.h"
#include "Settings/Settings.h"
#include "Utilities/WebSocketServerManager.h"

#include "Utilities/Logger/Logger.h"


// типа функций, в которые будут передаваться распарсенные команды
// using ThermalCameraStepperCommandHandler = std::function<void(long)>;

class MessageHandler_ {
 public:
  // --------- SINGLETON -----------
  static MessageHandler_ &getInstance() {
    static MessageHandler_ instance;
    return instance;
  }

 private:
  MessageHandler_() = default;
  MessageHandler_(const MessageHandler_ &) = delete;
  MessageHandler_ &operator=(const MessageHandler_ &) = delete;
  // --------- SINGLETON -----------

 public:
  void setThermalCamStepper(StepperI2C *thermalCamStepper) {
    this->thermalCamStepper = thermalCamStepper;
  }
  void setThermalCamSensor(Sensor *thermalCamSensor) {
    this->thermalCamSensor = thermalCamSensor;
  }

  // Входящие
  // Общий приемник
  static void handleIncomeMessageToServer(int clientNum, String response);

  // Исходящие
  void sendCommandResponse(uint32_t requestMsgId, bool status);
  static void sendStepperPropertiesToMainController(StepperI2C *stepper);
  static void sendSensorStateToMainController(Sensor *sensor);

  Command *createStepperCommand(IncomeMessage msg);
  Command *createSensorCommand(IncomeMessage msg);
  
  StepperI2C *thermalCamStepper;
  Sensor *thermalCamSensor;
};

void MessageHandler_::handleIncomeMessageToServer(int clientNum, String strMsg) {
  Logger.info("");
  Logger.info("[MAIN CONTROLLER]    --->    [FRAME CONTROLLER]");
  Logger.info("----------------------------");
  Logger.info(strMsg);
  Logger.info("----------------------------");

  IncomeMessage msg = IncomeMessage(strMsg);

  if (msg.msgType == IncomeMsgTypeValue::IDENTITY_RESPONSE) {
    if (msg.jsonDoc["role"] == "main_controller") {
      Logger.info("Get identity request with role == main_controller");
      WebSocketServerManager.setMainControllerClientNum(clientNum);
      getInstance().sendStepperPropertiesToMainController(getInstance().thermalCamStepper);
      getInstance().sendSensorStateToMainController(getInstance().thermalCamSensor);
    }
  }

  else if (msg.msgType == IncomeMsgTypeValue::DEVICE_COMMAND) {
    if (msg.jsonDoc["device_type"] == "stepper") {
      Command *command = getInstance().createStepperCommand(msg);
      bool status = false;
      if (command) {
        command->execute();
        delete command;
        status = true;
      }
      getInstance().sendCommandResponse(msg.msgId, status);
    }
    else if (msg.jsonDoc["device_type"] == "sensor") {
      Command *command = getInstance().createSensorCommand(msg);
      bool status = false;
      if (command) {
        command->execute();
        delete command;
        status = true;
      }
      getInstance().sendCommandResponse(msg.msgId, status);
    }
  }

  else if (msg.msgType == IncomeMsgTypeValue::DEVICE_STATE_REQUEST) {
    if (msg.jsonDoc["device_name"] == "thermal_camera_stepper")
      getInstance().sendStepperPropertiesToMainController(
          getInstance().thermalCamStepper);
  }

  else {
    Logger.info("MessageHandler::HandleIncomeMessage PARSING MESSAGE ERROR");
  }
}

Command *MessageHandler_::createSensorCommand(IncomeMessage msg) {
  if (msg.jsonDoc["device_name"] == SensorDeviceNameJSON(DeviceSensor::THERMAL_CAMERA)) {
    if (msg.jsonDoc["command"] == "on") {
      return new SensorTurnOnCommand(getInstance().thermalCamSensor);
    }
    else if (msg.jsonDoc["command"] == "off") {
      return new SensorTurnOffCommand(getInstance().thermalCamSensor);
    }
  }
  return nullptr;
}
Command *MessageHandler_::createStepperCommand(IncomeMessage msg) {
  StepperI2C *stepper = nullptr;
  // выбор степпера
  if (msg.jsonDoc["device_name"] == StepperDeviceNameJSON(DeviceStepper::THERMAL_CAMERA_STEPPER)) {
    stepper = getInstance().thermalCamStepper;
  }
  // выбор команды
  if (msg.jsonDoc["command"] == StepperCommandTypeJSON(StepperCommandType::GO_TO_X)) {
    int x = msg.jsonDoc["x"].as<int>();
    if (msg.jsonDoc.containsKey("speed") && !msg.jsonDoc["speed"].isNull()) {
      int speed = msg.jsonDoc["speed"].as<int>();
      return new FrameStepperGoToXCommand(stepper, x, speed);
    } else
      return new FrameStepperGoToXCommand(stepper, x);
  }

  if (msg.jsonDoc["command"] == StepperCommandTypeJSON(StepperCommandType::BASING))
    return new FrameStepperBasingCommand(stepper);

  if (msg.jsonDoc["command"] == StepperCommandTypeJSON(StepperCommandType::PRECISE_BASING))
    return new FrameStepperPreciseBasingCommand(stepper);
  
  if (msg.jsonDoc["command"] == StepperCommandTypeJSON(StepperCommandType::STOP))
    return new FrameStepperStopCommand(stepper);

  Logger.error("MessageHandler::createStepperCommand: Unknown command");
  return nullptr;
}

void MessageHandler_::sendSensorStateToMainController(Sensor * sensor) {
  OutcomeMessage msg = OutcomeMessage();
  msg.addField("msg_type", OutcomeMessageTypeNameJSON(OutcomeMsgTypeValue::DEVICE_STATE));
  msg.addField("device_type", String("sensor"));
  msg.addField("device_name", SensorDeviceNameJSON(sensor->getDeviceSensor()));
  msg.addField("state", SensorStateNameJSON(sensor->getState()));
  WebSocketServerManager.sendToMainController(msg);
}

void MessageHandler_::sendStepperPropertiesToMainController(
    StepperI2C *stepper) {
  OutcomeMessage msg = OutcomeMessage();
  msg.addField("msg_type", OutcomeMessageTypeNameJSON(OutcomeMsgTypeValue::DEVICE_STATE));
  msg.addField("device_type", String("stepper"));
  msg.addField("device_name", StepperDeviceNameJSON(stepper->deviceStepper));
  msg.addField("state", StepperStateNameJSON(stepper->getState()));
  msg.addField("abs_position", StepperPositionNameJSON(stepper->getPosition()));
  msg.addField("x", String(stepper->getCoordinate()));
  WebSocketServerManager.sendToMainController(msg);
}

void MessageHandler_::sendCommandResponse(uint32_t requestMsgId, bool status) {
  OutcomeMessage msg = OutcomeMessage();
  msg.addField("request_msg_id", String(requestMsgId));
  msg.addField("msg_type", OutcomeMessageTypeNameJSON(OutcomeMsgTypeValue::DEVICE_COMMAND_RESPONSE));
  msg.addField("status", status);
  WebSocketServerManager.sendToMainController(msg);
}

// --------- SINGLETON -----------
extern MessageHandler_ &MessageHandler;
MessageHandler_ &MessageHandler = MessageHandler.getInstance();
// --------- SINGLETON -----------
