#pragma once
#include <Arduino.h>
#include <vector>

#include "Settings/Logger.h"
#include "Utilities/Logger/ILogHandler.h"
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

  String _dataBuffer = "";
  LogLevel _currentLogLevel = DEBUG;
// ------------- SINGLETON ---------------

  // Добавить новый обработчик
  void addHandler(ILogHandler* handler) {
      _handlers.push_back(handler);
  }

  void log(LogLevel level, const char* message) {
      // Проходим по всем обработчикам и шлём
      for (auto handler : _handlers) {
          handler->logMessage(level, message);
      }
  }

  // Удобные "шорткаты":
  void debug(const char* msg)  { log(LogLevel::DEBUG,   msg); }
  void info(const char* msg)   { log(LogLevel::INFO,    msg); }
  void warn(const char* msg)   { log(LogLevel::WARN,    msg); }
  void error(const char* msg)  { log(LogLevel::ERROR,   msg); }
  void fatal(const char* msg)  { log(LogLevel::FATAL,   msg); }

private:
    // Храним набор обработчиков (sinks)
    std::vector<ILogHandler*> _handlers;  
};


// ------------- SINGLETON ---------------
extern Logger_ &Logger;
Logger_ &Logger = Logger.getInstance();
// ------------- SINGLETON ---------------