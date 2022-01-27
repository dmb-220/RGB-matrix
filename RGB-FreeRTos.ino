#include <Arduino.h>
#include <time.h>  
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "WiFi.h"
#include <HTTPClient.h>
#include <Wire.h>
#include "Adafruit_Si7021.h"
#include <Preferences.h>
#include <ezButton.h>
#include <FastLED.h>
//#include "LedGFX.h"
//LedGFX effects;
#include "RGB_FreeRTos.h"
#include "orai.h"
#include "ntp.h"

bool enableHeater = false;

MatrixPanel_I2S_DMA *dma_display = nullptr;
Adafruit_Si7021 sensor = Adafruit_Si7021();
Preferences pref;

ezButton buttonA(A_PIN);
ezButton buttonB(B_PIN);
ezButton buttonC(C_PIN);
ezButton buttonD(D_PIN);

void setup() {
  Serial.begin(115200);
  /************** DISPLAY **************/
  Serial.println("Starting...");
  Serial.print("Speed: (Mhz) ");
  Serial.println(ESP.getCpuFreqMHz());
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("WIFI connect ... ");
    Serial.println(ssid);
  }
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true); }
    
  //preference();
  //setup the effects generator
  //effects.Setup();
  
  //Paleidziam TASK'US
  xTaskCreate(TaskMatrix,  "TaskMatrix",  8192,  NULL,  5,  NULL);
  xTaskCreate(TaskORAI,  "TaskORAI",  2048,  NULL,  3,  NULL);
  xTaskCreate(TaskKEY,  "TaskKEY",  2048,  NULL,  4,  NULL);
  xTaskCreate(TaskTIME,  "TaskTIME",  2048,  NULL,  3,  NULL);
  xTaskCreate(TaskTEMP,  "TaskTEMP",  1024,  NULL,  7,  NULL);
}

void loop() {
   if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost!");
    WiFi.reconnect();
  }
}

/*void drawFrame() {
    for (int16_t x = 0; x < MATRIX_WIDTH; x++) {
            for (int y = 0; y < MATRIX_HEIGHT; y++) {
                int16_t v = 0;
                uint8_t wibble = sin8(ttime);
                v += sin16(x * wibble * 2 + ttime);
                v += cos16(y * (128 - wibble) * 2 + ttime);
                v += sin16(y * x * cos8(-ttime) / 2);

                effects.Pixel(x, y, (v >> 8) + 127);
            }
        }

        ttime += 1;
        cycles++;

        if (cycles >= 2048) {
            ttime = 0;
            cycles = 0;
        }
   
   effects.ShowFrame();
}*/

void TaskKEY(void *pvParameters) {
  buttonA.setDebounceTime(50);
  buttonB.setDebounceTime(50);
  buttonC.setDebounceTime(50);
  buttonD.setDebounceTime(50);
  
  for (;;) {
    buttonA.loop();
    buttonB.loop();
    buttonC.loop();
    buttonD.loop();

    button_key();
    
    vTaskDelay(50);
  }
}

void TaskTEMP(void *pvParameters) {
  sensor.begin();
  
  for (;;) {
    //Measure Relative Humidity from the HTU21D or Si7021
    hum = sensor.readHumidity();
    //Measure Temperature from the HTU21D or Si7021
    temp = sensor.readTemperature();

    Serial.print("Humidity:    ");
    Serial.print(sensor.readHumidity(), 2);
    Serial.print("\tTemperature: ");
    Serial.println(sensor.readTemperature(), 2);
    
    vTaskDelay(1000);
  }
}


void TaskTIME(void *pvParameters) {
  initTime("EET-2EEST,M3.5.0/3,M10.5.0/4");
  
  for (;;) {
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    Serial.println(&timeinfo);
    
    sekunde = timeinfo.tm_sec;
    minute = timeinfo.tm_min;
    valanda = timeinfo.tm_hour;

    vTaskDelay(500);
  }
}

void TaskORAI(void *pvParameters) {
  //Sudedam oru icon'as
  PopulateWeathers();
  for (;;) {
    orai(); 
    vTaskDelay(300*1000);
  }
}

void TaskMatrix(void *pvParameters) {
  //meniu
  int funkcija, set_laikas = 0;
  
  HUB75_I2S_CFG mxconfig = matrix_init(); 
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->setBrightness8(64);    // range is 0-255, 0 - 0%, 255 - 100%

  // Allocate memory and start DMA display
  if( not dma_display->begin() )
      Serial.println("I2S memory allocation failed");
      
  dma_display->clearScreen();
  
  for (;;) {    
    switch (funkcija) {
      case 1: break;
      //Bendras
      case 2:
        //Laikrodis
       laikrodis();
        //Saule teka, leidžiasi
        dma_display->setFont();
        dma_display->setTextColor(myCYAN, myBLACK);
        dma_display->drawBitmap(169, 40, sun_rise, 24, 24, myPILKA, myBLACK);
        dma_display->setCursor(197, 44);
        dma_display->print(NowTimeRise);
        dma_display->setCursor(197, 54);
        dma_display->print(NowTimeSet);
        dma_display->drawBitmap(230, 40, sun_set, 24, 24, myPILKA, myBLACK);

        namo_temp();

        //TEST vieta
        //dma_display->setFont();
        //dma_display->setTextColor(myBLUE, myBLACK);
        //dma_display->setCursor(80, 53);
        //dma_display->print(numb);

        //Rodom kokie orai
        if(WeatherID){
          int ID;
          if(Set_time > Sun_rise && Set_time < Sun_set){
            ID = WeatherID;
          }else{
            ID = WeatherID + 1000;
            }
          WeatherIdx = IndexOfWeatherID(ID);
        }

        //Nupieciam ICON'a, pagal orus
        dma_display->drawRGBBitmap(8, 0, Weathers[WeatherIdx].Gfx, 48, 48);
        dma_display->setFont();
        dma_display->setTextColor(myMAGENTA, myBLACK);
        //Lauko temperatura
        dma_display->setCursor(0, 49);
        dma_display->print("T:");
        if(current.temp >= 0){ dma_display->print(" "); }
        dma_display->print(current.temp);
        dma_display->drawCircle(50, 50, 2, myMAGENTA);
        dma_display->setCursor(54, 49);
        dma_display->print("C");
        //Vejo greitis
        dma_display->setCursor(0, 57);
        dma_display->print("W: ");
        dma_display->print(current.wind_speed);
        dma_display->print(" m/s ");
        break;
      //LAIKRODIS
      case 3:break;
      default:
        set_laikas++;
        //boot();
        dma_display->setFont();
        dma_display->setTextSize(1);
        dma_display->setTextColor(myRED, myBLACK);
        dma_display->setCursor(64, 32);
        dma_display->print("DMB-220 @2022");

        if(button != btn){
          if(btn == 1){ funkcija = 2; btn = 0; dma_display->clearScreen(); }
          }
        //if (set_laikas >= 50) {
          //set_laikas = 0; funkcija = 2;
          //dma_display->clearScreen();
        //}
        break;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  
}
