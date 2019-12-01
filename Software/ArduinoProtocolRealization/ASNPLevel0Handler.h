#ifndef ASNPLEVEL0HANDLER
#define ASNPLEVEL0HANDLER

#include <Arduino.h>

//its not trully level 0 handler, its also handle some level1 data from packages for routing purposes
class cASNPLevel0Handler{
public:
  static const long PACKAGE_RESET_TIMEOUT = 500; //ms
  static const long SEND_BUFFER_SIZE = 2048;
  static const long HALF_SEND_BUFFER_SIZE = SEND_BUFFER_SIZE / 2;

  static const long PACKAGE_HEADER_SIZE = 1+1+1; //PACKAGE_INFO + ADDRESS + DATA_SIZE
  static const long PACKAGE_SERVICE_DATA_SIZE = PACKAGE_HEADER_SIZE+1; // + CHECKSUM
  static const long MAX_PACKAGE_DATA_SIZE = 255;
  static const long PACKAGE_BUFFER_SIZE = MAX_PACKAGE_DATA_SIZE + PACKAGE_SERVICE_DATA_SIZE;

  static const uint8_t INFO_POS = 0;
  static const uint8_t ADDRESS_POS = 1;
  static const uint8_t LENGTH_POS = 2;
  static const uint8_t DATA_START_POS = 3;  
  
  enum eWaitState{
    WAIT_START,
    WAIT_ADDRESS,
    WAIT_LENGTH,
    WAIT_CHECKSUM
  };
protected:
  void receive(uint8_t data, bool clear);
public:
  static const uint8_t FLAG_PARITY = 1;
  static const uint8_t FLAG_HIGH_PRIORITY = 2;
  static const uint8_t FLAG_GROUP_ADDRESS = 4;
  static const uint8_t FLAG_EVENT = 8;
  static const uint8_t FLAG_4_UNUSED = 16;
  static const uint8_t FLAG_5_UNUSED = 32;
  static const uint8_t FLAG_6_UNUSED = 64;
  static const uint8_t FLAG_7_UNUSED = 128;
private:
  bool            m_EnabledPrivateAddresses[256];
  bool            m_EnabledGroupAddresses[256];
  bool            m_EnabledPrivateEvents[256];

  uint8_t         m_SendBuffer[SEND_BUFFER_SIZE];
  long            m_SendBufferSize;

  uint8_t         m_ReceiveBuffer[PACKAGE_BUFFER_SIZE];
  long            m_ReceiveBufferSize;

  uint8_t         m_Checksum;
  eWaitState      m_ReceiverState;

  SerialUART*     m_Stream;
    
  long            m_TimeFromLastData;
public:
  cASNPLevel0Handler();

  void setStream(SerialUART* serial);

  bool queuePackage(const uint8_t* data);
    
  bool startSending();

  uint8_t* processData(long dt);

  void enablePrivateAddressRouting(uint8_t addr){
    m_EnabledPrivateAddresses[addr] = true;
  }
  void enableGroupAddressRouting(uint8_t addr){
    m_EnabledGroupAddresses[addr] = true;
  }
  void enableEventsRouting(uint8_t addr){
    m_EnabledPrivateEvents[addr] = true;
  }
};

#endif //ASNPLEVEL0HANDLER
