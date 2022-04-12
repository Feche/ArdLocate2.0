void SendCMD(char* cmd, int wait, char* output = 0);

void SendCMD(char* cmd, int wait, char* output)
{
    char Str[256] = { NULL };
    int tick = millis();
    SIM.println(cmd);
    
    while (true)
    {
        if (SIM.available())
        {
            String SerialStr = SIM.readString();
            if (SerialStr.indexOf("OK") != -1)
            {
                sprintf(Str, "OK, %d ms", cmd, millis() - tick);
                Serial.println(Str);
                if(output)
                    SerialStr.toCharArray(output, 255);
                break;
            }
            else if (SerialStr.indexOf("ERROR") != -1)
            {
                sprintf(Str, "Command [%s] ERROR.", cmd);
                Serial.println(Str);
                break;
            }
        }
    
        if (millis() - tick > wait)
        {
            sprintf(Str, "Command [%s] timeout.", cmd);
            Serial.println(Str);
            break;
        }
    }
}

void SendCMD2(char* cmd, int wait, char* response)
{
    char Str[256] = { NULL };
    long tick = millis();
    SIM.println(cmd);
    
    while (true)
    {
        if (SIM.available())
        {
            String SerialStr = SIM.readString();
            if (SerialStr.indexOf(response) != -1)
            {
                sprintf(Str, "%s OK, %d ms", cmd, millis() - tick);
                Serial.println(Str);
                break;
            }
            else if (SerialStr.indexOf("ERROR") != -1)
            {
                sprintf(Str, "Command [%s] ERROR.", cmd);
                Serial.println(Str);
                break;
            }
        }
    
        if (millis() - tick > wait)
        {
            sprintf(Str, "Command [%s] timeout.", cmd);
            Serial.println(Str);
            break;
        }
    }
}

void SendSMS(char* sms)
{
    char Str[255] = { 0x00 };
    sprintf(Str, "AT+CMGS=\"%s\"", LastNum);
    SIM.println(Str);
    delay(50);
    sprintf(Str, "%s", sms);
    SIM.print(Str);
    delay(50);
    SIM.write(CTRL_Z);
}

float GetVoltage()
{
    return (ReadAnalog(A0) / 1362.0f) * 12.16f;
}

float ReadAnalog(int analog)
{
    unsigned long value = 0;
    for(int i = 0; i < 50; i++)
    {
        value += analogRead(analog);
        delay(1);
    }
    return value / 50;
}
