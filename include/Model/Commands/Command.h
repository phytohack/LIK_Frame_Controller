#pragma once 
#include <Arduino.h>

class Command {
 public:
  virtual ~Command() {}
  virtual void execute() = 0;

  bool response_required;
};