#include <ArduinoJson.h>
#include "Data_Point_Set.h"
#include "icons.h"

#define NUM_OF_WEATHERS 120

Currents current;

// structure to store weather descriptions and their ID's
struct Weather_Struct{
  uint16_t ID;
  const char* Line1;
  const char* Line2;
  const uint16_t* Gfx;
};

Weather_Struct Weathers[NUM_OF_WEATHERS];
uint8_t WeatherCount = 0; 

//Current weather data
//String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=TELŠIAI,LT&lang=lt&units=metric&APPID=2a0fdc441e007171bf18be3f9db4d7fd";
//One Call API
String OneCall = "http://api.openweathermap.org/data/2.5/onecall?lat=55.9814&lon=22.2472&exclude=minutely,alerts,hourly&appid=2a0fdc441e007171bf18be3f9db4d7fd&lang=lt&units=metric";

String jsonBuffer;

float main_temp, WindSpeed;
String WeatherDesc;
uint16_t WeatherMain, WeatherIcon;
uint16_t WeatherID = 0;
uint16_t WeatherIdx, time_zone;

time_t Sun_rise, Sun_set; 
time_t Set_time;


struct tm * DateTime;
char NowTimeRise[10], NowTimeSet[10];

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void orai(){
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      dma_display->drawRect(0, 0, 3, 3, myRED);
      
      JsonObject WeatherDoc;
      const size_t capacity = 25000;  // enough to handle daily and forecast
            
      jsonBuffer = httpGETRequest(OneCall.c_str());
      //Serial.println(jsonBuffer);
      DynamicJsonDocument  Doc(capacity);     
      DeserializationError error = deserializeJson(Doc, jsonBuffer);
      if (error) { Serial.println(error.c_str()); }

      WeatherDoc = Doc["current"]["weather"][0]; 
      
      WeatherID = WeatherDoc["id"];
      WeatherMain = WeatherDoc["main"];
      WeatherIcon = WeatherDoc["icon"];
      
      //main_temp = Doc["current"]["temp"];
      //WindSpeed = Doc["current"]["wind_speed"];
      current.temp = Doc["current"]["temp"];
      current.wind_speed = Doc["current"]["wind_speed"];
          
      //Laiko zona
      time_zone = Doc["timezone_offset"];

      Sun_rise = Doc["current"]["sunrise"];
      Sun_rise += time_zone;
      DateTime = gmtime(&Sun_rise);
      strftime (NowTimeRise,10,"%R",DateTime);
      
      Sun_set = Doc["current"]["sunset"];
      Sun_set += time_zone;
      DateTime = gmtime(&Sun_set);
      strftime (NowTimeSet,10,"%R",DateTime); 
      
      Set_time = Doc["current"]["dt"];
      Set_time += time_zone;
      DateTime = gmtime(&Set_time);

      dma_display->drawRect(0, 0, 3, 3, myBLACK);
      Doc.clear();
      jsonBuffer = "";
    }else {
      Serial.println("WiFi Disconnected");
    }  
}

//Sun-Rise
//24x24
const unsigned char sun_rise[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 
0x18, 0x00, 0x08, 0x18, 0x10, 0x1c, 0x00, 0x38, 0x0e, 0x00, 0x70, 0x04, 0x18, 0x20, 0x00, 0xff, 
0x00, 0x01, 0xe7, 0x80, 0x01, 0x81, 0x80, 0x01, 0x99, 0x80, 0xf3, 0x3c, 0xcf, 0xf3, 0x24, 0xcf, 
0x03, 0x81, 0xc0, 0x01, 0x81, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 
0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char sun_set[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 
0x18, 0x00, 0x08, 0x18, 0x10, 0x1c, 0x00, 0x38, 0x0e, 0x00, 0x70, 0x04, 0x18, 0x20, 0x00, 0xff, 
0x00, 0x01, 0xe7, 0x80, 0x01, 0x81, 0x80, 0x01, 0x81, 0x80, 0xf3, 0x24, 0xcf, 0xf3, 0x3c, 0xcf, 
0x03, 0x99, 0xc0, 0x01, 0x81, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 
0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

int16_t IndexOfWeatherID(uint16_t ID){
  uint8_t Idx=0;
  while(Idx < WeatherCount){
    if(Weathers[Idx].ID == ID){
      return Idx;
    }else{
      Idx++;
    }
  }
  // got this far, then not found
  return -1;
}

void AddCurrent(uint16_t ID,const char* FirstLine,const char* SecondLine, const uint16_t* Gfx){
  Weathers[WeatherCount].ID=ID;
  Weathers[WeatherCount].Line1=FirstLine;
  Weathers[WeatherCount].Line2=SecondLine;
  Weathers[WeatherCount].Gfx=Gfx;
  WeatherCount++;
}

void AddWeather(uint16_t ID,const char* FirstLine,const char* SecondLine, const uint16_t* Gfx){
  Weathers[WeatherCount].ID=ID;
  Weathers[WeatherCount].Line1=FirstLine;
  Weathers[WeatherCount].Line2=SecondLine;
  Weathers[WeatherCount].Gfx=Gfx;
  WeatherCount++;
}

void PopulateWeathers(){
  // Ids are for OpenWeathermap.org
  AddWeather(200,"Perkunija", "su lengvu lietumi", ico_30);
  AddWeather(201,"Perkunija", "su lietumi", ico_22);
  AddWeather(202,"Perkunija","su stipriu lietumi", ico_11);
  AddWeather(210,"Lengva perkunija","", ico_24d);
  AddWeather(211,"Perkunija","", ico_24d);
  AddWeather(212,"Heavy","Thunderstorm", ico_24d);
  AddWeather(221,"Thunderstorm","with ragged cloud", ico_24d);
  AddWeather(230,"Thunderstorm","with light drizzle", ico_33);
  AddWeather(231,"Thunderstorm","with drizzle", ico_33);
  AddWeather(232,"Thunderstorm","with heavy drizzle", ico_33);

  AddWeather(1200,"Perkunija", "su lengvu lietumi", ico_30);
  AddWeather(1201,"Perkunija", "su lietumi", ico_22);
  AddWeather(1202,"Perkunija","su stipriu lietumi", ico_11);
  AddWeather(1210,"Lengva perkunija","", ico_24n);
  AddWeather(1211,"Perkunija","", ico_24n);
  AddWeather(1212,"Heavy","Thunderstorm", ico_24n);
  AddWeather(1221,"Thunderstorm","with ragged cloud", ico_24n);
  AddWeather(1230,"Thunderstorm","with light drizzle", ico_33);
  AddWeather(1231,"Thunderstorm","with drizzle", ico_33);
  AddWeather(1232,"Thunderstorm","with heavy drizzle", ico_33);
  
  AddWeather(300,"Light drizzle","", ico_12);
  AddWeather(301,"Drizzle","", ico_12);
  AddWeather(302,"Heavy drizzle","", ico_12);
  AddWeather(310,"Light drizzle with","occasional rain", ico_48);
  AddWeather(311,"Drizzle with ","some rain", ico_48);
  AddWeather(312,"Heavy drizzle with","occasional rain",ico_48);
  AddWeather(313,"Occasional showers","of rain & drizzle", ico_48);
  AddWeather(314,"Heavy rain showers","and some drizzle",ico_12);
  AddWeather(321,"Showers of drizzle","",ico_12);
  
  AddWeather(1300,"Light drizzle","", ico_12);
  AddWeather(1301,"Drizzle","", ico_12);
  AddWeather(1302,"Heavy drizzle","", ico_12);
  AddWeather(1310,"Light drizzle with","occasional rain", ico_48);
  AddWeather(1311,"Drizzle with ","some rain", ico_48);
  AddWeather(1312,"Heavy drizzle with","occasional rain",ico_48);
  AddWeather(1313,"Occasional showers","of rain & drizzle", ico_48);
  AddWeather(1314,"Heavy rain showers","and some drizzle",ico_12);
  AddWeather(1321,"Showers of drizzle","",ico_12);
 
  AddWeather(500,"Light rain","Lengvas lietus", ico_46);
  AddWeather(501,"Moderate rain","Vidutinis lietus",ico_46);
  AddWeather(502,"Heavy rain","Smarkus lietus",ico_09);
  AddWeather(503,"Very heavy rain","Labai stiprus lietus",ico_09);
  AddWeather(504,"Extreme rain","Didelis lietus",ico_10);
  AddWeather(511,"Freezing rain","Saltas lietus",ico_10);
  AddWeather(520,"Light rain","showers",ico_46);
  AddWeather(521,"Rain showers","",ico_46);
  AddWeather(522,"Heavy rain showers","",ico_09);
  AddWeather(531,"Rain showers with","ragged cloud",ico_10);

  AddWeather(1500,"Light rain","Lengvas lietus", ico_46);
  AddWeather(1501,"Moderate rain","Vidutinis lietus",ico_46);
  AddWeather(1502,"Heavy rain","Smarkus lietus",ico_09);
  AddWeather(1503,"Very heavy rain","Labai stiprus lietus",ico_09);
  AddWeather(1504,"Extreme rain","Didelis lietus",ico_10);
  AddWeather(1511,"Freezing rain","Saltas lietus",ico_10);
  AddWeather(1520,"Light rain","showers",ico_46);
  AddWeather(1521,"Rain showers","",ico_46);
  AddWeather(1522,"Heavy rain showers","",ico_09);
  AddWeather(1531,"Rain showers with","ragged cloud",ico_10);

  AddWeather(600,"Light snow","",ico_44d);
  AddWeather(601,"Snow","",ico_44d);
  AddWeather(602,"Heavy snow","",ico_45d);
  AddWeather(611,"Sleet","",ico_43d);
  AddWeather(612,"Light sleet","showers",ico_43d);
  AddWeather(613,"Sleet showers","",ico_43d);
  AddWeather(615,"Light rain and","snow",ico_43d);
  AddWeather(616,"Rain and snow","",ico_43d);
  AddWeather(620,"Light snow showers","",ico_43d);
  AddWeather(621,"Snow showers","",ico_43d);
  AddWeather(622,"Heavy snow showers","",ico_43d);

  AddWeather(1600,"Light snow","",ico_44n);
  AddWeather(1601,"Snow","",ico_44n);
  AddWeather(1602,"Heavy snow","",ico_45n);
  AddWeather(1611,"Sleet","",ico_43n);
  AddWeather(1612,"Light sleet","showers",ico_43n);
  AddWeather(1613,"Sleet showers","",ico_43n);
  AddWeather(1615,"Light rain and","snow",ico_43n);
  AddWeather(1616,"Rain and snow","",ico_43n);
  AddWeather(1620,"Light snow showers","",ico_43n);
  AddWeather(1621,"Snow showers","",ico_43n);
  AddWeather(1622,"Heavy snow showers","",ico_43n);
  
  AddWeather(701,"Misty","Migla",ico_15);
  AddWeather(711,"Smoke","",ico_15);
  AddWeather(721,"Hazey","",ico_15);
  AddWeather(731,"Sand.dust whirls","",ico_15);
  AddWeather(741,"Fog","",ico_15);
  AddWeather(751,"Sand","",ico_15);
  AddWeather(761,"Dust","",ico_15);
  AddWeather(762,"Volcanic ash","",ico_15);
  AddWeather(771,"Squalls","",ico_15);
  AddWeather(781,"Tornado","",ico_15);

  AddWeather(1701,"Misty","Migla",ico_15);
  AddWeather(1711,"Smoke","",ico_15);
  AddWeather(1721,"Hazey","",ico_15);
  AddWeather(1731,"Sand.dust whirls","",ico_15);
  AddWeather(1741,"Fog","",ico_15);
  AddWeather(1751,"Sand","",ico_15);
  AddWeather(1761,"Dust","",ico_15);
  AddWeather(1762,"Volcanic ash","",ico_15);
  AddWeather(1771,"Squalls","",ico_15);
  AddWeather(1781,"Tornado","",ico_15);
  
  AddWeather(800,"Giedras dangus","", ico_01d);
  AddWeather(801,"A few clouds","Keletas debesu", ico_02d);
  AddWeather(802,"Scattered clouds","Padriki debesys", ico_03d);
  AddWeather(803,"Broken clouds but","Debesuota", ico_03d);
  AddWeather(804,"Very overcast","Labai apsiniauke", ico_03d); //04d

  AddWeather(1800,"Giedras dangus","", ico_01n);
  AddWeather(1801,"A few clouds","Keletas debesu", ico_02n);
  AddWeather(1802,"Scattered clouds","Padriki debesys", ico_03n);
  AddWeather(1803,"Broken clouds but","Debesuota", ico_03n);
  AddWeather(1804,"Very overcast","Labai apsiniauke", ico_03n); //04d
}

/***************************************************************************************
**                          Convert unix time to a time string
***************************************************************************************/
/*String strTime(time_t unixTime)
{
  unixTime += TIME_OFFSET;
  return ctime(&unixTime);
}*/
