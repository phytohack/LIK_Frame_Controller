#pragma once
#include <Arduino.h>

// описание протокола в message_protocol.md
// группа значений в таком порядке:
// 1) enum class
// 2) enumName Function
// 3) enumNameJSON Function
// 4) JSON2enum Function

enum class IncomeMsgTypeValue { IDENTITY_RESPONSE, DEVICE_COMMAND, DEVICE_STATE_REQUEST, UNKNOWN };


String IncomeMessageTypeName(IncomeMsgTypeValue type) {return "";};

String IncomeMessageTypeNameJSON(IncomeMsgTypeValue type) {
  switch (type) {
    case IncomeMsgTypeValue::IDENTITY_RESPONSE:
      return "identity_response";
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
      IncomeMessageTypeNameJSON(IncomeMsgTypeValue::IDENTITY_RESPONSE)) {
    return IncomeMsgTypeValue::IDENTITY_RESPONSE;
  } else if (value == IncomeMessageTypeNameJSON(IncomeMsgTypeValue::DEVICE_COMMAND)) {
    return IncomeMsgTypeValue::DEVICE_COMMAND;
  } else if (value == IncomeMessageTypeNameJSON(IncomeMsgTypeValue::DEVICE_STATE_REQUEST)) {
    return IncomeMsgTypeValue::DEVICE_STATE_REQUEST;
  } else {
    return IncomeMsgTypeValue::UNKNOWN;
  }
};

enum class OutcomeMsgTypeValue { DEVICE_STATE, DEVICE_COMMAND_RESPONSE, UNKNOWN };

String OutcomeMessageTypeNameJSON(OutcomeMsgTypeValue type) {
  switch (type) {
    case OutcomeMsgTypeValue::DEVICE_STATE:
      return "device_state";
    case OutcomeMsgTypeValue::DEVICE_COMMAND_RESPONSE:
      return "device_command_response";
    default:
      return "unknown";
  }
};