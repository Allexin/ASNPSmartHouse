#include "ASNPVariablesStorage.h"
#include <EEPROM.h>
#include "utils.h"

uint8_t getVariableSize(eVariableType type)
{
  switch(type){
    case VT_UINT8: return sizeof(uint8_t);break;
    case VT_UINT16: return sizeof(uint16_t);break;
    case VT_UFIXFLOAT16: return sizeof(sUFixFloat);break;
    case VT_FIXFLOAT16: return sizeof(sFixFloat);break;
    case VT_STRING16: return 16;break;
    case VT_LINK: return sizeof(sVariableLink);break;
  }
}

uint8_t getNameHash(char name[16])
{
  uint8_t nameHash = cUtils::simpleChecksum(name, 16);
  if (nameHash==0)
    nameHash = cUtils::simpleChecksum(name, 8);
  if (nameHash==0)
    nameHash = name[0];
  return nameHash;  
}

cASNPVariablesStorage::cASNPVariablesStorage():
  m_VariablesCount(0),
  m_eepromShift(0)
{
  for (int i = 0; i<MAX_LISTENERS_COUNT; ++i)
  {
    m_Listeners[i] = nullptr;   
  }
}

bool cASNPVariablesStorage::regVariable(char name[16], eVariableType type, uint8_t slotsCount, void* value, bool isConfig, bool canWrite)
{
  if (m_VariablesCount>=MAX_VARIABLES_COUNT){
    return false;
  }

  uint8_t varIndex = getVariableIndex(name, type, 0);
  if (varIndex!=0xff){
    return false;
  }

  if (slotsCount==0)
    slotsCount = 1;

  sVariable& var = m_Variables[m_VariablesCount];

  memcpy(var.info.name,name,16);
  var.info.type = type;
  var.info.slotsCount = slotsCount;
  var.nameHash = getNameHash(var.info.name);
  var.value = value;
  var.subscribersCount = 0;

  var.isConfig = isConfig;
  var.isReadOnly = !canWrite;
  
  var.variableSize = getVariableSize(var.info.type);

  if (isConfig && m_eepromShift+ (var.variableSize+1)*slotsCount>1024){
    return false;
  }

  if (isConfig){
    var.eepromPos = m_eepromShift;
    for (int i = 0; i<slotsCount; ++i){
      readVariableFromEEPROM(var.nameHash, type, value,m_eepromShift);
      m_eepromShift+=var.variableSize+1;    
      value+=var.variableSize;
    }
  }

  m_VariablesCount++;
  return true;
}

void cASNPVariablesStorage::readVariableFromEEPROM(uint8_t nameHash, eVariableType type, void* value, long eepromPos)
{
  if (EEPROM.read(eepromPos)==nameHash){
    uint8_t varSize = getVariableSize(type);
    for (int i = 0; i<varSize; ++i){
      ((uint8_t*)value)[i] = EEPROM.read(eepromPos+1+i);
    }
  }
}

void cASNPVariablesStorage::writeVariableToEEPROM(uint8_t nameHash, eVariableType type, void* value, long eepromPos)
{
  EEPROM.write(eepromPos,nameHash);
  uint8_t varSize = getVariableSize(type);
    for (int i = 0; i<varSize; ++i){
      EEPROM.write(eepromPos+1+i,((uint8_t*)value)[i]);
    }
}

uint8_t cASNPVariablesStorage::getVariableIndex(char name[16], eVariableType type, uint8_t slotIndex)
{
  uint8_t nameHash = getNameHash(name);
  for (int i = 0; i<m_VariablesCount; ++i){
    if (m_Variables[i].nameHash==nameHash){
      if (m_Variables[i].info.type==type){
        if (m_Variables[i].info.slotsCount>slotIndex){
          if (memcmp(m_Variables[i].info.name,name,16)==0){
            return i;
          }
        }
      }
    }
  }
  return 0xff;
}

const cASNPVariablesStorage::sVariable* cASNPVariablesStorage::getVariable(char name[16], eVariableType type)
{
  uint8_t nameHash = getNameHash(name);
  for (int i = 0; i<m_VariablesCount; ++i){
    if (m_Variables[i].nameHash==nameHash){
      if (m_Variables[i].info.type==type){
        return &m_Variables[i];
      }
    }
  }
  return nullptr;
}

bool cASNPVariablesStorage::addListener(cASNPVariablesStorageListener* listener)
{
  for (int i = 0; i<MAX_LISTENERS_COUNT; ++i)
  {
    if (m_Listeners[i] == nullptr)
    {
      m_Listeners[i] = listener;
      return true;
    }
  }
  return false;
}

bool cASNPVariablesStorage::readVariable(char name[16], eVariableType type, uint8_t slotIndex, void* value)
{
  uint8_t varIndex = getVariableIndex(name, type, slotIndex);

  if (varIndex==0xff)
    return false;

  memcpy(value, m_Variables[varIndex].value+m_Variables[varIndex].variableSize * slotIndex,m_Variables[varIndex].variableSize);
  return true;
}
bool cASNPVariablesStorage::writeVariable(char name[16], eVariableType type, uint8_t slotIndex, void* value)
{
  uint8_t varIndex = getVariableIndex(name, type, slotIndex);

  if (varIndex==0xff)
    return false;

  if (m_Variables[varIndex].isReadOnly)
    return false;

  memcpy(m_Variables[varIndex].value+m_Variables[varIndex].variableSize * slotIndex,value,m_Variables[varIndex].variableSize);  

  if (m_Variables[varIndex].isConfig)
    writeVariableToEEPROM(m_Variables[varIndex].nameHash, m_Variables[varIndex].info.type, value, m_Variables[varIndex].eepromPos+(m_Variables[varIndex].variableSize + 1) * slotIndex);
  
  for (int i = 0; i<MAX_LISTENERS_COUNT; ++i)
  {
    if (m_Listeners[i] != nullptr)
    {
      m_Listeners[i]->onVariableChanged(&m_Variables[varIndex].info, value, m_Variables[varIndex].subscribersCount>0);
    }
  }

  return true;
}

bool cASNPVariablesStorage::subscribeToVariable(char name[16], eVariableType type, bool subscribe)
{
  uint8_t varIndex = getVariableIndex(name, type, 0);

  if (varIndex==0xff)
    return false;

  if (m_Variables[varIndex].subscribersCount==0xff)  
    return false;

  if (subscribe){
    m_Variables[varIndex].subscribersCount++;
    return true;
  }

  if (m_Variables[varIndex].subscribersCount==0)
    return false;

  m_Variables[varIndex].subscribersCount--;
  return true;  
}

