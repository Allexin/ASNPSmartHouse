#include "ASNPDevice_SwitchController.h"
#include "ASNPUtils.h"

#include "SwitchDevice_3Button_Light.h"
#include "SwitchDevice_2Button_Gate.h"

static const uint8_t BUTTONS_PINS_COUNT = 9;
static const uint8_t BUTTONS_PINS[BUTTONS_PINS_COUNT] = {PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PB0};

void cDevice_SwitchController::reinitButtons()
{
  for (int i = 0; i<MAX_BUTTONS_COUNT; ++i)
  {
    delete m_Buttons[i];
    m_Buttons[i] = nullptr;
    switch(m_VarButtons[i].buttonType)
    {
      case 0:{
        //button disabled, do nothing
      };break;
      case 1:{
        //digital button
        if (m_VarButtons[i].buttinPinIndex<BUTTONS_PINS_COUNT)
        {
          m_Buttons[i] = new cButtonDigital(BUTTONS_PINS[m_VarButtons[i].buttinPinIndex]);
        }
        else
        {
          //invalid value: ignore button
        }
      };break;
      case 2:{
        //analog button
        //TODO 
      };break;
      default:
      {
        //invalid button type: ignore button
      }
    }
  }  
}

uint8_t parseHex(char hex)
{
  if (hex>='0' && hex<='9')
    return hex - '0';
  if (hex>='A' && hex<='F')
    return (hex - 'A')+0xA;
  if (hex>='a' && hex<='f')
    return (hex - 'a')+0xA;    
  return 0;
}

uint8_t parseAddress(char* addr3)
{
  return parseHex(addr3[0])*100+parseHex(addr3[1])*10+parseHex(addr3[2]);
}

void cDevice_SwitchController::reinitDevices()
{
  for (int i = 0; i<MAX_DEVICES_COUNT; ++i)
  {
    delete m_Devices[i];
    m_Devices[i] = nullptr;

    bool channels[CHANNELS_COUNT];
    for (int j = 0; j<CHANNELS_COUNT; ++j)
    {
      channels[j] = parseHex(m_VarDevices[i].channels[j])==1;
    }
    switch(m_VarDevices[i].deviceType)
    {
      case '0':{
        //device disabled, do nothing
      };break;
      case '1':{
        //cSwitchDevice_3Button_Light
        m_Devices[i] = new cSwitchDevice_3Button_Light(&m_Port, parseHex(m_VarDevices[i].buttons[0]),parseHex(m_VarDevices[i].buttons[1]),parseHex(m_VarDevices[i].buttons[2]), parseAddress(m_VarDevices[i].deviceAddress), channels);
      };break;
      case '2':{
        //cSwitchDevice_2Button_Gate
        m_Devices[i] = new cSwitchDevice_2Button_Gate(&m_Port, parseHex(m_VarDevices[i].buttons[0]),parseHex(m_VarDevices[i].buttons[1]),parseAddress(m_VarDevices[i].deviceAddress));
      };break;
      default:
      {
        //invalid device type: ignore device
      }
    }
  }  
}

void cDevice_SwitchController::onVariableChanged(class cASNPLevel1Handler* sender,sVariableInfo* variableInfo, void* variableValue)
{  
  reinitButtons();
  reinitDevices();
}


cDevice_SwitchController::cDevice_SwitchController():
  m_Port("SWITCH_CONTROLLER","Device to handle switches and control another devices")
{
  for (int i = 0; i<MAX_BUTTONS_COUNT; ++i)
  {
    m_Buttons[i] = nullptr;
  }
  for (int i = 0; i<MAX_DEVICES_COUNT; ++i)
  {
   m_Devices[i] = nullptr;
  }
}

void cDevice_SwitchController::start()
{
  //default variaables
  m_VarAddress = 0x01; //set default value for address
  m_Storage.regVariable(cUtils::StringToChar16(cASNPLevel1Handler::ADDRESS_VAR_NAME), eVariableType::VT_UINT8, 1, &m_VarAddress, true, true);
  
  memset(m_VarGroups,0,MAX_GROUPS_COUNT); //set default values for groups
  m_Storage.regVariable(cUtils::StringToChar16(cASNPLevel1Handler::GROUP_VAR_NAME),   eVariableType::VT_UINT8, MAX_GROUPS_COUNT, &m_VarGroups[0], true, true);

  //specific variables
  memset(m_VarButtons,0,sizeof(sButtonInfo)*MAX_BUTTONS_COUNT);
  m_VarButtons[0].buttonType = 1;
  m_VarButtons[0].buttinPinIndex = 0;
  m_VarButtons[1].buttonType = 1;
  m_VarButtons[1].buttinPinIndex = 1;
  m_VarButtons[2].buttonType = 1;
  m_VarButtons[2].buttinPinIndex = 2;
  m_VarButtons[3].buttonType = 1;
  m_VarButtons[3].buttinPinIndex = 3;
  m_VarButtons[4].buttonType = 1;
  m_VarButtons[4].buttinPinIndex = 4;
  m_VarButtons[5].buttonType = 1;
  m_VarButtons[5].buttinPinIndex = 5;
  m_VarButtons[6].buttonType = 1;
  m_VarButtons[6].buttinPinIndex = 6;
  m_VarButtons[7].buttonType = 1;
  m_VarButtons[7].buttinPinIndex = 7;
  m_VarButtons[8].buttonType = 1;
  m_VarButtons[8].buttinPinIndex = 8;
  m_Storage.regVariable(cUtils::StringToChar16("Buttons"), eVariableType::VT_UINT8VECTOR4, MAX_BUTTONS_COUNT, &m_VarButtons, true, true);

  memset(m_VarDevices,'0',sizeof(sDeviceInfo)*MAX_DEVICES_COUNT);
  m_Storage.regVariable(cUtils::StringToChar16("Devices"), eVariableType::VT_STRING16, MAX_DEVICES_COUNT, &m_VarDevices, true, true);
  
  Serial1.stm32SetRX(PA10);
  Serial1.stm32SetTX(PA9);
  Serial1.begin(cUtils::DEFAULT_BAUD_RATE); 

  m_Port.setStream(&Serial1);
  m_Port.setEventsListener(this);
  m_Port.setVariablesStorage(&m_Storage);

  reinitButtons();
  reinitDevices();

  m_TimeFromLastButtonsUpdate = 0;
}

bool cDevice_SwitchController::update(long dt)
{
  bool actions = false;
  bool updated = m_Port.updateReceiving(dt);
  actions = actions | updated;
  updated = m_Port.updateSending();
  actions = actions | updated;
  m_TimeFromLastButtonsUpdate+=dt;

  if (m_TimeFromLastButtonsUpdate>BUTTONS_UPDATE_TIME)
  {
    m_TimeFromLastButtonsUpdate = 0;
    for (int i = 0; i<MAX_BUTTONS_COUNT; ++i)
    {
      if (m_Buttons[i])
      {
        if (m_Buttons[i]->update())
        {
          /*if (m_Buttons[i]->isPressed())
          {
            char s[200];
            sprintf(s, "pressed %i", i);
            m_Port.sendLog(0x01,s);
          }*/
          for (int j = 0; j<MAX_DEVICES_COUNT; ++j)
          {
            if (m_Devices[j])
            {
              if (m_Buttons[i]->isPressed())
              {
                actions = actions | m_Devices[j]->buttonPressed(i);
              }
              else
              {
                actions = actions | m_Devices[j]->buttonReleased(i);
              }
            }
          }
        }
      }
    }    
  }
    
  return actions;
}
