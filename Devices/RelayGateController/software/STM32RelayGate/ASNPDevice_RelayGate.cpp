#include "ASNPDevice_RelayGate.h"
#include "ASNPUtils.h"

static const uint8_t PIN_ACTIVE = PB5;
static const uint8_t PIN_DIRECTION = PB4;

void cDevice_RelayGate::onVariableChanged(class cASNPLevel1Handler* sender,sVariableInfo* variableInfo, void* variableValue)
{  
  if (m_ActiveTime==0 && m_VarActive>0)
  {
    m_ActiveTime = m_VarCycleTime*1000;
  }
  else
  {
    if (m_ActiveTime>0 && m_VarActive==0)
    {
      m_ActiveTime = 0;
    }
  }
}


cDevice_RelayGate::cDevice_RelayGate():
  m_Port("RELAY_GATE","Device to control AC route and activity")
{
  cUtils::StringToChar16("Active",m_VarActiveName);  
  m_ActiveTime = 0;
  m_AntiSparkTimeout = 0;
  m_ActiveHIGH = false;
  m_DirectionHIGH = false;
}

void cDevice_RelayGate::start()
{
  m_VarAddress = 0x01; //set default value for address
  m_Storage.regVariable(cUtils::StringToChar16(cASNPLevel1Handler::ADDRESS_VAR_NAME), eVariableType::VT_UINT8, 1, &m_VarAddress, true, true);
  
  memset(m_VarGroups,0,MAX_GROUPS_COUNT); //set default values for groups
  m_Storage.regVariable(cUtils::StringToChar16(cASNPLevel1Handler::GROUP_VAR_NAME),   eVariableType::VT_UINT8, MAX_GROUPS_COUNT, &m_VarGroups[0], true, true);
  
  m_VarCycleTime = 25;
  m_Storage.regVariable(cUtils::StringToChar16("CycleTime"), eVariableType::VT_UINT8, 1, &m_VarCycleTime, true, true);
  
  m_VarDirection = 0;
  m_Storage.regVariable(cUtils::StringToChar16("Direction"), eVariableType::VT_UINT8, 1, &m_VarDirection, false, true);
  
  m_VarActive = 0;//current state of
  m_Storage.regVariable(m_VarActiveName, eVariableType::VT_UINT8, 1, &m_VarActive, false, true);
  
  Serial1.stm32SetRX(PA10);
  Serial1.stm32SetTX(PA9);
  Serial1.begin(cUtils::DEFAULT_BAUD_RATE); 

  m_Port.setStream(&Serial1);
  m_Port.setEventsListener(this);
  m_Port.setVariablesStorage(&m_Storage);

  pinMode(PIN_ACTIVE, OUTPUT);
  pinMode(PIN_DIRECTION,OUTPUT);
}

bool cDevice_RelayGate::update(long dt)
{
  bool actions = false;
  bool updated = m_Port.updateReceiving(dt);
  actions = actions | updated;
  updated = m_Port.updateSending();
  actions = actions | updated;

  if (m_ActiveTime>0)
  {
    m_ActiveTime-=dt;
    if (m_ActiveTime<0)
    {
      m_ActiveTime = 0;
    }
  }
  if (m_ActiveTime==0)
  {
    m_VarActive = 0;
  }

  if (m_AntiSparkTimeout>0){
    m_AntiSparkTimeout-=dt;
  }
  else{
    bool directionHigh = (m_VarDirection && m_VarActive);
    bool activeHigh = m_VarActive;

    if (directionHigh!=m_DirectionHIGH){
      m_DirectionHIGH = directionHigh;
      m_AntiSparkTimeout = ANTI_SPARK_TIME;
      digitalWrite(PIN_DIRECTION, directionHigh?HIGH:LOW);
    }
    else{
      if (activeHigh!=m_ActiveHIGH){
        m_ActiveHIGH = activeHigh;
        m_AntiSparkTimeout = ANTI_SPARK_TIME;
        digitalWrite(PIN_ACTIVE, activeHigh?HIGH:LOW);
      }
    }
  }
  
  return actions | m_ActiveTime>0;
}
