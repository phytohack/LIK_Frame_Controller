#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <sys/time.h>   // Для функции settimeofday()

// Класс Clock реализован по схеме singleton – только один экземпляр может существовать.
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

    /**
     * @brief Инициализация часов.
     * 
     * Устанавливаем системное время в ESP32 на основе значения по умолчанию.
     * Пока устройство не подключится к сети, время будет обновляться от этого значения.
     *
     * @param year   Год (по умолчанию 2025)
     * @param month  Месяц (по умолчанию 1)
     * @param day    День (по умолчанию 1)
     * @param hour   Часы (по умолчанию 0)
     * @param minute Минуты (по умолчанию 0)
     * @param second Секунды (по умолчанию 0)
     */
public:    
    void initialize(int year = 2025, int month = 2, int day = 1, int hour = 0, int minute = 0, int second = 0) {
        // Заполняем структуру tm значениями по умолчанию.
        struct tm t = {0};
        t.tm_year = year - 1900; // В структуре tm год считается от 1900.
        t.tm_mon  = month - 1;   // Месяцы нумеруются от 0.
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min  = minute;
        t.tm_sec  = second;
        // Преобразуем структуру tm в количество секунд с 1 января 1970 (time_t).
        _defaultTime = mktime(&t);

        // Устанавливаем системное время с помощью settimeofday().
        struct timeval tv;
        tv.tv_sec  = _defaultTime;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);

        // Отмечаем, что синхронизация с NTP ещё не выполнена.
        _synchronized = false;

        Serial.print("Default time set to: ");
        Serial.println(getFormattedDateTime());
    }

    /**
     * @brief Синхронизация времени с NTP-сервером.
     * 
     * Функция настраивает получение времени с указанного NTP-сервера. После вызова
     * configTime() система обновит текущее время, если синхронизация пройдет успешно.
     * В данном примере мы ждём, пока время (полученное через time(nullptr))
     * станет меньше времени по умолчанию – это означает, что NTP-сервер вернул корректное время.
     *
     * @param ntpServer         Адрес NTP-сервера (по умолчанию "pool.ntp.org")
     * @param gmtOffset_sec     Сдвиг по Гринвичу в секундах (например, 3*3600 для GMT+3)
     * @param daylightOffset_sec Сдвиг для летнего времени (обычно 0)
     * @param timeout_ms        Таймаут ожидания синхронизации (по умолчанию 10 сек)
     */
    void synchronize(const char* ntpServer = "pool.ntp.org", long gmtOffset_sec = 3 * 3600, int daylightOffset_sec = 0, unsigned long timeout_ms = 10000) {
        // Настраиваем получение времени с NTP-сервера.
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        Serial.println("Synchronizing time with NTP...");

        unsigned long start = millis();
        time_t now = time(nullptr);

        // Ждем синхронизации: в этом примере считается, что если время (now) стало больше _defaultTime,
        // то NTP-сервер вернул корректное время.
        // while ((now = time(nullptr)) >= _defaultTime && (millis() - start < timeout_ms)) {
        while ((now = time(nullptr)) <= _defaultTime && (millis() - start < timeout_ms)) {
            Serial.print(".");
            delay(500);
        }

        // Если время так и не изменилось, считаем, что синхронизация не удалась.
        // if (now >= _defaultTime) {
        if (now <= _defaultTime) {
            Serial.println("\nTime synchronization failed. Continuing with default time.");
        } else {
            Serial.println("\nTime synchronized successfully.");
            _synchronized = true;
        }
    }

    /**
     * @brief Получение текущего времени в формате "HH:MM:SS".
     */
    String getTime() {
        return getFormattedTime();
    }

    /**
     * @brief Получение текущей даты в формате "YYYY-MM-DD".
     */
    String getDate() {
        return getFormattedDate();
    }

    /**
     * @brief Получение текущей даты и времени в формате "YYYY-MM-DD HH:MM:SS".
     */
    String getDateTime() {
        return getFormattedDateTime();
    }

    /**
     * @brief Проверка, выполнена ли синхронизация времени с NTP.
     */
    bool isSynchronized() const {
        return _synchronized;
    }

private:
    // Приватный конструктор – для реализации singleton.
    Clock_() : _defaultTime(0), _synchronized(false) {}

    // Удаляем конструктор копирования и оператор присваивания, чтобы нельзя было создать копии.
    // Clock(const Clock&) = delete;
    // Clock& operator=(const Clock&) = delete;

    // Внутренние переменные:
    time_t _defaultTime;   // Значение времени по умолчанию, установленное при инициализации.
    bool   _synchronized;  // Флаг: время синхронизировано с NTP или нет.

    // Вспомогательная функция для форматирования текущего системного времени.
    // Принимает формат (например, "%H:%M:%S") и размер буфера.
    String formatTime(const char* format, size_t bufferSize) {
        time_t now = time(nullptr);
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);  // Получаем локальное время.
        char buffer[bufferSize];
        strftime(buffer, bufferSize, format, &timeinfo);
        return String(buffer);
    }

    // Возвращает время в формате "HH:MM:SS".
    String getFormattedTime() {
        return formatTime("%H:%M:%S", 10);
    }

    // Возвращает дату в формате "YYYY-MM-DD".
    String getFormattedDate() {
        return formatTime("%Y-%m-%d", 11);
    }

    // Возвращает дату и время в формате "YYYY-MM-DD HH:MM:SS".
    String getFormattedDateTime() {
        return formatTime("%Y-%m-%d %H:%M:%S", 20);
    }
};


// ------------- SINGLETON ---------------
extern Clock_ &Clock;
Clock_ &Clock = Clock.getInstance();
// ------------- SINGLETON ---------------
