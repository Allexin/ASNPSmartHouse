#ifndef UTILS
#define UTILS

#include <Arduino.h>
#include "ASNPLevel0Handler.h"
#include "ASNPVariablesStorage.h"

class cUtils{
protected:
  static uint8_t m_TempFormatBuffer[cASNPLevel0Handler::PACKAGE_BUFFER_SIZE]; //usit in send<> methods as temprary buffer to avoid dynamic memory allocation
  static int m_TempFormatBufferPos;
public:
  static void writeStartToTempBuffer(bool highPriority, bool groupAddress, bool event, uint8_t address, uint8_t cmdType, uint8_t cmd, uint8_t dataVersion);
  static void writeToTempBuffer(const uint8_t* data, uint8_t lngt);
  static void writeuint8ToTempBuffer(uint8_t value);
  static void writeStrToTempBuffer(const String& str);
  static void writeVariableInfoToTempBuffer(const sVariableInfo* info);
  static uint8_t* writeChecksumToTempBuffer(); //write checksum and return buttef with data. call at last step after ALL other writes
  static int getBufferSize() { return m_TempFormatBufferPos; }
protected:
  static char m_TempBuffer[16];
public:
  static const long DEFAULT_BAUD_RATE = 19200;

  static uint8_t simpleChecksum(void* data, uint8_t length){
    uint8_t checksum = 0;
    for (int i = 0; i<length; ++i){
      checksum += ((uint8_t*)data)[i];
    }
    return checksum;
  }

  //result is valid only before next call of this method
  static char* StringToChar16(const String& str){
    memset(m_TempBuffer,0,16);
    str.toCharArray(m_TempBuffer,min(16,str.length()+1));
  }

  static void StringToChar16(const String& str, char* name){
    memset(name,0,16);
    str.toCharArray(name,min(16,str.length()+1));
  }
};

#endif //UTILS
