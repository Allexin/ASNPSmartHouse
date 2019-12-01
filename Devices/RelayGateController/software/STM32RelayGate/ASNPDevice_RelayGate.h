#ifndef ASNPDEVICE_RELAYGATE
#define ASNPDEVICE_RELAYGATE

#include "ASNPLevel1Handler.h"
#include "ASNPVariablesStorage.h"

class cDevice_RelayGate: public cASNPLevel1EventsListener
{
protected:
  virtual void onVariableChanged(class cASNPLevel1Handler* sender,sVariableInfo* variableInfo, void* variableValue) override;
protected:
  static const uint8_t MAX_GROUPS_COUNT = 8;
  static const uint8_t ANTI_SPARK_TIME = 100;

  char m_VarActiveName[16];
  
  uint8_t m_VarAddress;
  uint8_t m_VarGroups[MAX_GROUPS_COUNT];
  uint8_t m_VarCycleTime;
  uint8_t m_VarDirection;
  uint8_t m_VarActive;

  long m_ActiveTime;

  bool m_ActiveHIGH;
  bool m_DirectionHIGH;
  long m_AntiSparkTimeout;
protected:
  cASNPVariablesStorage m_Storage;
  cASNPLevel1Handler m_Port;

public:
  cDevice_RelayGate();

  void start();
  bool update(long dt); //return true if active
};

#endif
