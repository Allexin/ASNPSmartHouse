
#include "stm32f1xx_hal_iwdg.h"
#include "ASNPDevice_RelayGate.h"

cDevice_RelayGate Device;

IWDG_HandleTypeDef hiwdg;
void startWatchDog(){
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload    = 1000;
  
  HAL_IWDG_Init(&hiwdg);
}

void resetWatchDog(){
  HAL_IWDG_Refresh(&hiwdg);
}

void offLed(){
  digitalWrite(LED_BUILTIN,HIGH);
}

void onLed(){
  digitalWrite(LED_BUILTIN,LOW);
}

long long lastUpdate = 0;

void setup() {  
  pinMode(LED_BUILTIN,OUTPUT);
  offLed();
  Device.start();
  
  startWatchDog();  

  lastUpdate = millis();
}

void loop() {
  resetWatchDog();
  
  long long currentTime = millis();
  long dt = currentTime - lastUpdate;
  if (currentTime<lastUpdate)
    dt = currentTime;
  lastUpdate = currentTime;

  if (Device.update(dt))
  {
    onLed();
  }
  else
  {    
    delay(50);
    offLed();
  }
}
