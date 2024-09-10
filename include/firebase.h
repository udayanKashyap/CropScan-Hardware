#ifndef FIREBASE_H
#define FIREBASE_H

#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "env.h"

String uid;

// Variables to save database paths
String databasePath;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void initFirebase()
{
    // Assign the api key (required)
    config.api_key = API_KEY;

    // Assign the user sign in credentials
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    // Assign the RTDB URL (required)
    config.database_url = DATABASE_URL;

    Firebase.reconnectWiFi(true);
    fbdo.setResponseSize(4096);

    // Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    // Assign the maximum retry of token generation
    config.max_token_generation_retry = 5;

    // Initialize the library with the Firebase authen and config
    Firebase.begin(&config, &auth);

    // Getting the user UID might take a few seconds
    Serial.println("Getting User UID");
    while ((auth.token.uid) == "")
    {
        Serial.print('.');
        delay(1000);
    }
    // Print user UID
    uid = auth.token.uid.c_str();
    Serial.print("User UID: ");
    Serial.println(uid);
}

int getIntensity(String path)
{
    int value;
    if (Firebase.RTDB.getInt(&fbdo, path.c_str(), &value))
    {
        return value;
    }
    else
    {
        Serial.println("FAILED");
        Serial.printf("REASON: %s\n", fbdo.errorReason());
        return -1;
    }
}

void setLDR(float value)
{
    String path = "/voltage";
    if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value))
    {

        Serial.println("PASSED");
    }
    else
    {
        Serial.println("FAILED");
        Serial.printf("REASON: %s\n", fbdo.errorReason());
    }
}

void setLdrArray(float *arr, int n)
{
    String path = "/voltageArr";
    String values = "";
    for (int i = 0; i < n; i++)
    {
        String val = String(arr[i], 3);
        val += ',';
        values += val;
    }
    Serial.println(values);
    if (Firebase.RTDB.pushString(&fbdo, path.c_str(), values.c_str()))
    {
        Serial.println("PASSED");
    }
    else
    {
        Serial.println("FAILED");
        Serial.printf("REASON: %s\n", fbdo.errorReason());
    }
}

#endif