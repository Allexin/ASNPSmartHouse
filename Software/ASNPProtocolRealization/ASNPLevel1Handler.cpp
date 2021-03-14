#include "ASNPLevel1Handler.h"
#include "ASNPUtils.h"

const String cASNPLevel1Handler::ADDRESS_VAR_NAME = "ADDRESS";
const String cASNPLevel1Handler::GROUP_VAR_NAME = "GROUP";

cASNPLevel1Handler::cASNPLevel1Handler(String deviceType, String deviceDescription)
{
  m_DeviceType = deviceType;
  m_DeviceDescription = deviceDescription;
  m_Listener = nullptr;
  m_Storage = nullptr;
  m_selfPersonalAddress = nullptr;
  m_selfPersonalAddressCount = 0;
  m_selfGroupAddress = nullptr;
  m_selfGroupAddressCount = 0;
  m_SendPingCounter = 0;
  memset(m_EventsSubsribedAddresses,0,256);
  m_FirstTick = true;
}

void cASNPLevel1Handler::setStream(SerialUART* serial)
{
  m_Level0Handler.setStream(serial);
}
void cASNPLevel1Handler::setVariablesStorage(cASNPVariablesStorage* storage)
{
  m_selfPersonalAddress = nullptr;
  m_selfPersonalAddressCount = 0;
  m_selfGroupAddress = nullptr;
  m_selfGroupAddressCount = 0;
  m_Storage = storage;
  if (!m_Storage){
    m_SendPingCounter = 0;
    return;
  }

  m_Storage->addListener(this);

  if(const cASNPVariablesStorage::sVariable* var = m_Storage->getVariable(cUtils::StringToChar16(ADDRESS_VAR_NAME), eVariableType::VT_UINT8)){
    m_selfPersonalAddress = (uint8_t*)var->value;
    m_selfPersonalAddressCount = var->info.slotsCount;
  }
  if(const cASNPVariablesStorage::sVariable* var = m_Storage->getVariable(cUtils::StringToChar16(GROUP_VAR_NAME), eVariableType::VT_UINT8)){
    m_selfGroupAddress = (uint8_t*)var->value;
    m_selfGroupAddressCount = var->info.slotsCount;
  }
  
  queuePong();
}
void cASNPLevel1Handler::setEventsListener(cASNPLevel1EventsListener* listener)
{
  m_Listener = listener;
}

void cASNPLevel1Handler::onVariableChanged(sVariableInfo* variableInfo, void* variableValue, bool subscribed)
{
  if (subscribed){
    sendVariableChangedEvent(variableInfo,variableValue);
  }

  bool addressChanged = false;
  if (memcmp(variableInfo->name,cUtils::StringToChar16(ADDRESS_VAR_NAME),16)==0)
      addressChanged = true;
  if (memcmp(variableInfo->name,cUtils::StringToChar16(GROUP_VAR_NAME),16)==0)
      addressChanged = true;
  if (addressChanged)
  {
    queuePong();
  }
  
  if (m_Listener)
  {
    m_Listener->onVariableChanged(this,variableInfo, variableValue);
  }
}

bool cASNPLevel1Handler::sendRawMessage(uint8_t* package)
{
  return m_Level0Handler.queuePackage(package);
}

bool cASNPLevel1Handler::updateReceiving(long dt)
{
  bool doneSomething = false;
  if (m_FirstTick){
    sendPing();
    m_FirstTick = false;
    doneSomething = true;
  }
  
  if (m_SendPingCounter>0){
    m_SendPingCounter-=dt;
    if (m_SendPingCounter<=0){
      m_SendPingCounter = 0;
      sendPong();
      doneSomething = true;
    }
  }
  
  while (uint8_t* package =  m_Level0Handler.processData(dt))
  {
    uint8_t info = package[cASNPLevel0Handler::INFO_POS];
    uint8_t address = package[cASNPLevel0Handler::ADDRESS_POS];
    uint8_t dataLength = package[cASNPLevel0Handler::LENGTH_POS];
    uint8_t* data = &package[cASNPLevel0Handler::DATA_START_POS];
    bool highPriority = (info & cASNPLevel0Handler::FLAG_HIGH_PRIORITY) == cASNPLevel0Handler::FLAG_HIGH_PRIORITY;
    bool isEvent = (info & cASNPLevel0Handler::FLAG_EVENT) == cASNPLevel0Handler::FLAG_EVENT;
    bool isBroadcast = address == 0x00;

    if (dataLength<3){
      continue; //incorrect package - ignore
    }

    bool isSysCommand = data[0]==0x00;
    uint8_t command = data[1];
    uint8_t version = data[2];

    bool isToMe = false;    
    if (isBroadcast)
    {
      isToMe = m_selfPersonalAddress || m_selfGroupAddress;
    }
    else{
      if (isEvent){
        isToMe = m_selfPersonalAddress || m_selfGroupAddress;
      }
      else{
        if (*m_selfPersonalAddress==address){
          isToMe = true;
        }
        else{
		  for (int i = 0; i<m_selfGroupAddressCount; ++i)
		  {
			  if (m_selfGroupAddress[i] == address)
			  {
				  isToMe = true;
			  }
		  }
        }
      }
    }

    if (isSysCommand){
      switch (command){
        case CMD_PING:{
          queuePong();
        };break;
        case CMD_PONG:{
          //set routes
          parsePong(data, dataLength);          
        };break;
        case CMD_REQUEST_INFO:{
          if (isToMe){
            //send description
            parseRequestInfo(data, dataLength);
          }
        };break;
        case CMD_RESPONSE_INFO:{
          //do nothing
        };break;
        case CMD_GET_VARIABLES_COUNT:{
          if (isToMe){
            //return variables count
            parseGetVariablesCount(data, dataLength);
          }
        };break;
        case CMD_RESPONSE_VARIABLES_COUNT:{
          //do nothing
        };break;
        case CMD_GET_VARIABLES:{
          if (isToMe){
            //return variables
            parseGetVariables(data, dataLength);
          }
        };break;
        case CMD_RESPONSE_VARIABLES:{
          //do nothing
        };break;
        case CMD_GET_VARIABLE:{
          if (isToMe){
            //return variable
            parseGetVariable(data, dataLength);
          }
        };break;
        case CMD_RESPONSE_VARIABLE:{
          //do nothing
        };break;
        case CMD_SET_VARIABLE:{
          if (isToMe){
            //set variable and call onVariableChanged
            parseSetVariable(data, dataLength);
          }
        };break;
        case CMD_SUBSCRIBE_TO_VARIABLE:{
          if (isToMe){
            //set route and variable listener
            parseSubscribeToVariable(data, dataLength);
          }
        };break;
        case CMD_EVENT_VARIABLE_CHANGED:{
          //do nothing
        };break;
      }      
    };

    if (m_Listener){      
      if (isToMe || m_Listener->CheckWantAllMessages()){
        m_Listener->onRawMessageReceived(this, package);
        if (isEvent)
        {
          m_Listener->onEventReceived(this, address, dataLength, data);
        }
        else
        {  
          m_Listener->onMessageReceived(this, dataLength, data);
        }
      }
    }
    doneSomething = true;
  };
  return doneSomething;
}

bool cASNPLevel1Handler::updateSending()
{
  return m_Level0Handler.startSending();
}

void cASNPLevel1Handler::queuePong()
{
  if (m_selfPersonalAddress){
    m_SendPingCounter = *m_selfPersonalAddress*20; //send with delay
    return;
  }
  if (m_selfGroupAddress){
    m_SendPingCounter = *m_selfGroupAddress*20; //we have not personal address, so send with delay from first group address
    return;
  }
  m_SendPingCounter = 0; //we cant send pong because have not any addresses
}

void cASNPLevel1Handler::sendPing()
{
                      //highPriority,  event, address, cmdType, cmd,           dataVersion
  cUtils::writeStartToTempBuffer(true, false,   0x00,       0x00,    CMD_PING,      0x01);
  //fill data
  //NO DATA FOR THIS CMD
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}

void cASNPLevel1Handler::sendPong()
{
  static const uint8_t MAX_PONG_ADDRESSES = 64;
                      //highPriority, event, address, cmdType, cmd,           dataVersion
  cUtils::writeStartToTempBuffer(true,false,  0x00,       0x00,    CMD_PONG,      0x01);
  //fill data
  cUtils::writeuint8ToTempBuffer(min(m_selfPersonalAddressCount,MAX_PONG_ADDRESSES));
  for (int i = 0; i<min(m_selfPersonalAddressCount,MAX_PONG_ADDRESSES);++i){
    cUtils::writeuint8ToTempBuffer(m_selfPersonalAddress[i]);
  }
  cUtils::writeuint8ToTempBuffer(min(m_selfGroupAddressCount,MAX_PONG_ADDRESSES));
  for (int i = 0; i<min(m_selfGroupAddressCount,MAX_PONG_ADDRESSES);++i){
    cUtils::writeuint8ToTempBuffer(m_selfGroupAddress[i]);
  }
  uint8_t eventsAddressesCount = 0;
  for (int i = 0; i<256; ++i){
    if (m_EventsSubsribedAddresses[i]>0)
    {
      eventsAddressesCount++;
    }
  }
  if (eventsAddressesCount>MAX_PONG_ADDRESSES)
  {
    eventsAddressesCount = MAX_PONG_ADDRESSES;
  }
  cUtils::writeuint8ToTempBuffer(eventsAddressesCount);
  for (int i = 0; i<256 && MAX_PONG_ADDRESSES>0; ++i){
    if (m_EventsSubsribedAddresses[i]>0)
    {
      cUtils::writeuint8ToTempBuffer(i);
    }
  }
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}

void cASNPLevel1Handler::sendResponseInfo(uint8_t responseAddress)
{
                      //highPriority, event, address,                   cmdType, cmd,                   dataVersion
  cUtils::writeStartToTempBuffer(true,false,  responseAddress,     0x00,    CMD_RESPONSE_INFO,      0x01);
  //fill data
  if (m_selfPersonalAddressCount>0){
    cUtils::writeuint8ToTempBuffer(m_selfPersonalAddress[0]);
  }
  else{
    cUtils::writeuint8ToTempBuffer(0x00);
  }
  cUtils::writeStrToTempBuffer(m_DeviceType);
  cUtils::writeStrToTempBuffer(m_DeviceDescription);
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}

void cASNPLevel1Handler::sendResponseVariablesCount(uint8_t responseAddress)
{
                      //highPriority, event, address,                           cmdType, cmd,                               dataVersion
  cUtils::writeStartToTempBuffer(true,false,  responseAddress,          0x00,    CMD_RESPONSE_VARIABLES_COUNT,      0x01);
  //fill data
  if (m_selfPersonalAddressCount>0){
    cUtils::writeuint8ToTempBuffer(m_selfPersonalAddress[0]);
  }
  else{
    cUtils::writeuint8ToTempBuffer(0x00);
  }
  if (!m_Storage){
    cUtils::writeuint8ToTempBuffer(0);
  }
  else
  {
    cUtils::writeuint8ToTempBuffer(m_Storage->getVariablesCount());    
  }
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}

void cASNPLevel1Handler::sendResponseVariables(uint8_t responseAddress, uint8_t startIndex)
{
  static const uint8_t MAX_VARIABLES_COUNT_PER_PACKAGE = 8;
                      //highPriority, event, address,                     cmdType, cmd,                         dataVersion
  cUtils::writeStartToTempBuffer(true,false,  responseAddress,      0x00,    CMD_RESPONSE_VARIABLES,      0x01);
  //fill data
  if (m_selfPersonalAddressCount>0){
    cUtils::writeuint8ToTempBuffer(m_selfPersonalAddress[0]);
  }
  else{
    cUtils::writeuint8ToTempBuffer(0x00);
  }

  cUtils::writeuint8ToTempBuffer(startIndex);
  if (!m_Storage){
    cUtils::writeuint8ToTempBuffer(0);
  }
  else{
    int count = m_Storage->getVariablesCount();
    if (count<=startIndex){
      cUtils::writeuint8ToTempBuffer(0);
    }
    else{
      int lastIndex = min(startIndex + MAX_VARIABLES_COUNT_PER_PACKAGE-1,count-1);
      cUtils::writeuint8ToTempBuffer(lastIndex);
      for (int i = startIndex; i<=lastIndex; ++i){
        cUtils::writeVariableInfoToTempBuffer(m_Storage->getVariableInfo(i));
      }
    }    
  }  
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}

void cASNPLevel1Handler::sendResponseVariable(uint8_t responseAddress, sVariableInfo* info, void* variableValue)
{
                      //highPriority, event,  address,                 cmdType, cmd,                               dataVersion
  cUtils::writeStartToTempBuffer(true,false,  responseAddress,   0x00,    CMD_RESPONSE_VARIABLE,      0x01);
  //fill data
  if (m_selfPersonalAddressCount>0){
    cUtils::writeuint8ToTempBuffer(m_selfPersonalAddress[0]);
  }
  else{
    cUtils::writeuint8ToTempBuffer(0x00);
  }
  cUtils::writeVariableInfoToTempBuffer(info);
  cUtils::writeToTempBuffer((uint8_t*)variableValue,getVariableSize(info->type));
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}

void cASNPLevel1Handler::sendVariableChangedEvent(sVariableInfo* info, void* variableValue)
{
  if (m_selfPersonalAddressCount==0){
    return;
  }
  
                      //highPriority, event, address,                          cmdType, cmd,                               dataVersion
  cUtils::writeStartToTempBuffer(true,true ,  m_selfPersonalAddress[0],  0x00,    CMD_EVENT_VARIABLE_CHANGED,      0x01);
  //fill data
  cUtils::writeVariableInfoToTempBuffer(info);
  cUtils::writeToTempBuffer((uint8_t*)variableValue,getVariableSize(info->type));
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}

////

void cASNPLevel1Handler::sendRequestInfo(uint8_t targetAddress)
{
  if (m_selfPersonalAddressCount==0){
    return;
  }
  
                      //highPriority, event,  address,                  cmdType, cmd,                               dataVersion
  cUtils::writeStartToTempBuffer(true,false,  targetAddress,    0x00,    CMD_REQUEST_INFO,                  0x01);
  //fill data
  cUtils::writeuint8ToTempBuffer(m_selfPersonalAddress[0]);
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}
void cASNPLevel1Handler::sendGetVariablesCount(uint8_t targetAddress)
{
  if (m_selfPersonalAddressCount==0){
    return;
  }
  
                      //highPriority, event,  address,                   cmdType, cmd,                               dataVersion
  cUtils::writeStartToTempBuffer(true,false,  targetAddress,     0x00,    CMD_GET_VARIABLES_COUNT,                  0x01);
  //fill data
  cUtils::writeuint8ToTempBuffer(m_selfPersonalAddress[0]);
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}
void cASNPLevel1Handler::sendGetVariables(uint8_t targetAddress, uint8_t startIndex)
{
  if (m_selfPersonalAddressCount==0){
    return;
  }
  
                      //highPriority, event,  address,                  cmdType, cmd,                               dataVersion
  cUtils::writeStartToTempBuffer(true,false,  targetAddress,    0x00,    CMD_GET_VARIABLES,                  0x01);
  //fill data
  cUtils::writeuint8ToTempBuffer(m_selfPersonalAddress[0]);
  cUtils::writeuint8ToTempBuffer(startIndex);
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}
void cASNPLevel1Handler::sendGetVariable(uint8_t targetAddress, const sVariableInfo* info)
{
  if (m_selfPersonalAddressCount==0){
    return;
  }
  
                      //highPriority, event,  address,                cmdType, cmd,                               dataVersion
  cUtils::writeStartToTempBuffer(true,false,  targetAddress,  0x00,    CMD_GET_VARIABLE,                  0x01);
  //fill data
  cUtils::writeuint8ToTempBuffer(m_selfPersonalAddress[0]);
  cUtils::writeVariableInfoToTempBuffer(info);
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}
void cASNPLevel1Handler::sendSetVariable(uint8_t targetAddress, const sVariableInfo* info, const void* variableValue)
{
                      //highPriority, event,  address,                cmdType, cmd,                               dataVersion
  cUtils::writeStartToTempBuffer(true,false,  targetAddress,  0x00,    CMD_SET_VARIABLE,                  0x01);
  //fill data
  cUtils::writeVariableInfoToTempBuffer(info);
  cUtils::writeToTempBuffer((uint8_t*)variableValue,getVariableSize(info->type));
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}
void cASNPLevel1Handler::sendLog(uint8_t targetAddress, const char* msg)
{
                      //highPriority, event,  address,                cmdType, cmd,               dataVersion
  cUtils::writeStartToTempBuffer(true,false,  targetAddress,  		     0x00, CMD_LOG,                  0x01);
  //fill data
  uint8_t length ;
  if (strlen(msg)>200)
  {
	  length = 200;
  }
  else
  {
	  length = strlen(msg);
  }
  cUtils::writeToTempBuffer(&length,sizeof(length));
  cUtils::writeToTempBuffer((uint8_t*)msg,length);
  //~fill data
  sendRawMessage(cUtils::writeChecksumToTempBuffer());
}

////////receive

uint8_t cASNPLevel1Handler::readuint8(uint8_t* data, uint8_t& pos,int dataLength)
{
  if (pos>=dataLength)
    return 0;
  ++pos;
  return data[pos-1];
}

sVariableInfo cASNPLevel1Handler::readVariableInfo(uint8_t* data, uint8_t& pos,int dataLength)
{
  sVariableInfo info;
  for (int i = 0; i<16; ++i)
  {
    info.name[i] = readuint8(data,pos,dataLength);
  }
  info.type = (eVariableType)readuint8(data,pos,dataLength);
  info.slotsCount = readuint8(data,pos,dataLength);
  return info;
}

void cASNPLevel1Handler::parsePong(uint8_t* data, uint8_t dataLength)
{
  uint8_t pos = 0;
  readuint8(data,pos,dataLength);//skip type
  readuint8(data,pos,dataLength);//skip cmd
  uint8_t version = readuint8(data,pos,dataLength);
  if (version!=0x01){
    return; //unsupported version
  }
  uint8_t paCount = readuint8(data,pos,dataLength);
  for (int i = 0; i<paCount;++i){
    m_Level0Handler.enablePrivateAddressRouting(readuint8(data,pos,dataLength));
  }

  uint8_t gaCount = readuint8(data,pos,dataLength);
  for (int i = 0; i<gaCount;++i){
    m_Level0Handler.enablePrivateAddressRouting(readuint8(data,pos,dataLength));
  }

  uint8_t eCount = readuint8(data,pos,dataLength);
  for (int i = 0; i<eCount;++i){
    m_Level0Handler.enableEventsRouting(readuint8(data,pos,dataLength));
  }
}

void cASNPLevel1Handler::parseRequestInfo(uint8_t* data, uint8_t dataLength)
{
  uint8_t pos = 0;
  readuint8(data,pos,dataLength);//skip type
  readuint8(data,pos,dataLength);//skip cmd
  uint8_t version = readuint8(data,pos,dataLength);
  if (version!=0x01){
    return; //unsupported version
  }
  
  sendResponseInfo(readuint8(data,pos,dataLength));
}
void cASNPLevel1Handler::parseGetVariablesCount(uint8_t* data, uint8_t dataLength)
{
  uint8_t pos = 0;
  readuint8(data,pos,dataLength);//skip type
  readuint8(data,pos,dataLength);//skip cmd
  uint8_t version = readuint8(data,pos,dataLength);
  if (version!=0x01){
    return; //unsupported version
  }

  sendResponseVariablesCount(readuint8(data,pos,dataLength));
}
void cASNPLevel1Handler::parseGetVariables(uint8_t* data, uint8_t dataLength)
{
  uint8_t pos = 0;
  readuint8(data,pos,dataLength);//skip type
  readuint8(data,pos,dataLength);//skip cmd
  uint8_t version = readuint8(data,pos,dataLength);
  if (version!=0x01){
    return; //unsupported version
  }

  uint8_t responseAddress = readuint8(data,pos,dataLength);
  uint8_t startIndex = readuint8(data,pos,dataLength);
  sendResponseVariables(responseAddress,startIndex);
}
void cASNPLevel1Handler::parseGetVariable(uint8_t* data, uint8_t dataLength)
{  
  if (!m_Storage)
    return;
  uint8_t pos = 0;
  readuint8(data,pos,dataLength);//skip type
  readuint8(data,pos,dataLength);//skip cmd
  uint8_t version = readuint8(data,pos,dataLength);
  if (version!=0x01){
    return; //unsupported version
  }

  uint8_t responseAddress = readuint8(data,pos,dataLength);
  sVariableInfo info = readVariableInfo(data,pos,dataLength);  
  const cASNPVariablesStorage::sVariable* var = m_Storage->getVariable(info.name, info.type);  
  if (!var){    
    return;
  }  
  if (var->info.slotsCount<=info.slotsCount)
    return;
  sendResponseVariable(responseAddress, &info, var->value+var->variableSize*info.slotsCount);
}
void cASNPLevel1Handler::parseSetVariable(uint8_t* data, uint8_t dataLength)
{
  uint8_t pos = 0;
  readuint8(data,pos,dataLength);//skip type
  readuint8(data,pos,dataLength);//skip cmd
  uint8_t version = readuint8(data,pos,dataLength);
  if (version!=0x01){
    return; //unsupported version
  }

  sVariableInfo info = readVariableInfo(data,pos,dataLength);
  uint8_t* variable = &data[pos];

  if (memcmp(info.name,cUtils::StringToChar16(ADDRESS_VAR_NAME),16)==0)
  {
    if (info.slotsCount==0)  
    {
      if (*variable==0)
      {
        //0 not allowed as main device address
        return;
      }
    }
  }
    
  if (m_Storage)
  {    
    if (m_Storage->writeVariable(info.name, info.type,info.slotsCount, variable))
    {
      return; //if variable is written successfully -  we automaticly receive variable changed event, no need to call it manually in next step.      
    }
  }
  //if storage is empty or cant cUtils::write variable - we must manually call variable changed event
  if (m_Listener){  
    m_Listener->onVariableChanged(this, &info, variable);
  }
}

void cASNPLevel1Handler::parseSubscribeToVariable(uint8_t* data, uint8_t dataLength)
{
  uint8_t pos = 0;
  readuint8(data,pos,dataLength);//skip type
  readuint8(data,pos,dataLength);//skip cmd
  uint8_t version = readuint8(data,pos,dataLength);
  if (version!=0x01){
    return; //unsupported version
  }

  if (m_Storage)
  {
    sVariableInfo info = readVariableInfo(data,pos,dataLength);
    bool subscribe = readuint8(data,pos,dataLength)>0;
    bool highPriority = readuint8(data,pos,dataLength)>0;
    
    m_Storage->subscribeToVariable(info.name, info.type, subscribe); 
  }
}

