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

// Initialize the firebase object and make the connection
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
    }
    // Print user UID
    uid = auth.token.uid.c_str();
    Serial.print("User UID: ");
    Serial.println(uid);
}

// Get the integer intensity value at the path
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

// Set the voltage reading to the LDR path
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

// Push the voltage array of size n to the database
void setLdrArray(float *voltageArr, int n)
{
    String path = "/voltageArr";
    String values = "";
    for (int i = 0; i < n; i++)
    {
        String val = String(voltageArr[i], 3);
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

// Push the complete data array of size n to the database
void uploadDataArr(float *voltageArr, int n, int Rvalue, int Gvalue, int Bvalue, int UVvalue, String ppm)
{
    String path = "/voltageArr";
    String values = "";
    for (int i = 0; i < n; i++)
    {
        String val = String(voltageArr[i], 3);
        val += ',';
        values += val;
    }
    values += ":";
    values += String(Rvalue);
    values += ":";
    values += String(Gvalue);
    values += ":";
    values += String(Bvalue);
    values += ":";
    values += String(UVvalue);
    values += ":";
    values += String(ppm);

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

// Get the string of intensities of R,G,B in the database
String getRGB(String path)
{
    String value;
    if (Firebase.RTDB.getString(&fbdo, path.c_str(), &value))
    {
        return value;
    }
    else
    {
        Serial.println("FAILED");
        Serial.printf("REASON: %s\n", fbdo.errorReason());
        return "/0";
    }
}

// Get the ppm (details) from the database
String getPPM(String path)
{
    String value;
    if (Firebase.RTDB.getString(&fbdo, path.c_str(), &value))
    {
        return value;
    }
    else
    {
        Serial.println("FAILED");
        Serial.printf("REASON: %s\n", fbdo.errorReason());
        return "/0";
    }
}

// Get the starter flag from the database
bool getStarter(){
    String path = "/flag";
    bool analysis_start = false;
    if (Firebase.RTDB.getBool(&fbdo, path.c_str(), &analysis_start))
    {
        return analysis_start;
    }
    else
    {
        Serial.println("FAILED AT getStarter");
        Serial.printf("REASON: %s\n", fbdo.errorReason());
        return false;
    }
}

// reset the starter flag to false in the database
bool resetStarter(){
    String path = "/flag";
    bool analysis_start = false;
    if (Firebase.RTDB.setBool(&fbdo, path, analysis_start))
    {
        Serial.println("/nSUCCESSFULLY RESET STARTER");
        return true;    
    }
    else
    {
        Serial.println("FAILED AT putStarter");
        return false;
    }
}

#endif
