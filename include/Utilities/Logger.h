#pragma once
#include <Arduino.h>

#define WRITE_LOG_TO_SERIAL true

class Logger_
{
private:
  Logger_() = default; // make constructor private

  enum LogLevel
  {
    OFF,
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG
  };

  String _dataBuffer = "";
  LogLevel _currentLogLevel = DEBUG;

public:
  static Logger_ &getInstance();

  void setLogLevel(LogLevel level) { _currentLogLevel = level; }

  void print(String data);
  void print(const __FlashStringHelper *ifsh);
  void println();
  void println(String data);
  void println(const __FlashStringHelper *ifsh);

  void debug(const char *message)
  {
    if (_currentLogLevel >= DEBUG)
      println("[DEBUG] " + String(message));
  }

  void info(const char *message)
  {
    if (_currentLogLevel >= INFO)
      println("[INFO] " + String(message));
  }

  void warn(const char *message)
  {
    if (_currentLogLevel >= WARN)
      println("[WARN] " + String(message));
  }

  void error(const char *message)
  {
    if (_currentLogLevel >= ERROR)
      println("[ERROR] " + String(message));
  }

  void fatal(const char *message)
  {
    if (_currentLogLevel >= FATAL)
      println("[FATAL] " + String(message));
  }
};

Logger_ &Logger_::getInstance()
{
  static Logger_ instance;
  return instance;
}

extern Logger_ &Logger;
Logger_ &Logger = Logger.getInstance();

void Logger_::print(String data)
{
  _dataBuffer = _dataBuffer + data;
}

void Logger_::println()
{
  println("");
}

void Logger_::println(String data)
{
  // if (!WRITE_LOG_TO_SD_CARD && !WRITE_LOG_TO_SERIAL) return; // если оба лога выключены (Serial и SD) - выходим

  // Date date = Clock.getDate();
  // Time time = Clock.getTime();

  String finalData = "";
  // finalData += _convertDateAndTimeToLogFormat(date, time);
  finalData += _dataBuffer;
  finalData += data;

  // if (WRITE_LOG_TO_SD_CARD) {
  //   String fileName = _convertDateToFileNameFormat(date);
  //   writeToFile(fileName, finalData);
  // }

  if (WRITE_LOG_TO_SERIAL)
  {
    Serial.println(finalData);
  }

  _dataBuffer = "";
}

void Logger_::print(const __FlashStringHelper *data)
{
  char buffer[256]; // adjust the size according to your needs
  strcpy_P(buffer, (PGM_P)data);
  print(buffer);
}

void Logger_::println(const __FlashStringHelper *data)
{
  char buffer[256]; // adjust the size according to your needs
  strcpy_P(buffer, (PGM_P)data);
  println(buffer);
}
