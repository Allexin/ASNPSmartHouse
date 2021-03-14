#ifndef ASNPLEVEL1HANDLER
#define ASNPLEVEL1HANDLER

#include <Arduino.h>
#include "ASNPVariablesStorage.h"
#include "ASNPLevel0Handler.h"

class cASNPLevel1EventsListener{
public:
  virtual bool CheckWantAllMessages() { return false; } //override this method if you want to intercept messages for another devices. remember - even if you override this method and return true - you get only message for addresses setted in pong messages
  
  virtual void onRawMessageReceived(class cASNPLevel1Handler* sender, uint8_t* package){}
  virtual void onMessageReceived(class cASNPLevel1Handler* sender, uint8_t dataLength, uint8_t* data){}
  virtual void onEventReceived(class cASNPLevel1Handler* sender, uint8_t senderAddress, uint8_t dataLength, uint8_t* data){}
  virtual void onVariableChanged(class cASNPLevel1Handler* sender,sVariableInfo* variableInfo, void* variableValue){}
};

class cASNPLevel1Handler: public cASNPVariablesStorageListener{
protected:
  virtual void onVariableChanged(sVariableInfo* variableInfo, void* variableValue, bool subscribed) override;
public:
  static const uint8_t CMD_PING = 0x00;
  static const uint8_t CMD_PONG = 0x01;
  static const uint8_t CMD_REQUEST_INFO = 0x20;
  static const uint8_t CMD_RESPONSE_INFO = 0x21;
  static const uint8_t CMD_GET_VARIABLES_COUNT = 0x22;
  static const uint8_t CMD_RESPONSE_VARIABLES_COUNT = 0x23;
  static const uint8_t CMD_GET_VARIABLES = 0x24;
  static const uint8_t CMD_RESPONSE_VARIABLES = 0x25;
  static const uint8_t CMD_GET_VARIABLE = 0x26;
  static const uint8_t CMD_RESPONSE_VARIABLE = 0x27;
  static const uint8_t CMD_SET_VARIABLE = 0x28;
  static const uint8_t CMD_SUBSCRIBE_TO_VARIABLE = 0x29;
  static const uint8_t CMD_EVENT_VARIABLE_CHANGED = 0x2A;
  
  static const uint8_t CMD_LOG = 0xFF;

  static const String ADDRESS_VAR_NAME;
  static const String GROUP_VAR_NAME;
  
protected:
  uint8_t* m_selfPersonalAddress;
  uint8_t m_selfPersonalAddressCount;
  uint8_t* m_selfGroupAddress;
  uint8_t m_selfGroupAddressCount;

  uint8_t m_EventsSubsribedAddresses[256]; //fill it when this device send message SUBSCRIBE_TO_VARIABLE
  

  cASNPLevel0Handler m_Level0Handler;
  cASNPLevel1EventsListener* m_Listener;
  cASNPVariablesStorage* m_Storage;

  bool            m_FirstTick;

  String m_DeviceType;
  String m_DeviceDescription;

  long m_SendPingCounter;  
protected:
  void parsePong(uint8_t* data, uint8_t dataLength);
  void parseRequestInfo(uint8_t* data, uint8_t dataLength);
  void parseGetVariablesCount(uint8_t* data, uint8_t dataLength);
  void parseGetVariables(uint8_t* data, uint8_t dataLength);
  void parseGetVariable(uint8_t* data, uint8_t dataLength);
  void parseSetVariable(uint8_t* data, uint8_t dataLength);
  void parseSubscribeToVariable(uint8_t* data, uint8_t dataLength);

  uint8_t readuint8(uint8_t* data, uint8_t& pos,int dataLength);
  sVariableInfo readVariableInfo(uint8_t* data, uint8_t& pos,int dataLength);
public:
  cASNPLevel1Handler(String deviceType, String deviceDescription);
  void setStream(SerialUART* serial);
  void setVariablesStorage(cASNPVariablesStorage* storage);
  void setEventsListener(cASNPLevel1EventsListener* listener); //only one allowed

  bool sendRawMessage(uint8_t* package);

  bool updateReceiving(long dt);
  bool updateSending();

  void queuePong();//send pong with proper delay
public:
//this messages automaticaly sended by level 1 handler, do not call it directly if you do not understand what are you doing!
  void sendPing();
  void sendPong(); //send pong Immediately
  void sendResponseInfo(uint8_t responseAddress);
  void sendResponseVariablesCount(uint8_t responseAddress);
  void sendResponseVariables(uint8_t responseAddress, uint8_t startIndex);
  void sendResponseVariable(uint8_t responseAddress, sVariableInfo* info, void* variableValue);
  void sendVariableChangedEvent(sVariableInfo* info, void* variableValue);

//this messages can be called externally when needed
  void sendRequestInfo(uint8_t targetAddress);
  void sendGetVariablesCount(uint8_t targetAddress);
  void sendGetVariables(uint8_t targetAddress, uint8_t startIndex);
  void sendGetVariable(uint8_t targetAddress, const sVariableInfo* info);
  void sendSetVariable(uint8_t targetAddress, const sVariableInfo* info, const void* variableValue);
  void sendLog(uint8_t targetAddress, const char* msg);
};


#endif //ASNPLEVEL1HANDLER
