void Startup()
{
    char Buffer[48];
    
    Serial.begin(115200);
    SIM.begin(115200);
    
    Serial.setTimeout(5);
    SIM.setTimeout(5);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(PC13, HIGH);
    
    unsigned long StartTick = millis();
    while (true)
    {
        if (millis() - StartTick >= 1000)
        {
            StartTick = millis();
            SendCMD("AT+CREG?", 1000, Buffer);
    
            if (strstr(Buffer, "0,1"))
                break;
            else
            {
                Serial.println("Waiting for modem..");
                Serial.println(Buffer);
                Serial.println("-------------------");
            }
        }
    }
    
    // Configure modem
    Serial.println("Configuring modem..");
    SendCMD("AT+ATE0", 5000);
    SendCMD("AT+CMGF=1", 5000);
    SendCMD("AT+CNMI=1,1,0,0,0", 5000);
    SendCMD("AT+HTTPINIT", 5000);
    SendCMD("AT+CMGD=0,4", 5000);
    
    // Configure GPS
    Serial.println("Configuring GPS..");
    SendCMD("AT+CGPSXDAUTO=1", 5000);
    SendCMD("AT+CGPSURL=\"supl.google.com:7276\"", 5000);
    delay(2000);
    SendCMD("AT+CGPS=1,1", 5000);
    sprintf(Buffer, "AT+CGPSINFOCFG=%d,272", UPDATE_TIME);
    SendCMD(Buffer, 5000);
}
