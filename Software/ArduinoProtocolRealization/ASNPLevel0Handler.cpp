#include "ASNPLevel0Handler.h"

cASNPLevel0Handler::cASNPLevel0Handler()
{
  m_SendBufferSize = 0;
  m_ReceiveBufferSize = 0;
  m_ReceiverState = WAIT_START;

  memset(m_EnabledPrivateAddresses,0,256);
  memset(m_EnabledGroupAddresses,0,256);
  memset(m_EnabledPrivateEvents,0,256);
  m_EnabledPrivateAddresses[0] = true;
  m_EnabledGroupAddresses[0] = true;
  m_EnabledPrivateEvents[0] = true;
}

void cASNPLevel0Handler::setStream(SerialUART* serial)
{
  m_Stream = serial;
}

bool cASNPLevel0Handler::queuePackage(const uint8_t* data)
{
  uint8_t packageInfo = data[INFO_POS];
  uint8_t address = data[ADDRESS_POS];
  uint8_t dataLength = data[LENGTH_POS];

  bool highPriority = (packageInfo & FLAG_HIGH_PRIORITY) == FLAG_HIGH_PRIORITY;
  bool isGroupAddress = (packageInfo & FLAG_GROUP_ADDRESS) == FLAG_GROUP_ADDRESS;
  bool isEvent = (packageInfo & FLAG_EVENT) == FLAG_EVENT;
  bool canSend = address == 0x00;
  if (isEvent){
    canSend = canSend || (m_EnabledPrivateEvents[address]);
  }
  else{
    canSend = canSend || (isGroupAddress?m_EnabledGroupAddresses[address]:m_EnabledPrivateAddresses[address]);
  }
  long avaiableSize = (highPriority?SEND_BUFFER_SIZE:HALF_SEND_BUFFER_SIZE) - m_SendBufferSize;
  long packageLength = dataLength+PACKAGE_SERVICE_DATA_SIZE;
  canSend = canSend && (avaiableSize >= packageLength);
  if (!canSend)
    return false;

  memcpy(&m_SendBuffer[m_SendBufferSize],data,packageLength);
  m_SendBufferSize+=packageLength;  
  return true;
}
bool cASNPLevel0Handler::startSending()
{
  if (m_SendBufferSize==0)
    return false;

  long maxWriteCount = m_Stream->availableForWrite();
  if (maxWriteCount==0)
    return false;
    
  int dataToSend = m_SendBufferSize;
  if (dataToSend>maxWriteCount){
    dataToSend = maxWriteCount;
  }

  m_Stream->write(m_SendBuffer,dataToSend);
  if (dataToSend<m_SendBufferSize){
    memmove(m_SendBuffer,&m_SendBuffer[dataToSend],m_SendBufferSize-dataToSend);
    m_SendBufferSize -= dataToSend;
  }
  else{
    m_SendBufferSize = 0;
  }
  return true;
}

void cASNPLevel0Handler::receive(uint8_t data, bool clear)
{
  if (clear){
    m_ReceiveBufferSize = 0;
    m_Checksum = 0;
  }
  m_ReceiveBuffer[m_ReceiveBufferSize] = data;
  m_Checksum += data;
  if (m_ReceiveBufferSize<PACKAGE_BUFFER_SIZE-1)
    m_ReceiveBufferSize++;
}

uint8_t* cASNPLevel0Handler::processData(long dt)
{
  if (m_TimeFromLastData<PACKAGE_RESET_TIMEOUT){
    m_TimeFromLastData += dt;
    if (m_TimeFromLastData>=PACKAGE_RESET_TIMEOUT && m_ReceiverState!=WAIT_START){
      //timeout            
      m_ReceiverState=WAIT_START;
    }
  }
  
  while (m_Stream->available())
  {
    m_TimeFromLastData = 0;
    int c = m_Stream->read();    
    
    switch(m_ReceiverState){
      case WAIT_START:{
        //if (c==START_CHAR_LOW || c==START_CHAR_HIGH){
          receive(c,true);          
          m_ReceiverState = WAIT_ADDRESS;
       // }
      };break;
      case WAIT_ADDRESS:{
        receive(c,false);        
        m_ReceiverState = WAIT_LENGTH;
      };break;
      case WAIT_LENGTH:{
        if (c<=MAX_PACKAGE_DATA_SIZE){
          receive(c,false);          
          m_ReceiverState = WAIT_CHECKSUM;
        }
        else{
          //incorrect data, wait next package
          m_ReceiverState = WAIT_START;
        }
      };break;
      case WAIT_CHECKSUM:{
        uint8_t currentChecksum = m_Checksum;
        receive(c,false);
        if (m_ReceiveBuffer[LENGTH_POS]==m_ReceiveBufferSize-LENGTH_POS-2){        
          m_ReceiverState = WAIT_START;
          if (c==currentChecksum){      
            return m_ReceiveBuffer;  
          }
          else{
            //error - incorrect checksum
          }          
        }
      };break;
    }
  }
  return nullptr;
}

