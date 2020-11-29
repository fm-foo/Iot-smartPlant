#include <Wire.h>
#include "SH1106Wire.h"   // legacy: #include "SH1106.h"
#include "images.h"

SH1106Wire display(0x3c, 4, 5);     // ADDRESS, SDA, SCL

typedef void (*Demo)(void);
int demoMode = 0;

void DisplaySetup() {
  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}

void smileFrame0() {
    display.drawXbm(40, 10, width, height, smile0);
}
void smileFrame1() {
    display.drawXbm(40, 10, width, height, smile1);
}
void smileFrame2() {
    display.drawXbm(40, 10, width, height, smile2);
}
void smileFrame3() {
    display.drawXbm(40, 10, width, height, smile3);
}
void smileFrame4() {
    display.drawXbm(40, 10, width, height, smile4);
}
void smileFrame5() {
    display.drawXbm(40, 10, width, height, smile5);
}
void smileFrame6() {
    display.drawXbm(40, 10, width, height, smile6);
}
void smileFrame7() {
    display.drawXbm(40, 10, width, height, smile7);
}
void smileFrame8() {
    display.drawXbm(40, 10, width, height, smile8);
}
void smileFrame9() {
    display.drawXbm(40, 10, width, height, smile9);
}
void smileFrame10() {
    display.drawXbm(40, 10, width, height, smile10);
}
void smileFrame11() {
    display.drawXbm(40, 10, width, height, smile11);
}
void smileFrame12() {
    display.drawXbm(40, 10, width, height, smile12);
}
void smileFrame13() {
    display.drawXbm(40, 10, width, height, smile13);
}
void smileFrame14() {
    display.drawXbm(40, 10, width, height, smile14);
}
void smileFrame15() {
    display.drawXbm(40, 10, width, height, smile15);
}
void smileFrame16() {
    display.drawXbm(40, 10, width, height, smile16);
}
void smileFrame17() {
    display.drawXbm(40, 10, width, height, smile17);
}
void smileFrame18() {
    display.drawXbm(40, 10, width, height, smile18);
}
void smileFrame19() {
    display.drawXbm(40, 10, width, height, smile19);
}
void smileFrame20() {
    display.drawXbm(40, 10, width, height, smile20);
}
void smileFrame21() {
    display.drawXbm(40, 10, width, height, smile21);
}
void smileFrame22() {
    display.drawXbm(40, 10, width, height, smile22);
}
void smileFrame23() {
    display.drawXbm(40, 10, width, height, smile23);
}
void smileFrame24() {
    display.drawXbm(40, 10, width, height, smile24);
}
void smileFrame25() {
    display.drawXbm(40, 10, width, height, smile25);
}
void smileFrame26() {
    display.drawXbm(40, 10, width, height, smile26);
}
void smileFrame27() {
    display.drawXbm(40, 10, width, height, smile27);
}

Demo demos[] = {smileFrame0,
                smileFrame1,
                smileFrame2,
                smileFrame3,
                smileFrame4, 
                smileFrame5,
                smileFrame6, 
                smileFrame7, 
                smileFrame8, 
                smileFrame9, 
                smileFrame10,
                smileFrame11,
                smileFrame12,
                smileFrame13,
                smileFrame14,
                smileFrame15,
                smileFrame16,
                smileFrame17,
                smileFrame18,
                smileFrame19,
                smileFrame20,
                smileFrame21,
                smileFrame22,
                smileFrame23,
                smileFrame24,
                smileFrame25,
                smileFrame26,
                smileFrame27};
                
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;

void SoilProgressBar(int counter) {
  int progress = counter % 100;
  // draw the progress bar
  display.drawProgressBar(0, 38, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 5, "Soil Moisture");
  display.drawString(64, 21, String(progress) + "%");
}

void TemperatureProgressBar(int counter) {
  int progress = counter % 100;
  // draw the progress bar
  display.drawProgressBar(0, 38, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 5, "Temperature");
  display.drawString(64, 21, String(progress) + "*C");
}

void HumidityProgressBar(int counter) {
  int progress = counter % 100;
  // draw the progress bar
  display.drawProgressBar(0, 38, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 5, "Air Humidity");
  display.drawString(64, 21, String(progress) + "%");
}

unsigned long previousMillis = 0;
int animationSoilIndex = 0;

void animateHappy(long currentMillis) {
  if (currentMillis - previousMillis >= 60) {
    previousMillis = currentMillis;
    
    // clear the display
    display.clear();
    demos[demoMode]();
    display.display();
    demoMode = (demoMode + 1)  % demoLength;
  }
}

void animateSoilMoistureProgressBar(long currentMillis, int soilMoistureValue){
  if (currentMillis - previousMillis >= 20) {
    if(soilMoistureValue > animationSoilIndex){
      previousMillis = currentMillis;
      // clear the display
      display.clear();
      SoilProgressBar(animationSoilIndex);
      display.display();
      if(soilMoistureValue > animationSoilIndex){
        animationSoilIndex++;
      }
    }else if(soilMoistureValue < animationSoilIndex){
      previousMillis = currentMillis;
      // clear the display
      display.clear();
      SoilProgressBar(animationSoilIndex);
      display.display();
      if(soilMoistureValue < animationSoilIndex){
        animationSoilIndex--;
      }
    }else if(soilMoistureValue == animationSoilIndex){
      previousMillis = currentMillis;
      // clear the display
      display.clear();
      SoilProgressBar(animationSoilIndex);
      display.display();
    }
  }
}

void animateTeperatureProgressBar(long currentMillis, int temperatureValue){
  if (currentMillis - previousMillis >= 20) {
    if(temperatureValue > animationSoilIndex){
      previousMillis = currentMillis;
      // clear the display
      display.clear();
      TemperatureProgressBar(animationSoilIndex);
      display.display();
      if(temperatureValue > animationSoilIndex){
        animationSoilIndex++;
      }
    }else if(temperatureValue < animationSoilIndex){
      previousMillis = currentMillis;
      // clear the display
      display.clear();
      TemperatureProgressBar(animationSoilIndex);
      display.display();
      if(temperatureValue < animationSoilIndex){
        animationSoilIndex--;
      }
    }else if(temperatureValue == animationSoilIndex){
      previousMillis = currentMillis;
      // clear the display
      display.clear();
      TemperatureProgressBar(animationSoilIndex);
      display.display();
    }
  }
}

void animateHumidityProgressBar(long currentMillis, int humidityValue){
  if (currentMillis - previousMillis >= 20) {
    if(humidityValue > animationSoilIndex){
      previousMillis = currentMillis;
      // clear the display
      display.clear();
      HumidityProgressBar(animationSoilIndex);
      display.display();
      if(humidityValue > animationSoilIndex){
        animationSoilIndex++;
      }
    }else if(humidityValue < animationSoilIndex){
      previousMillis = currentMillis;
      // clear the display
      display.clear();
      HumidityProgressBar(animationSoilIndex);
      display.display();
      if(humidityValue < animationSoilIndex){
        animationSoilIndex--;
      }
    }else if(humidityValue == animationSoilIndex){
      previousMillis = currentMillis;
      // clear the display
      display.clear();
      HumidityProgressBar(animationSoilIndex);
      display.display();
    }
  }
}
