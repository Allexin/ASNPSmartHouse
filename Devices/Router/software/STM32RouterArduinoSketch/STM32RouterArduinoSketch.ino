#include "stm32f1xx_hal_iwdg.h"
#include "ASNPDevice_Router.h"

//connect this pins to decrease Serial1 speed from default to 9600.
static const uint8_t PIN_POWER_FOR_TEST=PB12;
static const uint8_t PIN_TEST_SLOW_SPEED_PIN=PB13;

cDevice_Router Device;

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
  pinMode(PIN_POWER_FOR_TEST,OUTPUT);
  digitalWrite(PIN_POWER_FOR_TEST,HIGH);
  pinMode(PIN_TEST_SLOW_SPEED_PIN, INPUT_PULLDOWN);
  
  pinMode(LED_BUILTIN,OUTPUT);
  offLed();

  delay(100);
  onLed();
  delay(100);
  offLed();

  delay(1);

  Device.start();

  bool slowSpeed = digitalRead(PIN_TEST_SLOW_SPEED_PIN);    
  if (slowSpeed){
    Serial1.begin(9600);
  }
  digitalWrite(PIN_POWER_FOR_TEST,LOW);
  
  startWatchDog();  

  lastUpdate = millis();
}

void loop() {
  delay(100);
  onLed();
  delay(100);
  offLed();
  
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
    delay(10);
    offLed();
  }
}
