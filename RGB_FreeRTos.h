#include <Fonts/FreeSerifBold24pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include "Pumpkin_Story32pt7b.h"
#include "Perpetrator24pt7b.h"
#include "Honey14pt7b.h"

//Myktukai
#define A_PIN  35
#define B_PIN  34
#define C_PIN  39
#define D_PIN  36

// Configure for your panel(s) as appropriate!
#define PANEL_WIDTH 64
#define PANEL_HEIGHT 64    // Panel height of 64 will required PIN_E to be defined.
#define PANELS_NUMBER 4   // Number of chained panels, if just a single panel, obviously set to 1

#define PIN_E 2
#define PIN_D 17
#define PIN_C 5
#define PIN_B 32
#define PIN_A 33
#define PIN_R1 25
#define PIN_G1 27
#define PIN_B1 26
#define PIN_R2 14
#define PIN_G2 13
#define PIN_B2 12

#define PANE_WIDTH PANEL_WIDTH * PANELS_NUMBER
#define PANE_HEIGHT PANEL_HEIGHT

extern MatrixPanel_I2S_DMA *dma_display;
//extern LedGFX effects;
extern Preferences pref;
extern ezButton buttonA;
extern ezButton buttonB;
extern ezButton buttonC;
extern ezButton buttonD;

//Wi-Fi
const char* ssid = "Telia-126300";
const char* password =  "kgbh1465yeAN";
//Laikas
struct tm timeinfo;
int sekunde, minute, valanda;
int laikas = 0;
int old_sekunde, old_minute, old_valanda;
int old_tens, old_unit = 0;
int button, btn;

int numb;

//int ttime = 0;
//int cycles = 0;

uint16_t colorWheel(uint8_t pos) {
  if(pos < 85) {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  } else if(pos < 170) {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  } else {
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}

unsigned long logMillis;
int count = 0;
uint16_t myColor = colorWheel(count);
float temp, hum, old_temp;

// Some standard colors RGB
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0,0,255);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myYELLOW = dma_display->color565(255, 255, 0);
uint16_t myCYAN = dma_display->color565(0, 255, 255);
uint16_t myMAGENTA = dma_display->color565(255, 0, 255);
uint16_t myDARK_MAGENTA = dma_display->color565(139,0,139);
uint16_t myBLACK = dma_display->color565(0, 0, 0);

uint16_t myPILKA = dma_display->color565(150, 150, 150);
uint16_t myZALIA = dma_display->color565(0, 200, 0);

HUB75_I2S_CFG matrix_init(){
  HUB75_I2S_CFG mxconfig;
  mxconfig.mx_height = PANEL_HEIGHT;
  mxconfig.mx_width = PANEL_WIDTH;
  mxconfig.chain_length = PANELS_NUMBER;
  mxconfig.gpio.e = PIN_E;
  mxconfig.gpio.d = PIN_D;
  mxconfig.gpio.c = PIN_C;
  mxconfig.gpio.b = PIN_B;
  mxconfig.gpio.a = PIN_A;
  mxconfig.gpio.r1 = PIN_R1;
  mxconfig.gpio.g1 = PIN_G1;
  mxconfig.gpio.b1 = PIN_B1;
  mxconfig.gpio.r2 = PIN_R2;
  mxconfig.gpio.g2 = PIN_G2;
  mxconfig.gpio.b2 = PIN_B2;
  mxconfig.clkphase = false;
  //mxconfig.double_buff = true; // Turn of double buffer

  return mxconfig;
}

//nustatymu paėmimas
void preference(){
  pref.begin("WebRadio", false);  // instance of preferences for defaults (station, volume ...)
  if(pref.getShort("volume", 1000) == 1000){ // if that: pref was never been initialized
      pref.putShort("volume", 10);
      pref.putShort("station", 0);
  }
  else{ // get the stored values
      //cur_station = pref.getShort("station");
      //cur_volume = pref.getShort("volume");
  }
}
void read_key(){
  //nuskaitoma ADC reiksme
unsigned int abutton=analogRead(A7);
  //tikrinama koks myktukas buvo nuspaustas
  if (abutton<20){button=1; btn=1; /*MENIU - ATGAL*/ }else
  if (abutton<200 && abutton>150){button=2; btn=2; /*Aukstyn - pliusas*/ }else
  if (abutton<420 && abutton>390){button=3; btn=3; /*Zemyn - minusas*/ }else{button=0;} 
}

void button_key() {
  //Set pardine reiksme,
  //ji nepasikeicia jei nenuspaustas myktukas
  button=0;
  if(buttonA.isReleased()){ button=1; btn=1; }
  if(buttonB.isReleased()){ button=2; btn=2; }
  if(buttonC.isReleased()){ button=3; btn=3; }
  if(buttonD.isReleased()){ button=4; btn=4; }
}

void namo_temp(){
  dma_display->setFont(&Honey14pt7b);
  if(temp != old_temp){
    dma_display->fillRect(85, 41, 80, 15, myBLACK);
    old_temp = temp; 
  }
  dma_display->setTextColor(myRED);
  dma_display->setCursor(103, 53);
  dma_display->print(temp);
  dma_display->drawCircle(152, 44, 3, myRED);
  dma_display->drawCircle(152, 44, 2, myRED);
  dma_display->setCursor(156, 53);
  dma_display->print("C");

  dma_display->setTextColor(myGREEN, myBLACK);
  dma_display->setFont();
  dma_display->setTextSize(1);
  dma_display->setCursor(103, 57);
  dma_display->print(hum);
  dma_display->print(" % ");
}

void laikrodis(){
  unsigned long LOGIKA_Millis = millis();

    if(LOGIKA_Millis - logMillis >= 500){
      count += 1;
      logMillis = LOGIKA_Millis;
      myColor = colorWheel(count);
      if(count > 255){count = 0;}
    }
    
  dma_display->drawLine(67, 0, 252, 0, myColor);
  dma_display->drawLine(67, 39, 252, 39, myColor);

  //dma_display->setFont(&FreeSerifBold24pt7b);
  //dma_display->setFont(&Pumpkin_Story32pt7b);
  dma_display->setFont(&Perpetrator24pt7b);
  dma_display->setTextColor(myColor);

  if(sekunde != old_sekunde){
    int tens = sekunde/10%10;       
    int unit = sekunde%10; 
    
    if(unit != old_unit){
      dma_display->fillRect(222, 1, 30, 38, myBLACK);
      old_unit = unit;
    }
    if(tens != old_tens){
      dma_display->fillRect(198, 1, 27, 38, myBLACK);
      old_tens = tens;
    }
    old_sekunde = sekunde;
  }
  if(minute != old_minute){
    dma_display->fillRect(132, 1, 54, 38, myBLACK);
    old_minute = minute;
  }
  if(valanda != old_valanda){
    dma_display->fillRect(67, 1, 54, 38, myBLACK);
    old_valanda = valanda;
  }
  
  dma_display->setCursor(64,35);
  dma_display->print(&timeinfo, "%H");
  dma_display->setCursor(130,35);
  dma_display->print(&timeinfo, "%M");
  dma_display->setCursor(196,35);
  dma_display->print(&timeinfo, "%S");

  //Tasku judejimas tarp skaiciu
  if(sekunde%2){
    dma_display->drawRect(123, 10, 7, 7, myBLACK);
    dma_display->drawRect(124, 11, 5, 5, myBLACK);
    dma_display->drawRect(123, 23, 7, 7, myBLACK);
    dma_display->drawRect(124, 24, 5, 5, myBLACK);

    dma_display->drawRect(189, 6, 7, 7, myBLACK);
    dma_display->drawRect(190, 7, 5, 5, myBLACK);
    dma_display->drawRect(189, 27, 7, 7, myBLACK);
    dma_display->drawRect(190, 28, 5, 5, myBLACK);
    
    dma_display->drawRect(123, 6, 7, 7, myColor);
    dma_display->drawRect(124, 7, 5, 5, myColor);
    dma_display->drawRect(123, 27, 7, 7, myColor);
    dma_display->drawRect(124, 28, 5, 5, myColor);
    
    dma_display->drawRect(189, 10, 7, 7, myColor);
    dma_display->drawRect(190, 11, 5, 5, myColor);
    dma_display->drawRect(189, 23, 7, 7, myColor);
    dma_display->drawRect(190, 24, 5, 5, myColor);
    }else{ 
    dma_display->drawRect(123, 6, 7, 7, myBLACK);
    dma_display->drawRect(124, 7, 5, 5, myBLACK);
    dma_display->drawRect(123, 27, 7, 7, myBLACK);
    dma_display->drawRect(124, 28, 5, 5, myBLACK);
    
    dma_display->drawRect(189, 10, 7, 7, myBLACK);
    dma_display->drawRect(190, 11, 5, 5, myBLACK);
    dma_display->drawRect(189, 23, 7, 7, myBLACK);
    dma_display->drawRect(190, 24, 5, 5, myBLACK);
    
    dma_display->drawRect(123, 10, 7, 7, myColor);
    dma_display->drawRect(124, 11, 5, 5, myColor);
    dma_display->drawRect(123, 23, 7, 7, myColor);
    dma_display->drawRect(124, 24, 5, 5, myColor);

    dma_display->drawRect(189, 6, 7, 7, myColor);
    dma_display->drawRect(190, 7, 5, 5, myColor);
    dma_display->drawRect(189, 27, 7, 7, myColor);
    dma_display->drawRect(190, 28, 5, 5, myColor);
  }
}
