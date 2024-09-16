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
unsigned long voltageDelay = 10;
LED ledR, ledG, ledB;
LDR ldr1;
void sendBurst();

void setup()
{
    Serial.begin(115200);

    initWiFi();
    initFirebase();

    ledR.initLED(17, 0);
    ledG.initLED(16, 1);
    ledB.initLED(18, 2);
    ldr1.initLDR(35);
}

void loop()
{
    int Rintensity = 0, Gintensity = 0, Bintensity = 0, rgb = 0;
    ;

    // Get Database requests
    if (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)
    {
        sendDataPrevMillis = millis();
        int firebaseready = Firebase.ready();
        if (firebaseready)
        {
            String rgb = getRGB("/intensity");
            Serial.println(rgb);

            int n = rgb.length();
            int colourCount = 0;
            String R(""), G(""), B("");
            for (int i = 0; i < n; i++)
            {
                if (rgb[i] == ',')
                {
                    colourCount++;
                    continue;
                }
                if (colourCount == 0)
                {
                    R += rgb[i];
                }
                if (colourCount == 1)
                {
                    G += rgb[i];
                }
                if (colourCount == 2)
                {
                    B += rgb[i];
                }
            }
            Rintensity = (R.toInt() / 100.00) * 255;
            Gintensity = (G.toInt() / 100.00) * 255;
            Bintensity = (B.toInt() / 100.00) * 255;
        }
        ledR.setLED((Rintensity));
        ledG.setLED((Gintensity));
        ledB.setLED((Bintensity));

        Serial.printf("Timestamp: %d Intensity: %d_%d_%d\n", sendDataPrevMillis / 1000, Rintensity, Gintensity, Bintensity);
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
    int N = 50;
    float voltage[N];
    int counter = 0;
    while (counter < N)
    {
        if (millis() - sendDataPrevMillis > voltageDelay || sendDataPrevMillis == 0)
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
