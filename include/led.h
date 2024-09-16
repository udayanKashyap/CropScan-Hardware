#ifndef LED_H
#define LED_H
#include <Arduino.h>

#define DEFAULT_PWM_FREQ 100
#define DEFAULT_PWM_RESOLUTION 8

class LED
{
public:
    unsigned short int pin, resolution;
    unsigned int pwm_frequency;
    int channel;

    void initLED(int pin, int channel, int pwm_freq = DEFAULT_PWM_FREQ, int resolution = DEFAULT_PWM_RESOLUTION)
    {
        LED::pin = pin;
        pwm_frequency = pwm_freq;
        LED::resolution = resolution;
        LED::channel = channel;

        ledcAttachPin(pin, channel);
        ledcSetup(channel, pwm_freq, resolution);
    }

    void setLED(int dutyCycle)
    {
        ledcWrite(channel, dutyCycle);
    }
};

#undef DEFAULT_PWM_FREQ
#undef DEFAULT_PWM_RESOLUTION
#endif