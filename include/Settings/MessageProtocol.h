#pragma once
#include <Arduino.h>

// описание протокола в message_protocol.md
// группа значений в таком порядке:
// 1) enum class
// 2) enumName Function
// 3) enumNameJSON Function
// 4) JSON2enum Function

enum class IncomeMsgTypeValue { IDENTITY_RESPONCE, DEVICE_COMMAND, DEVICE_STATE_REQUEST, UNKNOWN };

String IncomeMessageTypeName(IncomeMsgTypeValue type) {};

String IncomeMessageTypeNameJSON(IncomeMsgTypeValue type) {
  switch (type) {
    case IncomeMsgTypeValue::IDENTITY_RESPONCE:
      return "identity_responce";
    case IncomeMsgTypeValue::DEVICE_COMMAND:
      return "device_command";
    case IncomeMsgTypeValue::DEVICE_STATE_REQUEST:
      return "device_state_request";
    default:
      return "unknown";
  }
};

IncomeMsgTypeValue JSON2IncomeMessageType(String value) {
  if (value ==
      IncomeMessageTypeNameJSON(IncomeMsgTypeValue::IDENTITY_RESPONCE)) {
    return IncomeMsgTypeValue::IDENTITY_RESPONCE;
  } else if (value == IncomeMessageTypeNameJSON(IncomeMsgTypeValue::DEVICE_COMMAND)) {
    return IncomeMsgTypeValue::DEVICE_COMMAND;
  } else if (value == IncomeMessageTypeNameJSON(IncomeMsgTypeValue::DEVICE_STATE_REQUEST)) {
    return IncomeMsgTypeValue::DEVICE_STATE_REQUEST;
  } else {
    return IncomeMsgTypeValue::UNKNOWN;
  }
};