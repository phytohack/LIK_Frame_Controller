#pragma once
#include <Arduino.h>
#include <vector>

#include "Settings/Logger.h"
#include "Utilities/Logger/ILogHandler.h"
#include "Utilities/Logger/SerialLogHandler.h"
#include "Utilities/Logger/SPIFFSLogHandler.h"

#include "Utilities/Clock.h"

/*
Менеджер логирования.
Управляет "синками" логов ILogHandlers (Serial, SD, WiFi, etc).
*/

class Logger_
{
// ------------- SINGLETON ---------------
public:
  static Logger_ &getInstance()
  {
    static Logger_ instance;
    return instance;
  }
private:
  Logger_(){};
  Logger_(const Logger_ &) = delete;
  Logger_ &operator=(const Logger_ &) = delete;

// ------------- SINGLETON ---------------
public:
  void setup()
  {
    SerialLogHandler* serialHandler = new SerialLogHandler(LogLevel::DEBUG);
    SpiffsLogHandler* spiffsHandler = new SpiffsLogHandler(LogLevel::ERROR);

    addHandler(serialHandler);
    addHandler(spiffsHandler);
  }

  // Добавить новый обработчик
  void addHandler(ILogHandler* handler) {
      _handlers.push_back(handler);
  }

  void log(LogLevel level, const char* message) {
      // Проходим по всем обработчикам и шлём
      for (auto handler : _handlers) {
          handler->logMessage(level, _formatMessage(level, message));
      }
  }
  // Перегруженные методы для String
  void log(LogLevel level, const String &message) {
      log(level, message.c_str());
  }

  // Удобные шорткаты:
  void debug(const char* msg)  { log(LogLevel::DEBUG,   msg); }
  void info(const char* msg)   { log(LogLevel::INFO,    msg); }
  void warn(const char* msg)   { log(LogLevel::WARN,    msg); }
  void error(const char* msg)  { log(LogLevel::ERROR,   msg); }
  void fatal(const char* msg)  { log(LogLevel::FATAL,   msg); }

  // Удобные шорткаты с String
  void debug(const String &msg)  { log(LogLevel::DEBUG,   msg); }
  void info(const String &msg)   { log(LogLevel::INFO,    msg); }
  void warn(const String &msg)   { log(LogLevel::WARN,    msg); }
  void error(const String &msg)  { log(LogLevel::ERROR,   msg); }
  void fatal(const String &msg)  { log(LogLevel::FATAL,   msg); }


private:
  // Храним набор обработчиков (sinks)
  std::vector<ILogHandler*> _handlers;  

  String _formatMessage(LogLevel level, String msg) {
    String datetime = Clock.getDateTime();
    return "[" + datetime + "] " + logLevelToString(level) + " " + msg;
  }
};


// ------------- SINGLETON ---------------
extern Logger_ &Logger;
Logger_ &Logger = Logger.getInstance();
// ------------- SINGLETON ---------------