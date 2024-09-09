#include <Arduino.h>

// #define FIREBASEJSON_USE_FS

#include "wifi.h"
#include "firebase.h"
#include "led.h"
#include "ldr.h"

#define BUTTON1 21

// Timer variables
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 1000;
LED led1;
LDR ldr1;
void sendBurst();

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

    // Database requests
    if (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)
    {
        sendDataPrevMillis = millis();
        if (Firebase.ready())
        {
            intensity = getIntensity("/intensity");
        }
        led1.setLED(intensity);
        Serial.printf("Timestamp: %d Intensity: %d\n", sendDataPrevMillis / 1000, intensity);
    }
    if (digitalRead(BUTTON1) == LOW)
    {
        Serial.println("Sending voltage data...");
        sendDataPrevMillis = 0;
        sendBurst();
        Serial.println("Voltage Data Sent");
    }
}

void sendBurst()
{
    int N = 20;
    float voltage[N];
    int counter = 0;
    while (counter < N)
    {
        if (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)
        {
            sendDataPrevMillis = millis();
            voltage[counter] = ldr1.readLDR() / 1000.00;
            Serial.printf("Timestamp: %d    Voltage: %f\n", sendDataPrevMillis / 1000, voltage[counter]);
            counter++;
        }
    }
    if (Firebase.ready())
    {
        setLdrArray(voltage, N);
    }
}
