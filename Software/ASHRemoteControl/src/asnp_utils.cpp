#include "asnp_utils.h"


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


void StringToName(QString value, char* name)
{
    memset(name,0,16);
    QByteArray dta = value.toLatin1();
    memcpy(name, dta.data(), qMin(16,dta.size()));
}
