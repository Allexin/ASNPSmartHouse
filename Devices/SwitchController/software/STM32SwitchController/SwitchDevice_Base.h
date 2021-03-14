#ifndef SWITCHDEVICE_BASE
#define SWITCHDEVICE_BASE

static const uint8_t MAX_BUTTONS_COUNT = 16;
static const uint8_t INVALID_BUTTON = MAX_BUTTONS_COUNT+1;
static const uint8_t CHANNELS_COUNT = 8;

class cSwitchDeviceBase
{
protected:  
  bool buttonsStates[MAX_BUTTONS_COUNT];
  cASNPLevel1Handler* m_Port;
public:  
  cSwitchDeviceBase(cASNPLevel1Handler* port):
  m_Port(port)
  {
    for (int i = 0; i<MAX_BUTTONS_COUNT; ++i)
    {
      buttonsStates[i] = false;
    }
  }
  virtual void update(int dt){}
  virtual bool buttonPressed(uint8_t button)
  {
    if (button>=MAX_BUTTONS_COUNT)
    {
      return false;//invalid buttonindex
    }
    buttonsStates[button] = true;
    return false;
  }
  virtual bool buttonReleased(uint8_t button)
  {
    if (button>=MAX_BUTTONS_COUNT)
    {
      return false;//invalid buttonindex
    }
    buttonsStates[button] = false;
    return false;
  }

  bool isButtonPressed(uint8_t button)
  {
    if (button>=MAX_BUTTONS_COUNT)
    {
      return false;//invalid buttonindex
    }
    return buttonsStates[button];
  }
};

#endif
