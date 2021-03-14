#ifndef ASNPDEVICE_SWITCHCONTROLLER
#define ASNPDEVICE_SWITCHCONTROLLER

#include "ASNPLevel1Handler.h"
#include "ASNPVariablesStorage.h"

#include "SwitchDevice_Base.h"

struct __attribute__((packed)) sButtonInfo{
  uint8_t buttonType; //off(0)/digital(1)/analog(2)
  uint8_t buttinPinIndex; //inner index, not hardware index
  uint8_t args[2];  
};

struct __attribute__((packed)) sDeviceInfo{
  char deviceType; //off('0')/3Button_Light('1')/2Button_Gate('2')
  char deviceAddress[3]; //asnp address(decimal)
  char channels[8]; // true('1')/false('0')
  char buttons[4]; //button index(from buttons list) in HEX('0' - 'A')
};


class cButtonBase
{
protected:  
  bool m_Pressed;
public:  
  virtual bool update() = 0;
  virtual bool isPressed()
  {
    return m_Pressed;
  }
};

class cButtonDigital: public cButtonBase
{
protected:
  uint8_t m_Pin;
  bool m_Transition;
public:
  cButtonDigital(uint8_t pin):cButtonBase()
  ,m_Pin(pin)
  {
    pinMode(m_Pin, INPUT); 
    m_Transition = true;  
  }
  virtual bool update() override
  {
    if (digitalRead(m_Pin)==HIGH)
    {
      //unpressed
      if (m_Transition)
      {
        if (!m_Pressed)
        {
          return false; //key bounced, but not changed
        }
        else
        {
          m_Pressed = false; 
          return true; //key state changed  
        }
      }
      else
      {
        m_Transition = true;
        return false; //in transition, wait new state
      }
    }
    else
    {
      //pressed
      if (m_Transition)
      {
        if (m_Pressed)
        {
          return false; //key bounced, but not changed
        }
        else
        {
          m_Pressed = true; 
          return true; //key state changed  
        }
      }
      else
      {
        m_Transition = true;
        return false; //in transition, wait new state
      }
    }
  }
};

class cDevice_SwitchController: public cASNPLevel1EventsListener
{
protected:
  virtual void onVariableChanged(class cASNPLevel1Handler* sender,sVariableInfo* variableInfo, void* variableValue) override;

  void reinitButtons();
  void reinitDevices();
protected:
  static const uint8_t MAX_GROUPS_COUNT = 8;
    
  uint8_t m_VarAddress;
  uint8_t m_VarGroups[MAX_GROUPS_COUNT];

  static const uint8_t BUTTONS_UPDATE_TIME = 25;
  static const uint8_t MAX_DEVICES_COUNT = 8;

  
  sButtonInfo m_VarButtons[MAX_BUTTONS_COUNT]; //uint8vector4
  sDeviceInfo m_VarDevices[MAX_DEVICES_COUNT];

  cButtonBase* m_Buttons[MAX_BUTTONS_COUNT];
  cSwitchDeviceBase* m_Devices[MAX_DEVICES_COUNT];

  long m_TimeFromLastButtonsUpdate;
  /*
  uint8_t m_VarActive;

  long m_ActiveTime;

  bool m_ActiveHIGH;
  bool m_DirectionHIGH;
  long m_AntiSparkTimeout;*/
protected:
  cASNPVariablesStorage m_Storage;
  cASNPLevel1Handler m_Port;

public:
  cDevice_SwitchController();

  void start();
  bool update(long dt); //return true if active
};

#endif
