//#include "stm32yyxx_ll_adc.h"

#define SIM           Serial1

#define CR            0x0D
#define CTRL_Z        0x1A

#define UPDATE_TIME   5
#define GPS_MIN_SPEED   5

#define MY_NUMBER "+541122512171"

char* LastNum = MY_NUMBER;

void setup()
{
    analogReadResolution(12);
    
    pinMode(PB11, OUTPUT);
    digitalWrite(PB11, LOW);
    
    Startup();
    // Send SMS
    SendSMS("ArdLocate 2.0 started");
    
    Serial.println("Ready.");
}

void loop()
{    
    while(Serial.available())
    {
        String SerialStr = Serial.readString();
        SIM.println(SerialStr);
    }

    while(SIM.available())
    {
        char sData[2048] = { 0x00 };
        SIM.readBytes(sData, 2048);

        Serial.println(sData);

        // SMS Received
        if(strstr(sData, "+CMTI:"))
        {
            CheckNewSMS();
        }
        // GPS NMEA received
        else if(strstr(sData, "$GN") || strstr(sData, "$GP"))
        {
            OnNMEAReceive(sData);
        }
    }
}
