#pragma once
#include <IPAddress.h>

class AbsNetworkConnection {
public:
    // Виртуальный деструктор нужен для правильного освобождения ресурсов 
    // при удалении объектов через указатель на базовый класс
    virtual ~AbsNetworkConnection() {}


    virtual bool setup() = 0;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() = 0;
    virtual IPAddress getIP() = 0;

    // Можно добавить другие методы на ваше усмотрение
    // Например: перезагрузить соединение, получить IP-адрес, и т.п.
    // virtual void reconnect() = 0;
    // virtual IPAddress getLocalIP() = 0;
};
