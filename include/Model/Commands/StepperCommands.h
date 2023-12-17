#pragma once 
#include <Arduino.h>

#include "Devices/StepperI2C.h"
#include "Model/Message.h"

class Command {
 public:
  virtual ~Command() {}
  virtual void execute() = 0;

  bool responce_required;
};

class StepperCommand : public Command {
 public:
  StepperCommand(StepperI2C *stepper) : stepper(stepper){};
  StepperI2C *stepper;
};

class StepperGoToXCommand : public StepperCommand {
 public:
  StepperGoToXCommand(StepperI2C *stepper, int x) : StepperCommand(stepper), x(x) {}
  StepperGoToXCommand(StepperI2C *stepper, int x, int speed)
      : StepperCommand(stepper), x(x), speed(speed) {}

  int x;
  int speed = 0;

  void execute() override {
    if (speed) stepper->goToX(x, speed);
    else stepper->goToX(x);
  }
};

class StepperBasingCommand : public StepperCommand {
 public:
  StepperBasingCommand(StepperI2C *stepper) : StepperCommand(stepper){}

  void execute() override {
    stepper->basePositioning();
  }
};

class StepperPreciseBasingCommand : public StepperCommand {
 public:
  StepperPreciseBasingCommand(StepperI2C *stepper) : StepperCommand(stepper){}

  void execute() override {
    stepper->preciseBasePositioning();
  }
};

class StepperStopCommand : public StepperCommand {
 public:
  StepperStopCommand(StepperI2C *stepper) : StepperCommand(stepper){}

  void execute() override {
    stepper->stop();
  }
};