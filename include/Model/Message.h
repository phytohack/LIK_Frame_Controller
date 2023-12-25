#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

#include "Settings/MessageProtocol.h"
#include "Utilities/Logger.h"

// enum class IncomeMessageType { IDENTITY_RESPONSE, COMMAND, UNKNOWN };

class IncomeMessage {
 public:
  IncomeMessage(String msg);

  DynamicJsonDocument jsonDoc;

  uint8_t msgId;
  IncomeMsgTypeValue msgType = IncomeMsgTypeValue::UNKNOWN;
  bool requiredResponse = false;
};

IncomeMessage::IncomeMessage(String msg) : jsonDoc(1024) {
  DeserializationError error = deserializeJson(jsonDoc, msg);

  if (error) {
    Logger.error("IncomeMessage -- Failed to parse JSON");
    return;
  }

  msgType = JSON2IncomeMessageType(jsonDoc["msg_type"]);
  msgId = jsonDoc["msg_id"];

  if (jsonDoc.containsKey("required_response") &&
      !jsonDoc["required_response"].isNull()) {
    bool requiredResponse = jsonDoc["required_response"].as<bool>();
    Logger.print("Required Response exists and equal: ");
    Logger.println(String(requiredResponse));
  }
};

class OutcomeMessage {
 public:
  static int nextMsgId;  // Static member to keep track of the next message ID

  OutcomeMessage();

  void addField(String field, String value);
  void addField(String field, int value);
  void addField(String field, bool value);
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

void OutcomeMessage::addField(String field, bool value) {
  _msg += "\"";
  _msg += field;
  _msg += "\"";
  _msg += ":";
  _msg += value ? "true" : "false";
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