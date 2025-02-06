#pragma once
#include <Arduino.h>
#include <GyverStepper.h>

#include "Devices/LowLevel/LimitSwitcher.h"
#include "Devices/LowLevel/Multiplexer.h"
#include "Settings/Settings.h"
#include "Utilities/Timer.h"

class StepperI2C : public GStepper<STEPPER_I2C> {
  using PropertiesChangeCallback = std::function<void(StepperI2C*)>;

 public:
  // без концевика
  StepperI2C(DeviceStepper deviceStepper, Multiplexer* mux, int stepsPerTurn,
             byte stepPin, byte dirPin, byte enaPin)
      : deviceStepper(deviceStepper),
        GStepper<STEPPER_I2C>(mux, stepsPerTurn, stepPin, dirPin, enaPin) {
    mux->setupAsOutput();
  };

  // с концевиком
  StepperI2C(DeviceStepper deviceStepper, Multiplexer* mux, int stepsPerTurn,
             byte stepPin, byte dirPin, byte enaPin,
             LimitSwitcher* baseLimitSwitcher)
      : StepperI2C(deviceStepper, mux, stepsPerTurn, stepPin, dirPin, enaPin) {
    _baseLimitSwitcher = baseLimitSwitcher;
    delay(20);
    if (_baseLimitSwitcher->isPushed()) _setPosition(StepperPosition::BASE);
  };

  // за какой девайс отвечает шаговик (Settings/Steppers/StepperDeviceEnum.h)
  DeviceStepper deviceStepper;

  void setPropertiesChangeCallback(PropertiesChangeCallback callback) {
    _propertiesChangeCallback = callback;
  }

  StepperState getState() { return _state; };
  StepperPosition getPosition() { return _position; };
  int getCoordinate() { return convertFromXToCm(getCurrent()); };

  void goToX(long x);
  void goToX(long x, long speed);
  void goToX(long x, long speed, long acceleration);
  void basePositioning();
  void basePositioning(int speed, int acceleration);
  void preciseBasePositioning();

  int convertFromXToCm(int x) {
    return int(x / (400000/7897));
  }
  int convertFromCmToX(int cm) {
    return int(cm * (400000/7897));
  }

 private:
  LimitSwitcher* _baseLimitSwitcher = nullptr;

  StepperState _state = StepperState::HOLD;
  StepperPosition _position = StepperPosition::UNKNOWN;

  // если уже делали preciseBasing - то следом не нужно делать basing() или preciseBasing()
  bool _preciseBased = false;
  
  PropertiesChangeCallback _propertiesChangeCallback;  // все вместе - state, position, x

  void _setState(StepperState state);
  void _setPosition(StepperPosition pos);
  void _softStop(int acceleration);
  void _hardStop();
  void _updatePosition();
  void _handleBaseLimitSwitcherPushing();
};

void StepperI2C::_updatePosition() {
  // если нет концевика - позиция в любом случае неизвестна
  if (_baseLimitSwitcher == nullptr) {
    _setPosition(StepperPosition::UNKNOWN);
    return;
  }
  // если есть концевик и нажат - позиция BASE
  else if (_baseLimitSwitcher->isPushed()) {
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

void StepperI2C::_setState(StepperState state) {
  _state = state;
  if (_propertiesChangeCallback) {
    _propertiesChangeCallback(this);
  }
}

void StepperI2C::_setPosition(StepperPosition position) {
  _position = position;
  if (_propertiesChangeCallback) _propertiesChangeCallback(this);
}

// "Мягкая" остановка c заданным ускорением
void StepperI2C::_softStop(int acceleration) {
  setAcceleration(acceleration);
  stop();  // Плавная остановка с заданным ускорением

  // Дотикать
  while (tick()) {
    if (_baseLimitSwitcher->isPushed()) {
      _handleBaseLimitSwitcherPushing();
      return;
    }
  }
  _updatePosition();
  _setState(StepperState::HOLD);
}

void StepperI2C::_handleBaseLimitSwitcherPushing() {
  _hardStop();
  _updatePosition();
  setCurrent(0);
  _setState(StepperState::HOLD);
  Serial.println("Base limit switcher pushed");
}

void StepperI2C::_hardStop() {
  brake();

  // Откатываемся на 1 тик назад, чтобы не было резкого старта
  setRunMode(KEEP_SPEED);
  setSpeed(200);
  tick();
  stop();
}

// ДВИЖЕНИЕ К КООРДИНАТЕ Х
void StepperI2C::goToX(long x) {
  int speed = StepperSpeed(deviceStepper);
  int acc = StepperAcceleration(deviceStepper);
  goToX(x, speed, acc);
}

void StepperI2C::goToX(long x, long speed) {
  int acc = StepperAcceleration(deviceStepper);
  goToX(x, speed, acc);
}

void StepperI2C::goToX(long cm, long speed, long acceleration) {
  int x = convertFromCmToX(cm);

  // определить направление
  int curr = getCurrent();
  curr < x ? _setState(StepperState::MOVING_FORWARD)
                   : _setState(StepperState::MOVING_BACKWARD);

  
  setRunMode(FOLLOW_POS);  // режим
  setMaxSpeed(speed);      // скорость движения к цели
  // setSpeed(speed);      // скорость движения к цели
  setAcceleration(acceleration);  // ускорение
  setTarget(x);                   // цель

  Timer timer(100);
  timer.startTimer();
  while (tick()) {  // Ехать
    if (timer.isExpired()) {
      timer.startTimer();
      Serial.print(".");
      // убрал проверку концевика под таймер, потому что проверка каждый раз
      // делает задержку и медленно едет
      if (_state == StepperState::MOVING_BACKWARD &&
          _baseLimitSwitcher->isPushed()) {
        _handleBaseLimitSwitcherPushing();
        return;
      }
    }
  }

  _updatePosition();
  _setState(StepperState::HOLD);
  
  _preciseBased = false;
}

void StepperI2C::basePositioning() {
  int speed = StepperBasingSpeed(deviceStepper);
  int acc = StepperBasingAcceleration(deviceStepper);
  basePositioning(speed, acc);
}

void StepperI2C::basePositioning(int speed, int acc) {
  if (_preciseBased) {
    Logger.debug("No need to Basing - already precise based");
    return;
  }
  
  if (_baseLimitSwitcher == nullptr) {
    Logger.warn("Base Positioning Imposible. Base Limit Switcher Not Set");
    return;
  }

  _setState(StepperState::MOVING_BACKWARD);
  setRunMode(KEEP_SPEED);
  setSpeed(-speed);
  setAcceleration(acc);
  Timer timer(100);
  timer.startTimer();
  while (tick()) {
    if (timer.isExpired()) {
      timer.startTimer();
      if (_baseLimitSwitcher->isPushed()) {
        _handleBaseLimitSwitcherPushing();
        break;
      }
    }
  }

  _preciseBased = false;
}

void StepperI2C::preciseBasePositioning() {
  if (_preciseBased) {
    Logger.debug("No need to Precise Basing - already precise based");
    return;
  }
  
  if (_baseLimitSwitcher == nullptr) {
    Logger.warn("Base Positioning Imposible. Base Limit Switcher Not Set");
    return;
  }

  basePositioning();  // выполнить обычное позиционирование
  
  int speed = StepperPreciseBasingSpeed(deviceStepper);
  int acc = StepperPreciseBasingAcceleration(deviceStepper);
  int distance = StepperPreciseBasingDistance(deviceStepper);
  
  goToX(distance);  // отъехать на небольшое расстояние
  basePositioning(speed, acc);  // выполнить позиционирование очень медленно

  _preciseBased = true;
}