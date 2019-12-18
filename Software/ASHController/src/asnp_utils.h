#ifndef ASNP_UTILS_H
#define ASNP_UTILS_H

#include <stdint.h>
#include <QByteArray>
#include <QString>

static const uint8_t FLAG_PARITY = 1;
static const uint8_t FLAG_HIGH_PRIORITY = 2;
static const uint8_t FLAG_GROUP_ADDRESS = 4;
static const uint8_t FLAG_EVENT = 8;

static const uint8_t CMD_SET_VARIABLE = 0x28;

static const long PACKAGE_HEADER_SIZE = 1+1+1; //PACKAGE_INFO + ADDRESS + DATA_SIZE
static const long PACKAGE_SERVICE_DATA_SIZE = PACKAGE_HEADER_SIZE+1; // + CHECKSUM
static const long MAX_PACKAGE_DATA_SIZE = 255;
static const long PACKAGE_BUFFER_SIZE = MAX_PACKAGE_DATA_SIZE + PACKAGE_SERVICE_DATA_SIZE;

enum eVariableType : uint8_t{
  VT_UINT8 = 0,
  VT_UINT16,
  VT_UFIXFLOAT16,
  VT_FIXFLOAT16,
  VT_STRING16,
  VT_LINK
};

#ifdef __GNUC__
#define pack_struct __attribute__((packed))
#else
#define pack_struct
#pragma pack(push,1)
#endif

struct pack_struct sUFixFloat{
  uint8_t integer;
  uint8_t frac;
};

struct pack_struct sFixFloat{
  int8_t integer;
  uint8_t frac;
};

struct pack_struct sVariableLink{
  uint8_t addressType;
  uint8_t address;
  char linkVariableName[16];
  uint8_t linkVariableSlotIndex;
  eVariableType linkVariableType;
};

struct pack_struct sVariableInfo{
  char name[16];
  eVariableType type;
  union{
    uint8_t slotsCount;
    uint8_t slotIndex;
  };
};

#ifdef __GNUC__
#undef pack_struct
#else
#undef pack_struct
#pragma pack(pop)
#endif

uint8_t getVariableSize(eVariableType type);



class cASNPMessageBuilder{
protected:
    QByteArray          m_SendBuffer;
public:
    void writeStart(bool highPriority, bool groupAddress, bool event, uint8_t address, uint8_t dataLength, uint8_t cmdType, uint8_t cmd, uint8_t dataVersion)
    {
        uint8_t parity = (highPriority?1:0) +(groupAddress?1:0) +(event?1:0);
        bool parityBit = parity % 2 == 1;
        m_SendBuffer.clear();
        uint8_t flags = 0;
        if (parityBit)
          flags = flags | FLAG_PARITY;
        if (highPriority)
          flags = flags | FLAG_HIGH_PRIORITY;
        if (groupAddress)
          flags = flags | FLAG_GROUP_ADDRESS;
        if (event)
          flags = flags | FLAG_EVENT;
        m_SendBuffer.push_back(flags);
        m_SendBuffer.push_back(address);
        if (dataLength<3)
          dataLength = 3;
        m_SendBuffer.push_back(dataLength);
        m_SendBuffer.push_back(cmdType);
        m_SendBuffer.push_back(cmd);
        m_SendBuffer.push_back(dataVersion);
    }

    void writeEnd()
    {
        m_SendBuffer[2] = m_SendBuffer.size() - PACKAGE_HEADER_SIZE;
        uint8_t checksum = 0;
        for (int i = 0; i<m_SendBuffer.size(); ++i){
            checksum += m_SendBuffer[i];
        }
        writeuint8(checksum);
    }
    void write(const uint8_t* data, uint8_t lngt)
    {
      if (m_SendBuffer.size()+lngt>PACKAGE_BUFFER_SIZE)
        return;
      m_SendBuffer.append((const char*)data,lngt);
    }
    void writeuint8(uint8_t value)
    {
        write(&value, 1);
    }

    void writeVariableInf(sVariableInfo *info)
    {
        for (int i = 0; i<16; ++i){
            writeuint8(info->name[i]);
        }
        writeuint8(info->type);
        writeuint8(info->slotsCount);
    }

    QByteArray package() { return m_SendBuffer; }
};

void StringToName(QString value, char* name);

#endif // ASNP_UTILS_H
