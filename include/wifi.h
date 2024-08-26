#ifndef WIFI_H
#define WIFI_H

#include <WiFiManager.h>
#include <WiFi.h>

#define DEFAULT_SSID "OnePlus Nord CE 2 "
#define DEFAULT_PASS "jilljill"
#define WIFI_CONNECT_TIMEOUT 5000 // 5000 msec

WiFiManager wifimanager;

// Initialise Wifi
void initWiFi()
{
    unsigned long wifi_connect_start_millis = millis();
    wifimanager.autoConnect("ESP32");
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - wifi_connect_start_millis > WIFI_CONNECT_TIMEOUT)
        {
            Serial.println("!! Failed to Initialise wifi !!");
        }
        Serial.print(".");
        delay(500);
    }
    Serial.println(WiFi.localIP());
    Serial.println();
}

// Create Wifi Access Point to create new connections
void createAP()
{
    WiFi.disconnect(1);
    Serial.println("Entering Config Mode...");

    IPAddress localIP(10, 0, 0, 1), gateway(10, 0, 0, 1), subnet(255, 255, 255, 0);
    wifimanager.setAPStaticIPConfig(localIP, gateway, subnet);

    wifimanager.startConfigPortal("ESP32-Config");
    return;
}

// Default Wifi settings
void initWiFiDefault()
{
    WiFi.begin(DEFAULT_SSID, DEFAULT_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.print("\nConnected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
}

#undef DEFAULT_SSID
#undef DEFAULT_PASS
#undef WIFI_CONNECT_TIMEOUT
#endif