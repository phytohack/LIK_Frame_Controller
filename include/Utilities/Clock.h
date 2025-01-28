#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

enum class TimeStatus {
    UNSYNCHRONIZED,
    SYNCHRONIZED
};

class Clock_ {
// ------------- SINGLETON ---------------
public:
    static Clock_& getInstance() {
        static Clock_ instance;
        return instance;
    }
private:
    Clock_(const Clock_&) = delete;
    Clock_& operator=(const Clock_&) = delete;
// ------------- SINGLETON ---------------

public:
    void initialize(const char* ntpServer = "pool.ntp.org", long gmtOffset_sec = 0, int daylightOffset_sec = 0, unsigned long timeout_ms = 10000) {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        Serial.println("Synchronizing time...");
        unsigned long start = millis();
        while (!time(nullptr)) {
            if (millis() - start > timeout_ms) {
                Serial.println("\nTime sync failed. Starting from 00:00:00.");
                _status = TimeStatus::UNSYNCHRONIZED;
                _setBaseTime(2025, 1, 1, 0, 0, 0);
                _lastUpdate = millis();
                break;
            }
            Serial.print(".");
            delay(500);
        }
        if (_status != TimeStatus::UNSYNCHRONIZED) {
            Serial.println("\nTime synchronized.");
            _status = TimeStatus::SYNCHRONIZED;
        }
    }

    void update() {
        if (_status == TimeStatus::UNSYNCHRONIZED) {
            unsigned long currentMillis = millis();
            unsigned long elapsed = currentMillis - _lastUpdate;
            _lastUpdate = currentMillis;

            _incrementTime(elapsed);
        }
    }

    String getTime() {
        update();
        if (_status == TimeStatus::SYNCHRONIZED) {
            return getFormattedTime();
        } else {
            return formatTime();
        }
    }

    String getDate() {
        update();
        if (_status == TimeStatus::SYNCHRONIZED) {
            return getFormattedDate();
        } else {
            return formatDate();
        }
    }

    String getDateTime() {
        update();
        if (_status == TimeStatus::SYNCHRONIZED) {
            return getFormattedDateTime();
        } else {
            return formatDate() + " " + formatTime();
        }
    }

    bool isSynchronized() const {
        return _status == TimeStatus::SYNCHRONIZED;
    }

private:
    
    Clock_() : _status(TimeStatus::UNSYNCHRONIZED), _lastUpdate(0) {}

    void _setBaseTime(int year, int month, int day, int hour, int min, int sec) {
        _baseTime.tm_year = year - 1900;
        _baseTime.tm_mon = month - 1;
        _baseTime.tm_mday = day;
        _baseTime.tm_hour = hour;
        _baseTime.tm_min = min;
        _baseTime.tm_sec = sec;
        _baseTime.tm_isdst = 0;
    }
    
    void _incrementTime(unsigned long elapsed_ms) {
        // Добавляем прошедшее время к baseTime
        time_t rawtime = mktime(&_baseTime) + elapsed_ms / 1000;
        gmtime_r(&rawtime, &_baseTime);
    }

    String getFormattedTime() {
        char buffer[10];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", &_currentTime);
        return String(buffer);
    }

    String getFormattedDate() {
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &_currentTime);
        return String(buffer);
    }

    
    String getFormattedDateTime() {
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &_currentTime);
        return String(buffer);
    }

    String formatTime() {
        char buffer[9];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", &_baseTime);
        return String(buffer);
    }

    String formatDate() {
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &_baseTime);
        return String(buffer);
    }
    
    TimeStatus _status;
    unsigned long _lastUpdate;
    struct tm _baseTime;
    struct tm _currentTime;
};

// ------------- SINGLETON ---------------
extern Clock_ &Clock;
Clock_ &Clock = Clock.getInstance();
// ------------- SINGLETON ---------------