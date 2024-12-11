#include <Arduino.h>

// #define FIREBASEJSON_USE_FS

#include "wifi.h"
#include "firebase.h"
#include "led.h"
#include "ldr.h"

#define BUTTON1 0

#define RED_PIN 17
#define GREEN_PIN 16
#define BLUE_PIN 18
#define UV_PIN 19

#define LDR_PIN 35

// Timer variables
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 1000;
unsigned long voltageDelay = 50;
LED ledR, ledG, ledB, ledUV;
LDR ldr1;
void sendBurst();

// Global state maintained
int Rintensity = 0, Gintensity = 0, Bintensity = 0, UVintensity = 0, rgb = 0;

void setup()
{
    Serial.begin(115200);

    initWiFi();
    initFirebase();

    ledR.initLED(RED_PIN, 0);
    ledG.initLED(GREEN_PIN, 1);
    ledB.initLED(BLUE_PIN, 2);
    ledUV.initLED(UV_PIN, 3);
    ldr1.initLDR(LDR_PIN);
}

void loop()
{
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
            String R(""), G(""), B(""), UV("");
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
                if (colourCount == 3)
                {
                    UV += rgb[i];
                }
            }
            Rintensity = (R.toInt() / 100.00) * 255;
            Gintensity = (G.toInt() / 100.00) * 255;
            Bintensity = (B.toInt() / 100.00) * 255;
            UVintensity = (UV.toInt() / 100.00) * 255;
        }
        ledR.setLED((Rintensity));
        ledG.setLED((Gintensity));
        ledB.setLED((Bintensity));
        ledUV.setLED((UVintensity));

        Serial.printf("Timestamp: %d Intensity: %d_%d_%d_%d\n", sendDataPrevMillis / 1000, Rintensity, Gintensity, Bintensity, UVintensity);
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
        String ppm = getPPM("/ppm");
        Serial.printf("ppm: %s+\n", ppm);
        uploadDataArr(voltage, N, Rintensity, Gintensity, Bintensity, UVintensity, ppm);
    }
}
