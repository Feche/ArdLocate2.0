struct GPS
{
    int kmh;
    char utc[12];
    float lat;
    float lon;
    char mode[8];
    int satinuse;
    float hdop;
    float altitude;
    bool moving;
};

GPS gps = { 0, "000000", 0.000000, 0.000000, "X", 0, 0, 0, false };

void OnNMEAReceive(char* sData)
{
    char* split_string = strtok(sData, "\r\n");
    while(split_string)
    {
        // Speed
        if(strstr(split_string, "GPVTG"))
        {
            strsep(&split_string, ",");
            strsep(&split_string, ",");
            strsep(&split_string, ",");
            strsep(&split_string, ",");
            strsep(&split_string, ",");
            strsep(&split_string, ",");
            strsep(&split_string, ",");
            char* kmh = strsep(&split_string, ",");
            kmh = strlen(kmh) == 0 ? (char*)"0" : kmh;
            gps.kmh = atoi(kmh);
        }
        // Position
        else if (strstr(split_string, "GNGNS"))
        {            
            strsep(&split_string, ",");

            // UTC time
            char* utc = strsep(&split_string, ",");
            utc = strlen(utc) == 0 ? (char*)"000000" : utc;
            memcpy(&gps.utc, utc, 12);

            // Latitude
            char* lat = strsep(&split_string, ",");
            char* latdir = strsep(&split_string, ",");
            gps.lat = ConvertDegreeAngleToDouble(lat, latdir);

            // Longitude
            char* lon = strsep(&split_string, ",");
            char* londir = strsep(&split_string, ",");
            gps.lon = ConvertDegreeAngleToDouble(lon, londir);

            // Mode
            char* mode = strsep(&split_string, ",");
            mode = strlen(mode) == 0 ? (char*)"X" : mode;
            memcpy(&gps.mode, mode, 8);

            // Sattelites in use
            char* satinuse = strsep(&split_string, ",");
            satinuse = strlen(satinuse) == 0 ? (char*)"0" : satinuse;
            gps.satinuse = atoi(satinuse);

            // Hdop
            char* hdop = strsep(&split_string, ",");
            hdop = strlen(hdop) == 0 ? (char*)"9999" : hdop;
            gps.hdop = atoi(hdop);
            
            // Altitude
            char* altitude = strsep(&split_string, ",");
            altitude = strsep(&split_string, ",");
            altitude = (strlen(altitude) == 0) ? (char*)"0" : altitude;  
            gps.altitude = atoi(altitude);
        }
        split_string = strtok(NULL, "\r\n");
    }

    if(gps.kmh >= GPS_MIN_SPEED)
    { 
        UpdateToServer("moving");
        gps.moving = true;
    }
    else
    {
        if(gps.moving)
        {
            gps.moving = false;
            UpdateToServer("stopped");
        }
    }
}

void UpdateToServer(char* reason)
{
    if(gps.hdop == 9999)
        return;
        
    digitalWrite(PC13, LOW);
    
    char UrlData[200];
    char lat[12];
    char lon[12];
    char altitude[8];
    char hdop[8];
    
    dtostrf(gps.lat, 1, 6, lat);
    dtostrf(gps.lon, 1, 6, lon);
    dtostrf(gps.altitude, 1, 2, altitude);
    dtostrf(gps.hdop, 1, 1, hdop);
    
    sprintf(UrlData, "AT+HTTPPARA=\"URL\",\"http://190.111.213.103/gpsIN.php?lat=%s&lon=%s&sat=%d&alt=%s&sp=%d&hdop=%s&re=%s\"", lat, lon, gps.satinuse, altitude, gps.kmh, hdop, reason);
    SendCMD2(UrlData, 5000, "OK");
    SendCMD2("AT+HTTPACTION=0", 10000, "200");
    
    digitalWrite(PC13, HIGH);
    
    //CheckNewSMS();
}

float ConvertDegreeAngleToDouble(char* fixdata, char* dir)
{
    if(strlen(fixdata) == 0)
      return 0.0000;
      
    int idx = GetIndex(fixdata, ".");
    char deg[16] = { NULL };
    char mins[16] = { NULL };
    
    memcpy(&deg, fixdata, idx - 2);
    memcpy(&mins, (fixdata + idx) - 2, 9);
    
    int multi = 1;
    if(*dir == 87 || *dir == 83)
      multi = -1;

    float coord = (atoi(deg) + (atof(mins) / 60)) * multi;
    return coord;
}

int GetIndex(char* data, char* pattern)
{
    int result = 0;
    for(int i = 0; i < strlen(data); i++)
    {
        if(data[i] == 0x2E)
        {
            result = i;
            break;
        }
    }
    return result;
}
