#ifndef SWITCHDEVICE_2BUTTON_GATE
#define SWITCHDEVICE_2BUTTON_GATE

#include "SwitchDevice_Base.h"

class cSwitchDevice_2Button_Gate: public cSwitchDeviceBase
{
protected:
  static const unsigned long FORGET_TIME = 25000;
  uint8_t m_ButtonDown;
  uint8_t m_ButtonUp;
  uint8_t m_GateAddress;

  int8_t m_LastPressButton;
  long long m_LastPressTime;

  void moveGate(bool stop, bool up = true)
  {
    sVariableInfo viDirection;
    cUtils::StringToChar16("Direction", viDirection.name);
    viDirection.type = eVariableType::VT_UINT8;
    viDirection.slotIndex = 0;
    uint8_t direction = up?0:1;
    
    sVariableInfo viActive;
    cUtils::StringToChar16("Active", viActive.name);
    viActive.type = eVariableType::VT_UINT8;
    viActive.slotIndex = 0;
    uint8_t active = stop?0:1;
    
    if (!stop)
    {
      m_Port->sendSetVariable(m_GateAddress, &viDirection, &direction);
    }
    m_Port->sendSetVariable(m_GateAddress, &viActive, &active);
  }  
public:
  cSwitchDevice_2Button_Gate(cASNPLevel1Handler* port, uint8_t buttonDown,uint8_t buttonUp,uint8_t gateAddress):cSwitchDeviceBase(port)
  ,m_ButtonDown(buttonDown)
  ,m_ButtonUp(buttonUp)
  ,m_GateAddress(gateAddress)
  ,m_LastPressButton(-1)
  ,m_LastPressTime(0)
  {    
  }

  virtual bool buttonPressed(uint8_t button) override
  {
    cSwitchDeviceBase::buttonPressed(button);

    bool stopPressed = false;

    if (button==m_ButtonDown)
    {      
      if (button==m_LastPressButton &&millis()-m_LastPressTime< FORGET_TIME)
      {
        stopPressed = true;
      }
      else
      {
        m_LastPressButton = button;
        m_LastPressTime = millis();
        //down
        moveGate(false,false);
        return true;
      }
    }
    else
    if (button==m_ButtonUp)
    {
      if (button==m_LastPressButton &&millis()-m_LastPressTime< FORGET_TIME)
      {
        stopPressed = true;
      }
      else
      {
        m_LastPressButton = button;
        m_LastPressTime = millis();
        //up
        moveGate(false,true);
        return true;
      }
    }

    if (stopPressed)
    {
      m_LastPressTime = 0;
      m_LastPressButton = -1;
      //stop
      moveGate(true);
      return true;
    }
    return false;
  }
};

#endif
