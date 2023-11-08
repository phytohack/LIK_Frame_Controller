#pragma once
#include <Arduino.h>
#include <GyverStepper.h>

#include "Devices/LimitSwitcher.h"
#include "Settings/StepperPositionSettings.h"
#include "Settings/StepperStateSettings.h"
#include "Utilities/Logger.h"

class SensorStepperController {
  using StateChangeCallback = std::function<void(SensorStepperController&)>;
  using PositionChangeCallback = std::function<void(SensorStepperController&)>;

 public:
  // конструктор без концевика
  SensorStepperController(int stepsPerTurn, byte stepPin, byte dirPin,
                          byte enaPin)
      : _stepper(stepsPerTurn, stepPin, dirPin, enaPin) {
    _updatePosition();
  };

  // конструктор с концевиком
  SensorStepperController(int stepsPerTurn, byte stepPin, byte dirPin,
                          byte enaPin, byte baseLSPin)
      : _baseLimitSwitcher(baseLSPin),
        _stepper(stepsPerTurn, stepPin, dirPin, enaPin) {
    _updatePosition();
  };

  void setStateChangeCallback(StateChangeCallback callback) {
    _stateChangeCallback = callback;
  }
  void setPositionChangeCallback(PositionChangeCallback callback) {
    _positionChangeCallback = callback;
  }

  StepperState getState() { return _state; }
  StepperPosition getPosition() { return _position; };

  void goToX(long x, long speed = 1000, long acceleration = 200);
  void basePositioning(
      int speed = basePositioningSpeed,
      int acc = basePositioningAcceleration);  // позиционирование на базе
  void preciseBasePositioning();  // точное позиционирование (с отъездом назад и
                                  // медленным возвратом)

 private:
  GStepper<STEPPER2WIRE> _stepper;
  LimitSwitcher _baseLimitSwitcher;

  StepperState _state = StepperState::HOLD;
  StepperPosition _position = StepperPosition::UNKNOWN;

  StateChangeCallback _stateChangeCallback;
  PositionChangeCallback _positionChangeCallback;

  void _setState(StepperState state);
  void _setPosition(StepperPosition pos);
  void _softStop(int acceleration = 500);
  void _hardStop();

  void _handleBaseLimitSwitcherPushing();
  void _updatePosition();
  // void updateState();
};

void SensorStepperController::_updatePosition() {
  // если нет концевика - позиция в любом случае неизвестна
  if (!_baseLimitSwitcher.isPinSet()) {
    _setPosition(StepperPosition::UNKNOWN);
    return;
  }
  // если есть концевик и нажат - позиция BASE
  else if (_baseLimitSwitcher.isPushed()) {
    _setPosition(StepperPosition::BASE);
    return;
  }
  // если есть концевик, не нажат и была известна позиция - тогда NONBASE
  else if (_position != StepperPosition::UNKNOWN) {
    _setPosition(StepperPosition::NONBASE);
    return;
  }
  // иначе позиция так и остается UNKNOWN
}

void SensorStepperController::_setState(StepperState state) {
  _state = state;
  if (_stateChangeCallback) _stateChangeCallback(*this);
}

void SensorStepperController::_setPosition(StepperPosition position) {
  _position = position;
  if (_positionChangeCallback) _positionChangeCallback(*this);
}

// "Мягкая" остановка c заданным ускорением
void SensorStepperController::_softStop(int acceleration) {
  _stepper.setAcceleration(acceleration);
  _stepper.stop();  // Плавная остановка с заданным ускорением

  // Дотикать
  while (_stepper.tick()) {
    if (_baseLimitSwitcher.isPushed()) {
      _handleBaseLimitSwitcherPushing();
      return;
    }

    _updatePosition();
    _setState(StepperState::HOLD);
  }
}

void SensorStepperController::_handleBaseLimitSwitcherPushing() {
  _hardStop();
  _updatePosition();
  _stepper.setCurrent(1);
}

void SensorStepperController::_hardStop() {
  _stepper.brake();

  // Откатываемся на 1 тик назад, чтобы не было резкого старта
  _stepper.setRunMode(KEEP_SPEED);
  _stepper.setSpeed(200);
  _stepper.tick();
  _stepper.stop();

  _setState(StepperState::HOLD);
}

void SensorStepperController::goToX(long x, long speed,
                                    long acceleration) {
  _stepper.getCurrent() < x ? _setState(StepperState::MOVING_FORWARD)
                            : _setState(StepperState::MOVING_BACKWARD);

  _stepper.setRunMode(FOLLOW_POS);
  _stepper.setMaxSpeed(speed);
  _stepper.setAcceleration(acceleration);
  _stepper.setTarget(x);

  while (_stepper.tick()) {
    if (_state == StepperState::MOVING_BACKWARD &&
        _baseLimitSwitcher.isPushed()) {
      _handleBaseLimitSwitcherPushing();
      return;
    }
  }

  _updatePosition();
  _setState(StepperState::HOLD);
}

void SensorStepperController::basePositioning(int speed, int acc) {
  if (!_baseLimitSwitcher.isPinSet()) {
    Logger.warn("Base Positioning Imposible. Base Limit Switcher Not Set");
    return;
  }

  _stepper.setRunMode(KEEP_SPEED);
  _stepper.setMaxSpeed(-speed);
  _stepper.setAcceleration(acc);

  while (_stepper.tick() && !_baseLimitSwitcher.isPushed()) {
  }
  _handleBaseLimitSwitcherPushing();
}

void SensorStepperController::preciseBasePositioning() {
  if (!_baseLimitSwitcher.isPinSet()) {
    Logger.warn(
        "Precise Base Positioning Imposible. Base Limit Switcher Not Set");
    return;
  }

  basePositioning();  // выполнить обычное позиционирование
  goToX(preciseBasePositioningDistance); // отъехать на небольшое расстояние
  basePositioning(preciseBasePositioningSpeed); // выполнить позиционирование очень медленно
}
