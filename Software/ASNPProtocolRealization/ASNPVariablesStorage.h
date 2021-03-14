#ifndef ASNPVARIABLESSTORAGE
#define ASNPVARIABLESSTORAGE

#include <Arduino.h>

enum eVariableType : uint8_t{
  VT_UINT8 = 0,
  VT_UINT16,
  VT_UFIXFLOAT16,
  VT_FIXFLOAT16,
  VT_STRING16,
  VT_LINK,
  VT_UINT8VECTOR4
};

uint8_t getVariableSize(eVariableType type);

struct __attribute__((packed)) sVariableInfo{
  char name[16];
  eVariableType type;
  uint8_t slotsCount;//also slot index in some cases
};

struct __attribute__((packed)) sUFixFloat{
  uint8_t integer;
  uint8_t frac;
};

struct __attribute__((packed)) sFixFloat{
  int8_t integer;
  uint8_t frac;
};


struct __attribute__((packed)) sVariableLink{
  uint8_t addressType;
  uint8_t address;
  char linkVariableName[16];
  uint8_t linkVariableSlotIndex;
  eVariableType linkVariableType;
};

uint8_t getVariableSize(eVariableType type);

class cASNPVariablesStorageListener{
public:  
  virtual void onVariableChanged(sVariableInfo* variableInfo, void* variableValue, bool subscribed){}
};

class cASNPVariablesStorage{
public:
  static const uint8_t MAX_VARIABLES_COUNT = 16;  

  static const uint8_t MAX_LISTENERS_COUNT = 3;

  struct __attribute__((packed)) sVariable{
    sVariableInfo info;
    uint8_t nameHash;
    long eepromPos;    
    void* value;
    uint8_t variableSize;
    bool isConfig;
    bool isReadOnly;
    uint8_t subscribersCount;
  };
protected:
  long m_eepromShift;
  sVariable m_Variables[MAX_VARIABLES_COUNT];
  uint8_t m_VariablesCount;

  cASNPVariablesStorageListener* m_Listeners[MAX_LISTENERS_COUNT];
  
  void readVariableFromEEPROM(uint8_t nameHash, eVariableType type, void* value, long eepromPos);
  void writeVariableToEEPROM(uint8_t nameHash, eVariableType type, void* value, long eepromPos);
  uint8_t getVariableIndex(char name[16], eVariableType type, uint8_t slotIndex);  
public:
  cASNPVariablesStorage();
  bool regVariable(char name[16], eVariableType type, uint8_t slotsCount, void* value, bool isConfig, bool canWrite);

  bool addListener(cASNPVariablesStorageListener* listener); //this is for network handlers, not for external use! if you want to know when variable changed - you must listen network handler!

  bool subscribeToVariable(char name[16], eVariableType type, bool subscribe); 
  bool readVariable(char name[16], eVariableType type, uint8_t slotIndex, void* value);
  bool writeVariable(char name[16], eVariableType type, uint8_t slotIndex, void* value);
  uint8_t getVariablesCount(){ return m_VariablesCount;}
  const sVariable* getVariable(char name[16], eVariableType type);
  sVariableInfo* getVariableInfo(uint8_t index) { return &m_Variables[index].info; }  
};


#endif //ASNPVARIABLESSTORAGE
