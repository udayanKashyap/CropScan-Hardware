#include <Arduino.h>

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
bool global_starter=true;


LED ledR, ledG, ledB, ledUV, ledBuiltin;
LDR ldr1;
void sendBurst(String);

// Global state maintained
int Rintensity = 0, Gintensity = 0, Bintensity = 0, UVintensity = 0, rgb = 0;

void setup()
{
    Serial.begin(115200);

    initWiFi();
    initFirebase();

    ledBuiltin.initLED(LED_BUILTIN, 4);

    ledR.initLED(RED_PIN, 0);
    ledG.initLED(GREEN_PIN, 1);
    ledB.initLED(BLUE_PIN, 2);
    ledUV.initLED(UV_PIN, 3);
    ldr1.initLDR(LDR_PIN);
    int timerStart = millis();
    while(millis() - timerStart < 2000);
}

void loop(){
    // Reset the leds
    ledBuiltin.setLED(100);
    ledR.setLED(0);
    ledG.setLED(0);
    ledB.setLED(0);
    ledUV.setLED(0);

    // Get Database Reqs
    if(millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)
    {   
        ledBuiltin.setLED(255);

        // reset the last_data_sent timer
        sendDataPrevMillis = millis();
        int is_firebase_ready = Firebase.ready();
        if(is_firebase_ready)
        {   
            // get the starter flag
            bool analysis_start = getStarter();
            if(analysis_start && global_starter)
            {    
                ledBuiltin.setLED(255);
                {// RED DATA
                    Rintensity=255;
                    ledR.setLED(Rintensity);
                    Serial.println("Sending Red Data...");
                    sendBurst("R");
                    Rintensity=0;
                    ledR.setLED(0);
                }
                {// GREEN DATA
                    Gintensity=255;
                    ledG.setLED(Gintensity);
                    Serial.println("Sending Green Data...");
                    sendBurst("G");
                    Gintensity=0;
                    ledG.setLED(0);
                }
                {// BLUE DATA
                    Bintensity=255;
                    ledB.setLED(Bintensity);
                    Serial.println("Sending Blue Data...");
                    sendBurst("B");
                    Bintensity=0;
                    ledB.setLED(0);
                }
                {// UV DATA
                    UVintensity=255;
                    ledUV.setLED(UVintensity);
                    Serial.println("Sending UV Data...");
                    sendBurst("UV");
                    UVintensity=0;
                    ledUV.setLED(0);
                }
                // Set the starter flag to false
                bool successful_transaction = resetStarter();
                if (!successful_transaction)
                {
                    Serial.println("FAILED TRANSACTION");
                    global_starter=false;
                }     
            }
        }
    }
}


void sendBurst(String color)
{
    int N = 10;
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
        // String ppm = getPPM("/ppm");
        // Serial.printf("ppm: %s+\n", ppm);
        String ppm = color;
        uploadDataArr(voltage, N, Rintensity, Gintensity, Bintensity, UVintensity, ppm);
    }
}
