#pragma once
#include <Arduino.h>

class Timer {
public:
  Timer();
  Timer(unsigned long timer_value);

  void setTimer(unsigned long timer_value);
  unsigned long getTimer();
  void startTimer();
  void resetTimer();

  bool isItTime();
  bool hasMinutePassed();

  unsigned long timeLeft();
  unsigned long timeLeftInSec();
  unsigned long timeLeftInMin();

private:
  unsigned long _timer_value = 0;
  unsigned long _start_time = 0;

  long _last_time_in_min_requested = -1;
};

Timer::Timer() {}

Timer::Timer(unsigned long timer_value) {
  setTimer(timer_value);
}

void Timer::setTimer(unsigned long timer_value) {
  _timer_value = timer_value;
}

unsigned long Timer::getTimer() {
  return _timer_value;
}

void Timer::startTimer() {
  _start_time = millis();
  _last_time_in_min_requested = -1;  // чтобы при первом запросе точно отдал значение
}

void Timer::resetTimer() {
  _start_time = 0;
}

bool Timer::isItTime() {
  if (millis() - _start_time > _timer_value) return true;
  else return false;
}

unsigned long Timer::timeLeft() {
  if (millis() - _start_time < _timer_value) return _timer_value - (millis() - _start_time);
  else return 0;
}

unsigned long Timer::timeLeftInSec() {
  return timeLeft() / 1000;
}

unsigned long Timer::timeLeftInMin() {
  return timeLeftInSec() / 60;
}

bool Timer::hasMinutePassed() {
  long time_left_in_minutes = timeLeftInMin();
  if (_last_time_in_min_requested != time_left_in_minutes) {
    _last_time_in_min_requested = time_left_in_minutes;
    return true;
  }
  return false;
}