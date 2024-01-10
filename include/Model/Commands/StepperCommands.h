#pragma once 
#include <Arduino.h>

#include "Devices/StepperI2C.h"
#include "Model/Message.h"

class Command {
 public:
  virtual ~Command() {}
  virtual void execute() = 0;

  bool response_required;
};

class FrameStepperCommand : public Command {
 public:
  FrameStepperCommand(StepperI2C *stepper) : stepper(stepper){};
  StepperI2C *stepper;
};

class FrameStepperGoToXCommand : public FrameStepperCommand {
 public:
  FrameStepperGoToXCommand(StepperI2C *stepper, int x) : FrameStepperCommand(stepper), x(x) {}
  FrameStepperGoToXCommand(StepperI2C *stepper, int x, int speed)
      : FrameStepperCommand(stepper), x(x), speed(speed) {}

  int x;
  int speed = 0;

  void execute() override {
    if (speed) stepper->goToX(x, speed);
    else stepper->goToX(x);
  }
};

class FrameStepperBasingCommand : public FrameStepperCommand {
 public:
  FrameStepperBasingCommand(StepperI2C *stepper) : FrameStepperCommand(stepper){}

  void execute() override {
    stepper->basePositioning();
  }
};

class FrameStepperPreciseBasingCommand : public FrameStepperCommand {
 public:
  FrameStepperPreciseBasingCommand(StepperI2C *stepper) : FrameStepperCommand(stepper){}

  void execute() override {
    stepper->preciseBasePositioning();
  }
};

class FrameStepperStopCommand : public FrameStepperCommand {
 public:
  FrameStepperStopCommand(StepperI2C *stepper) : FrameStepperCommand(stepper){}

  void execute() override {
    stepper->stop();
  }
};