void CheckNewSMS()
{
    char Result[255] = { 0x00 };
    char Buffer[128] = { 0x00 };
    SendCMD("AT+CMGR=0", 5000, Result);
    SendCMD("AT+CMGD=0,4", 5000);

    if(strstr(Result, "CMGR"))
    {
        strtok(Result, ",");
        LastNum = strtok(NULL, ",");
    
        LastNum++;
        LastNum[strlen(LastNum) - 1] = 0x00;
        
        strtok(NULL, "\"");
        strtok(NULL, "\"");
        char* Command = strtok(NULL, "\"");
    
        // Ubication
        if(strstr(Command, "Ubi") || strstr(Command, "ubi"))
        {
            char lat[16];
            char lon[16];
            char altitude[8];
            char hdop[8];
            
            dtostrf(gps.lat, 1, 6, lat);
            dtostrf(gps.lon, 1, 6, lon);
            dtostrf(gps.altitude, 1, 2, altitude);
            dtostrf(gps.hdop, 1, 1, hdop);
            
            sprintf(Buffer, "https://www.google.com/maps/search/?api=1&query=%s,%s\nSpeed: %d - Altitude: %s - Satellites: %d - Hdop: %s", lat, lon, gps.kmh, altitude, gps.satinuse, hdop);
            SendSMS(Buffer);
        }
        // Voltage
        else if(strstr(Command, "Volt") || strstr(Command, "volt"))
        {
            char Volt[8] = { 0x00 };
            dtostrf(GetVoltage(), 8, 2, Volt);
            sprintf(Buffer, "Voltaje: %s volts", Volt);
            SendSMS(Buffer);
        }
        // Update
        else if(strstr(Command, "Update"))
        {
            UpdateToServer("forced");
            SendSMS("Ubicacion actualizada");
        }
        // Temp
        else if(strstr(Command, "Temp"))
        {
            int32_t VRef = readVref();
            sprintf(Buffer, "Temp: %d C", readTempSensor(VRef));
            SendSMS(Buffer);
        }
         // Apagar
        else if(strstr(Command, "Apagar"))
        {
            digitalWrite(PB11, HIGH);
            delay(1000);
            SendSMS("APAGADO!");
        }
         // Encender
        else if(strstr(Command, "Encender"))
        {
            digitalWrite(PB11, LOW);
            delay(1000);
            SendSMS("ENCENDIDO!");
        }
        // AT command
        else
        {    
            if(strstr(LastNum, MY_NUMBER))
            {
                Command = strupr(Command);
                if(strstr(Command, "AT"))
                {
                    SendCMD(Command, 5000, Buffer);
                    Buffer[strlen(Buffer) - 1] = 0x00;
                    Buffer[strlen(Buffer) - 1] = 0x00;
                    SendSMS(Buffer);
                }
            }
        }
    }
}
