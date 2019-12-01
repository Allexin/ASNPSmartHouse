#include "ASNPDevice_Router.h"
#include "utils.h"

void cDevice_Router::onRawMessageReceived(class cASNPLevel1Handler* sender, uint8_t* package)
{
  for (int i = 0; i<PORTS_COUNT; ++i){
    if (m_Ports[i]!=sender){
      m_Ports[i]->sendRawMessage(package);
    }
  }
}
void cDevice_Router::onMessageReceived(class cASNPLevel1Handler* sender, bool receiverIsGroup, uint8_t dataLength, uint8_t* data)
{
  //do nothing
}
void cDevice_Router::onEventReceived(class cASNPLevel1Handler* sender, uint8_t senderAddress, uint8_t dataLength, uint8_t* data)
{
  //do nothing
}
void cDevice_Router::onVariableChanged(class cASNPLevel1Handler* sender,sVariableInfo* variableInfo, void* variableValue)
{
  //do nothing
}


cDevice_Router::cDevice_Router():
  m_Port1("ROUTER","Device to routing 3 ports"),
  m_Port2("ROUTER","Device to routing 3 ports"),
  m_Port3("ROUTER","Device to routing 3 ports")
{
  m_Ports[0] = &m_Port1;
  m_Ports[1] = &m_Port2;
  m_Ports[2] = &m_Port3;
}

void cDevice_Router::start()
{
  m_Port1.setStream(&Serial1);
  m_Port2.setStream(&Serial2);
  m_Port3.setStream(&Serial3);
  for (int i = 0; i<PORTS_COUNT;++i)
  {
    m_Ports[i]->setEventsListener(this);
  }
}

bool cDevice_Router::update(long dt)
{
  bool actions = false;
  for (int i = 0; i<PORTS_COUNT; ++i){
    bool updated = m_Ports[i]->updateReceiving(dt);
    actions = actions | updated;
  }
  for (int i = 0; i<PORTS_COUNT; ++i){
    bool updated = m_Ports[i]->updateSending();
    actions = actions | updated;
  }
  return actions;
}

