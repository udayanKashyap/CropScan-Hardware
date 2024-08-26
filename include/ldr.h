#ifndef LDR_H
#define LDR_H
#include <Arduino.h>

#define DEFAULT_RESOLUTION 12
#define V_REF 3.3

class LDR
{
public:
    unsigned short int pin;
    unsigned short int resolution;

    void initLDR(int pin, int resolution = DEFAULT_RESOLUTION)
    {
        LDR::pin = pin;
        LDR::resolution = resolution;
        analogReadResolution(resolution);
    }

    int readLDR()
    {
        return analogReadMilliVolts(pin);
    }

    int readLDR_RAW()
    {
        return analogRead(pin);
    }
};

#undef DEFAULT_RESOLUTION
#undef V_REF
#endif