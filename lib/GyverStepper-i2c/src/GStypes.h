#ifndef _GStypes_h
#define _GStypes_h
enum GS_driverType {
    STEPPER2WIRE,
    STEPPER4WIRE,
    STEPPER4WIRE_HALF,
    STEPPER_PINS,
    STEPPER_VIRTUAL,
	STEPPER_I2C
};


enum GS_posType {
    ABSOLUTE,
    RELATIVE,
};

#endif