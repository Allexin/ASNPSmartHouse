#include "ASNPDevice_DMXDimmer.h"
#include "_DMX.h"
#include "ASNPUtils.h"

void cDevice_DMXDimmer::onVariableChanged(class cASNPLevel1Handler* sender,sVariableInfo* variableInfo, void* variableValue)
{  
  if (m_VarChannelsCount>MAX_CHANNELS_COUNT)
  {
    m_VarChannelsCount = MAX_CHANNELS_COUNT;
  }
  for (int i = 0; i<MAX_CHANNELS_COUNT; ++i)
  {
    if (m_StepsAccum[i]>(m_VarChannelStepSqrt[i]*m_VarChannelStepSqrt[i]*2))
    {
      m_StepsAccum[i] = 0;
    }
  }
  
  //if channels count changed -reinit dimmer
  if (DMX.channelsCount()!=m_VarChannelsCount)
  {
    //this code can reset device. reason unknown. be careful
    DMX.end();
    DMX.begin(m_VarChannelsCount);
  }
}


cDevice_DMXDimmer::cDevice_DMXDimmer():
  m_Port("DMX_DIMMER","Device to control dimmers through DMX, max 16 channels")
{
}

void cDevice_DMXDimmer::start()
{
  m_VarAddress = 0x01; //set default value for address
  m_Storage.regVariable(cUtils::StringToChar16(cASNPLevel1Handler::ADDRESS_VAR_NAME), eVariableType::VT_UINT8, 1, &m_VarAddress, true, true);
  memset(m_VarGroups,0,MAX_GROUPS_COUNT); //set default values for groups
  m_Storage.regVariable(cUtils::StringToChar16(cASNPLevel1Handler::GROUP_VAR_NAME),   eVariableType::VT_UINT8, MAX_GROUPS_COUNT, &m_VarGroups[0], true, true);
  m_VarChannelsCount = 8; //set default channels count
  m_Storage.regVariable(cUtils::StringToChar16("ChannelsCount"), eVariableType::VT_UINT8, 1, &m_VarChannelsCount, true, true);
  memset(m_VarChannelState,START_DIMMER_STATE,MAX_CHANNELS_COUNT); //set initial dimmer state
  m_Storage.regVariable(cUtils::StringToChar16("ChannelState"), eVariableType::VT_UINT8, MAX_CHANNELS_COUNT, &m_VarChannelState[0], false, true);
  memset(m_VarChannelTarget,0,MAX_CHANNELS_COUNT); //set target dimmer state
  m_Storage.regVariable(cUtils::StringToChar16("ChannelTarget"), eVariableType::VT_UINT8, MAX_CHANNELS_COUNT, &m_VarChannelTarget[0], false, true);
  memset(m_VarChannelStepSqrt,2,MAX_CHANNELS_COUNT); // set dimming speed
  m_Storage.regVariable(cUtils::StringToChar16("ChannelStepSqrt"), eVariableType::VT_UINT8, MAX_CHANNELS_COUNT, &m_VarChannelStepSqrt[0], false, true);

  for (int i = 0; i<MAX_CHANNELS_COUNT; ++i){
    m_StepsAccum[i] = 0;
  }
  
  Serial1.stm32SetRX(PA10);
  Serial1.stm32SetTX(PA9);
  Serial1.begin(cUtils::DEFAULT_BAUD_RATE); 

  m_Port.setStream(&Serial1);
  m_Port.setEventsListener(this);
  m_Port.setVariablesStorage(&m_Storage);

  DMX.begin(m_VarChannelsCount);
}

bool cDevice_DMXDimmer::update(long dt)
{
  bool actions = false;
  bool updated = m_Port.updateReceiving(dt);
  actions = actions | updated;
  updated = m_Port.updateSending();
  actions = actions | updated;

  for (int i = 0; i<m_VarChannelsCount; ++i)
  {
    if (m_VarChannelState[i]!=m_VarChannelTarget[i])
    {
      long stepSize = m_VarChannelStepSqrt[i]*m_VarChannelStepSqrt[i];
      if (stepSize==0){
        m_VarChannelState[i]=m_VarChannelTarget[i];
      }
      else{
        m_StepsAccum[i]+=dt;
        while (m_VarChannelState[i]!=m_VarChannelTarget[i] && m_StepsAccum[i]>stepSize)
        {
          m_StepsAccum[i]-=stepSize;
          if (m_VarChannelState[i]<m_VarChannelTarget[i]){
            m_VarChannelState[i]++;
          }
          else{
            m_VarChannelState[i]--;
          }
          actions = true;
        }
      }      
    }
    else
    {
      m_StepsAccum[i] = 0;
    }
  }

  DMX.beginTransmission();
  for (int i = 0; i<m_VarChannelsCount; ++i){
    DMX.write(i+1,m_VarChannelState[i]);
  }
  DMX.endTransmission();
  
  
  return actions;
}
