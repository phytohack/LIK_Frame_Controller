#pragma once
#include <Arduino.h>
#include "Devices/LowLevel/Multiplexer.h"
#include "Settings/Settings.h"

#include <map>

class MultiplexerFactory_ {

public:
	static MultiplexerFactory_& getInstance() {
        static MultiplexerFactory_ instance;
	    return instance;
    };
    
    Multiplexer* getMultiplexer(int address) {
        if (multiplexers.find(address) == multiplexers.end()) {
            Logger.info("Creating new multiplexer with address " + String(address));
            multiplexers[address] = new Multiplexer(address, SDA_PIN, SCL_PIN);
        } else {
            Logger.warn("Multiplexer with address " + String(address) + " already exists");
        }
        return multiplexers[address];
    }

private:
	MultiplexerFactory_(){};

    std::map<int, Multiplexer*> multiplexers;

	// Deleted copy constructor and assignment operator to prevent copying.
	MultiplexerFactory_(const MultiplexerFactory_&) = delete;
	MultiplexerFactory_& operator=(const MultiplexerFactory_&) = delete;
};

// Чтобы вызывать просто как MultiplexerFactory (без _)
extern MultiplexerFactory_ &MultiplexerFactory;
MultiplexerFactory_ &MultiplexerFactory = MultiplexerFactory.getInstance();