#include <Arduino.h>

#include "wifi.h"
#include "firebase.h"
#include "led.h"
#include "ldr.h"

// Timer variables
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 1000;
LED led1;
LDR ldr1;

void setup()
{
    Serial.begin(460800);

    initWiFi();
    initFirebase();

    led1.initLED(16);
    ldr1.initLDR(35);
}

void loop()
{
    int intensity = 0;

    float voltage;
    // Database requests
    if (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)
    {
        sendDataPrevMillis = millis();
        voltage = ldr1.readLDR() / 1000.00;

        if (Firebase.ready())
        {
            intensity = getIntensity("/intensity");
            setLDR(voltage);
        }

        led1.setLED(intensity);
        Serial.printf("Timestamp: %d Intensity: %d    Voltage: %f\n", sendDataPrevMillis / 1000, intensity, voltage);
    }
}