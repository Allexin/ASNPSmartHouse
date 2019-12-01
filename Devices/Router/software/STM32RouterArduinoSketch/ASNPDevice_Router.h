#ifndef ASNPDEVICE_ROUTER
#define ASNPDEVICE_ROUTER

#include "ASNPLevel1Handler.h"

class cDevice_Router: public cASNPLevel1EventsListener
{
protected:
  virtual bool CheckWantAllMessages() override { return true; }

  virtual void onRawMessageReceived(class cASNPLevel1Handler* sender, uint8_t* package) override;
  virtual void onMessageReceived(class cASNPLevel1Handler* sender, bool receiverIsGroup, uint8_t dataLength, uint8_t* data) override;
  virtual void onEventReceived(class cASNPLevel1Handler* sender, uint8_t senderAddress, uint8_t dataLength, uint8_t* data) override;
  virtual void onVariableChanged(class cASNPLevel1Handler* sender,sVariableInfo* variableInfo, void* variableValue) override;
protected:
  static const uint8_t PORTS_COUNT = 3;  

  cASNPLevel1Handler m_Port1;
  cASNPLevel1Handler m_Port2;
  cASNPLevel1Handler m_Port3;
  cASNPLevel1Handler* m_Ports[PORTS_COUNT];

public:
  cDevice_Router();

  void start();
  bool update(long dt); //return true if data is received or transmited
};

#endif
