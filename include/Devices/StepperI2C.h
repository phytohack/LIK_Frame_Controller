#pragma once
#include <Arduino.h>
#include <GyverStepper.h>

#include "Devices/LowLevel/LimitSwitcher.h"
#include "Devices/LowLevel/Multiplexer.h"
#include "Settings/Settings.h"
#include "Utilities/Timer.h"

class StepperI2C : public GStepper<STEPPER_I2C> {
  using StateChangeCallback = std::function<void(StepperI2C&)>;
  // using PositionChangeCallback = std::function<void(StepperI2C&)>;
  using PositionChangeCallback = std::function<void(StepperPosition)>;
  using CoordinateChangeCallback = std::function<void(long)>;

 public:
  // без концевика
  StepperI2C(Multiplexer* mux, int stepsPerTurn, byte stepPin, byte dirPin,
             byte enaPin)
      : GStepper<STEPPER_I2C>(mux, stepsPerTurn, stepPin, dirPin, enaPin) {
    mux->setupAsOutput();
  };

  // с концевиком
  StepperI2C(Multiplexer* mux, int stepsPerTurn, byte stepPin, byte dirPin,
             byte enaPin, LimitSwitcher* baseLimitSwitcher)
      : GStepper<STEPPER_I2C>(mux, stepsPerTurn, stepPin, dirPin, enaPin),
        _baseLimitSwitcher(baseLimitSwitcher) {
    mux->setupAsOutput();
    if (_baseLimitSwitcher->isPushed()) _setPosition(StepperPosition::BASE);
  };

  void setStateChangeCallback(StateChangeCallback callback) {
    _stateChangeCallback = callback;
  }
  void setPositionChangeCallback(PositionChangeCallback callback) {
    _positionChangeCallback = callback;
  }

  void setCoordinateChangeCallback(CoordinateChangeCallback callback) {
    _coordinateChangeCallback = callback;
  }

  StepperState getState() { return _state; };
  StepperPosition getPosition() { return _position; };

  void goToX(long x, long speed = 1000, long acceleration = 200);
  // позиционирование на базе по концевику
  void basePositioning(int speed = THERMAL_CAMERA_BASE_POSITIONING_SPEED,
                       int acceleration = THERMAL_CAMERA_BASE_POSITIONING_ACCELERATION);
  // точное позиционирование на базе (с отъездом и медленным возвратом)
  void preciseBasePositioning();
  // long getCurrent();

 private:
  // Multiplexer _mux;
  LimitSwitcher* _baseLimitSwitcher = nullptr;

  StepperState _state = StepperState::HOLD;
  StateChangeCallback _stateChangeCallback;
  PositionChangeCallback _positionChangeCallback;
  CoordinateChangeCallback _coordinateChangeCallback;

  StepperPosition _position = StepperPosition::UNKNOWN;

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
  if (_stateChangeCallback) _stateChangeCallback(*this);

  // если приехали куда-то - то послать еще и новую координату
  if (_coordinateChangeCallback && _state == StepperState::HOLD)
    _coordinateChangeCallback(getCurrent());
}

void StepperI2C::_setPosition(StepperPosition position) {
  _position = position;
  // if (_positionChangeCallback) _positionChangeCallback(*this);
  if (_positionChangeCallback) _positionChangeCallback(position);
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
void StepperI2C::goToX(long x, long speed, long acceleration) {
  // определить направление
  getCurrent() < x ? _setState(StepperState::MOVING_FORWARD)
                   : _setState(StepperState::MOVING_BACKWARD);

  setRunMode(FOLLOW_POS);  // режим
  setMaxSpeed(speed);      // скорость движения к цели
  // setSpeed(speed);      // скорость движения к цели
  setAcceleration(acceleration);  // ускорение
  setTarget(x);                   // цель

  Timer timer(100);
  timer.startTimer();
  while (tick()) {  // Ехать
    if (timer.isItTime()) {
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
}

void StepperI2C::basePositioning(int speed, int acc) {
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
    if (timer.isItTime()) {
      timer.startTimer();
      if (_baseLimitSwitcher->isPushed()) {
        _handleBaseLimitSwitcherPushing();
        break;
      }
    }
  }
}

void StepperI2C::preciseBasePositioning() {
  if (_baseLimitSwitcher == nullptr) {
    Logger.warn("Base Positioning Imposible. Base Limit Switcher Not Set");
    return;
  }

  basePositioning();  // выполнить обычное позиционирование
  goToX(THERMAL_CAMERA_PRECISE_BASE_POSITIONING_DISTANCE);  // отъехать на небольшое
                                                   // расстояние
  basePositioning(
      THERMAL_CAMERA_PRECISE_BASE_POSITIONING_SPEED);  // выполнить позиционирование
                                              // очень медленно
}