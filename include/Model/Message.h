#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

#include "Settings/MessageProtocol.h"
#include "Utilities/Logger.h"

// enum class IncomeMessageType { IDENTITY_RESPONCE, COMMAND, UNKNOWN };

class IncomeMessage {
 public:
  IncomeMessage(String msg);

  DynamicJsonDocument jsonDoc;

  uint8_t msgId;
  IncomeMsgType msgType = IncomeMsgType::UNKNOWN;
  bool requiredResponce = false;
};

IncomeMessage::IncomeMessage(String msg) : jsonDoc(1024) {
  DeserializationError error = deserializeJson(jsonDoc, msg);

  if (error) {
    Logger.error("IncomeMessage -- Failed to parse JSON");
    return;
  }

  msgType = JSON2IncomeMessageType(jsonDoc["msg_type"]);
  msgId = jsonDoc["msg_id"];

  if (jsonDoc.containsKey("required_responce") &&
      !jsonDoc["required_responce"].isNull()) {
    bool requiredResponce = jsonDoc["required_responce"].as<bool>();
    Logger.print("Required Responce exists and equal: ");
    Logger.println(String(requiredResponce));
  }
};

class OutcomeMessage {
 public:
  static int nextMsgId;  // Static member to keep track of the next message ID

  OutcomeMessage();

  void addField(String field, String value);
  void addField(String field, int value);
  String getMessage();

 private:
  String _msg = "";
};

// Starting from 0 or any other desired value
int OutcomeMessage::nextMsgId = 0;

// Use the current msg_id, then increment
OutcomeMessage::OutcomeMessage() { addField("msg_id", String(nextMsgId++)); }

void OutcomeMessage::addField(String field, int value) {
  _msg += "\"";
  _msg += field;
  _msg += "\"";
  _msg += ":";
  _msg += String(value);
  _msg += ",";
}

void OutcomeMessage::addField(String field, String value) {
  _msg += "\"";
  _msg += field;
  _msg += "\"";
  _msg += ":";
  _msg += "\"";
  _msg += value;
  _msg += "\"";
  _msg += ",";
};

String OutcomeMessage::getMessage() {
  if (!_msg.isEmpty() && _msg[_msg.length() - 1] == ',') {
    _msg.remove(_msg.length() - 1);  // Remove the trailing comma
  }
  return "{" + _msg + "}";
}