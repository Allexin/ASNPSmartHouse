#include "ASNPUtils.h"

char cUtils::m_TempBuffer[16];
uint8_t cUtils::m_TempFormatBuffer[cASNPLevel0Handler::PACKAGE_BUFFER_SIZE]; //usit in send<> methods as temprary buffer to avoid dynamic memory allocation
int cUtils::m_TempFormatBufferPos;

void cUtils::writeStartToTempBuffer(bool highPriority, bool groupAddress, bool event, uint8_t address, uint8_t cmdType, uint8_t cmd, uint8_t dataVersion)
{
  uint8_t parity = (highPriority?1:0) +(groupAddress?1:0) +(event?1:0);
  bool parityBit = parity % 2 == 1;
  m_TempFormatBufferPos = 0;
  uint8_t flags = 0;
  if (parityBit)
    flags = flags | cASNPLevel0Handler::FLAG_PARITY;
  if (highPriority)
    flags = flags | cASNPLevel0Handler::FLAG_HIGH_PRIORITY;
  if (groupAddress)
    flags = flags | cASNPLevel0Handler::FLAG_GROUP_ADDRESS;
  if (event)
    flags = flags | cASNPLevel0Handler::FLAG_EVENT;
  writeuint8ToTempBuffer(flags);
  writeuint8ToTempBuffer(address);
  writeuint8ToTempBuffer(0);  //in next writes we fill this field with correct value
  writeuint8ToTempBuffer(cmdType);
  writeuint8ToTempBuffer(cmd);
  writeuint8ToTempBuffer(dataVersion);
}

uint8_t* cUtils::writeChecksumToTempBuffer()
{
  m_TempFormatBuffer[2] = m_TempFormatBufferPos - cASNPLevel0Handler::PACKAGE_HEADER_SIZE;
  
  uint8_t checksum = 0;
  for (int i = 0; i<m_TempFormatBufferPos; ++i){
    checksum += m_TempFormatBuffer[i];
  }
  writeuint8ToTempBuffer(checksum);
  return m_TempFormatBuffer;
}
void cUtils::writeToTempBuffer(const uint8_t* data, uint8_t lngt)
{
  if (m_TempFormatBufferPos+lngt>cASNPLevel0Handler::PACKAGE_BUFFER_SIZE)
    return;
  memcpy(&m_TempFormatBuffer[m_TempFormatBufferPos],data,lngt);  
  m_TempFormatBufferPos+=lngt;
}
void cUtils::writeuint8ToTempBuffer(uint8_t value)
{
  writeToTempBuffer(&value, 1);
}
void cUtils::writeStrToTempBuffer(const String& str)
{
  static const uint8_t MAX_STR_LENGTH = 96;
  static uint8_t tmp_buffer[MAX_STR_LENGTH];
  uint8_t strLen = min(MAX_STR_LENGTH-1,str.length());
  str.toCharArray((char*)tmp_buffer,strLen+1);  
  writeuint8ToTempBuffer(strLen);
  writeToTempBuffer(tmp_buffer,strLen);
}

void cUtils::writeVariableInfoToTempBuffer(const sVariableInfo* info)
{
  writeToTempBuffer((const uint8_t*)info->name,16);
  writeuint8ToTempBuffer(info->type);
  writeuint8ToTempBuffer(info->slotsCount);
}
