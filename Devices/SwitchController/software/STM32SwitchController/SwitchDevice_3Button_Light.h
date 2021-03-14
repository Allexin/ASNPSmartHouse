#ifndef SWITCHDEVICE_3BUTTON_LIGHT
#define SWITCHDEVICE_3BUTTON_LIGHT

#include "SwitchDevice_Base.h"

class cSwitchDevice_3Button_Light: public cSwitchDeviceBase
{
protected:
  static const uint8_t DIMM_STEP = 16;
  static const uint8_t FAST_DIMMING = 1;
  static const uint8_t SLOW_DIMMING = 4;

  uint8_t m_ButtonOff;
  uint8_t m_ButtonDimmer;
  uint8_t m_ButtonOn;
  uint8_t m_DimmerAddress;
  uint8_t m_CurrentState;
  bool m_Channels[CHANNELS_COUNT];

  void sendNewLightState(bool fast)
  {
    sVariableInfo viSpeed;
    cUtils::StringToChar16("ChannelStepSqrt", viSpeed.name);
    viSpeed.type = eVariableType::VT_UINT8;
    sVariableInfo viValue;
    cUtils::StringToChar16("ChannelTarget", viValue.name);
    viValue.type = eVariableType::VT_UINT8;

    for (int i = 0; i<CHANNELS_COUNT; ++i)
    {
      if (m_Channels[i])
      {
        viSpeed.slotIndex = i;
        uint8_t speed = fast?FAST_DIMMING:SLOW_DIMMING;
        m_Port->sendSetVariable(m_DimmerAddress, &viSpeed, &speed);
        viValue.slotIndex = i;
        m_Port->sendSetVariable(m_DimmerAddress, &viValue, &m_CurrentState);
      }
    }
  }
public:
  cSwitchDevice_3Button_Light(cASNPLevel1Handler* port, uint8_t buttonOff,uint8_t buttonDimmer,uint8_t buttonOn, uint8_t dimmerAddress, bool channels[CHANNELS_COUNT]):cSwitchDeviceBase(port)
  ,m_ButtonOff(buttonOff)
  ,m_ButtonDimmer(buttonDimmer)
  ,m_ButtonOn(buttonOn)
  ,m_DimmerAddress(dimmerAddress)
  ,m_CurrentState(0)
  {    
    memcpy(m_Channels, channels, CHANNELS_COUNT);
  }

  virtual bool buttonPressed(uint8_t button) override
  {
    cSwitchDeviceBase::buttonPressed(button);

    if (button==m_ButtonOff)
    {
      if (isButtonPressed(m_ButtonDimmer) && m_CurrentState-DIMM_STEP > 0)
      {
        //dimming <
        m_CurrentState-=DIMM_STEP;
        sendNewLightState(true);
        return true;
      }
      else
      {
        m_CurrentState = 0;
        //off
        sendNewLightState(false);
        return true;
      }
    }
    else
    if (button==m_ButtonOn)
    {
      if (isButtonPressed(m_ButtonDimmer) && m_CurrentState+DIMM_STEP < 255)
      {
        m_CurrentState+=DIMM_STEP;
        //dimming >
        sendNewLightState(true);
        return true;
      }
      else
      {
        m_CurrentState = 200;
        //on
        sendNewLightState(false);
        return true;
      }
    }
    return false;
  }
};

#endif
