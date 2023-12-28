#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

#include "Devices/StepperI2C.h"
#include "Model/Commands/StepperCommands.h"
#include "Model/Message.h"
#include "Settings/Settings.h"
#include "Utilities/WebSocketServerManager.h"

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
  void setThermalStepper(StepperI2C *thermalCamStepper) {
    this->thermalCamStepper = thermalCamStepper;
  }

  // Входящие
  // Общий приемник
  static void handleIncomeMessageToServer(int clientNum, String response);

  // Исходящие
  void sendCommandResponse(int requestMsgId, bool status);
  static void sendStepperPropertiesToMainController(StepperI2C *stepper);

  Command *createStepperCommand(IncomeMessage msg);
  StepperI2C *thermalCamStepper;
};

void MessageHandler_::handleIncomeMessageToServer(int clientNum, String strMsg) {
  Logger.debug("");
  Logger.debug("--- >>> INCOME MESSAGE");
  Logger.debug("----------------------------");
  Logger.debug(strMsg);
  Logger.debug("----------------------------");

  IncomeMessage msg = IncomeMessage(strMsg);

  if (msg.msgType == IncomeMsgTypeValue::IDENTITY_RESPONSE) {
    if (msg.jsonDoc["role"] == "main_controller") {
      Logger.debug("Get identity request with role == main_controller");
      WebSocketServerManager.setMainControllerClientNum(clientNum);
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
  }

  else if (msg.msgType == IncomeMsgTypeValue::DEVICE_STATE_REQUEST) {
    if (msg.jsonDoc["device_name"] == "thermal_camera_stepper")
      getInstance().sendStepperPropertiesToMainController(
          getInstance().thermalCamStepper);
  }

  else {
    Logger.debug("MessageHandler::HandleIncomeMessage PARSING MESSAGE ERROR");
  }
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
      return new StepperGoToXCommand(stepper, x, speed);
    } else
      return new StepperGoToXCommand(stepper, x);
  }

  if (msg.jsonDoc["command"] == StepperCommandTypeJSON(StepperCommandType::BASING))
    return new StepperBasingCommand(stepper);

  if (msg.jsonDoc["command"] == StepperCommandTypeJSON(StepperCommandType::PRECISE_BASING))
    return new StepperPreciseBasingCommand(stepper);
  
  if (msg.jsonDoc["command"] == StepperCommandTypeJSON(StepperCommandType::STOP))
    return new StepperStopCommand(stepper);
}

void MessageHandler_::sendStepperPropertiesToMainController(
    StepperI2C *stepper) {
  OutcomeMessage msg = OutcomeMessage();
  msg.addField("msg_type", OutcomeMessageTypeNameJSON(OutcomeMsgTypeValue::DEVICE_STATE));
  msg.addField("device_name", StepperDeviceNameJSON(stepper->deviceStepper));
  msg.addField("state", StepperStateNameJSON(stepper->getState()));
  msg.addField("abs_position", StepperPositionNameJSON(stepper->getPosition()));
  msg.addField("x", String(stepper->getCoordinate()));
  WebSocketServerManager.sendToMainController(msg);
}

void MessageHandler_::sendCommandResponse(int requestMsgId, bool status) {
  OutcomeMessage msg = OutcomeMessage();
  msg.addField("request_msg_id", String(requestMsgId));
  msg.addField("msg_type", OutcomeMessageTypeNameJSON(OutcomeMsgTypeValue::DEVICE_COMMAND_RESPONCE));
  msg.addField("status", status);
  WebSocketServerManager.sendToMainController(msg);
}

// --------- SINGLETON -----------
extern MessageHandler_ &MessageHandler;
MessageHandler_ &MessageHandler = MessageHandler.getInstance();
// --------- SINGLETON -----------
