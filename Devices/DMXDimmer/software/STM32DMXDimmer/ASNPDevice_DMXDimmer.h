#ifndef ASNPDEVICE_DMXDIMMER
#define ASNPDEVICE_DMXDIMMER

#include "ASNPLevel1Handler.h"
#include "ASNPVariablesStorage.h"

class cDevice_DMXDimmer: public cASNPLevel1EventsListener
{
protected:
  virtual void onVariableChanged(class cASNPLevel1Handler* sender,sVariableInfo* variableInfo, void* variableValue) override;
protected:
  static const uint8_t MAX_GROUPS_COUNT = 5;
  static const uint8_t MAX_CHANNELS_COUNT = 16;
  static const uint8_t START_DIMMER_STATE = 0;
  uint8_t m_VarAddress;
  uint8_t m_Test1[MAX_GROUPS_COUNT];
  uint8_t m_VarGroups[MAX_GROUPS_COUNT];
  uint8_t m_Test2[MAX_GROUPS_COUNT];
  uint8_t m_VarChannelsCount;
  uint8_t m_Test3[MAX_GROUPS_COUNT];
  uint8_t m_VarChannelState[MAX_CHANNELS_COUNT];
  uint8_t m_Test4[MAX_GROUPS_COUNT];
  uint8_t m_VarChannelTarget[MAX_CHANNELS_COUNT];
  uint8_t m_Test5[MAX_GROUPS_COUNT];
  uint8_t m_VarChannelStepSqrt[MAX_CHANNELS_COUNT];
  
  long m_StepsAccum[MAX_CHANNELS_COUNT];
protected:
  cASNPVariablesStorage m_Storage;
  cASNPLevel1Handler m_Port;

public:
  cDevice_DMXDimmer();

  void start();
  bool update(long dt); //return tru if dimming in progress
};

#endif
