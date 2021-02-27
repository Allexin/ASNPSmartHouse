#include "casnpclient.h"

QString deviceTypeToString(eDeviceType type)
{
    switch (type) {
        case eDeviceType::UNKONWN:return "UNKNOWN";
        case eDeviceType::DMX_DIMMER:return "DMX_DIMMER";
        case eDeviceType::GATE:return "GATE";
    };

    return "INVALID";
}

sDeviceInfo::sDeviceInfo(uint8_t setAddr, eDeviceType setType, uint8_t setChannel):
    address(setAddr),
    type(setType),
    channel(setChannel)
{
    name = deviceTypeToString(type)+"["+QString::number(address)+"]";
}

void cASNPClient::sendASNPVariables(uint8_t address, QVector<sVariable> variables)
{
    QByteArray package;
    for (int i = 0; i<variables.size(); ++i)
    {
        cASNPMessageBuilder builder;
        builder.writeStart(true,false,false,address,0x00,0x00,CMD_SET_VARIABLE,0x01);
        builder.writeVariableInf(&variables[i].vi);
        switch(variables[i].vi.type)
        {
            case VT_UINT8: {builder.writeuint8(variables[i].variableValue.toInt());};break;
            case VT_UINT16: {qDebug() << "TODO";};break;
            case VT_UFIXFLOAT16: {qDebug() << "TODO";};break;
            case VT_FIXFLOAT16: {qDebug() << "TODO";};break;
            case VT_STRING16: {qDebug() << "TODO";};break;
            case VT_LINK: {qDebug() << "TODO";};break;
        }
        builder.writeEnd();
        package = package + builder.package();
    }

    m_UDPClient.sendCommand((const uint8_t*)package.constData(), package.size());
}

cASNPClient::cASNPClient(QObject *parent) : QObject(parent)
{
    m_AvailableDevices.push_back(sDeviceInfo(21,eDeviceType::GATE));        //большое
    m_AvailableDevices.push_back(sDeviceInfo(22,eDeviceType::GATE));        //маленьбкое рядом с большим
    m_AvailableDevices.push_back(sDeviceInfo(23,eDeviceType::GATE));        //угловое улица
    m_AvailableDevices.push_back(sDeviceInfo(24,eDeviceType::GATE));        //чердак
    m_AvailableDevices.push_back(sDeviceInfo(25,eDeviceType::GATE));        //улица

    m_AvailableDevices.push_back(sDeviceInfo(1,eDeviceType::DMX_DIMMER,0)); //детская
    m_AvailableDevices.push_back(sDeviceInfo(1,eDeviceType::DMX_DIMMER,1));
    m_AvailableDevices.push_back(sDeviceInfo(1,eDeviceType::DMX_DIMMER,2));
    m_AvailableDevices.push_back(sDeviceInfo(1,eDeviceType::DMX_DIMMER,3));

    m_AvailableDevices.push_back(sDeviceInfo(10,eDeviceType::DMX_DIMMER,0)); //кухня

    m_AvailableDevices.push_back(sDeviceInfo(12,eDeviceType::GATE));        //детская

    m_AvailableDevices.push_back(sDeviceInfo(11,eDeviceType::DMX_DIMMER,0)); //взрослая
    m_AvailableDevices.push_back(sDeviceInfo(11,eDeviceType::DMX_DIMMER,1));
    m_AvailableDevices.push_back(sDeviceInfo(11,eDeviceType::DMX_DIMMER,2));
    m_AvailableDevices.push_back(sDeviceInfo(11,eDeviceType::DMX_DIMMER,3));
    m_AvailableDevices.push_back(sDeviceInfo(11,eDeviceType::DMX_DIMMER,4));
    m_AvailableDevices.push_back(sDeviceInfo(11,eDeviceType::DMX_DIMMER,5));
}

void cASNPClient::ASNPSetVariableUint8(int address, QString varName, int varslot, int varvalue)
{
    QVector<sVariable> variables;
    sVariable var;
    var.variableValue = QString::number(varvalue);
    StringToName(varName,var.vi.name);
    var.vi.slotIndex = varslot;
    var.vi.type = VT_UINT8;
    variables.push_back(var);

    sendASNPVariables(address,variables);
    sendASNPVariables(address,variables);
    sendASNPVariables(address,variables);
    sendASNPVariables(address,variables);
}

void cASNPClient::ASNPSetVariables2Uint8(int address, QString var1Name, int var1slot, int var1value, QString var2Name, int var2slot, int var2value)
{
    QVector<sVariable> variables;
    sVariable var;
    var.variableValue = QString::number(var1value);
    StringToName(var1Name,var.vi.name);
    var.vi.slotIndex = var1slot;
    var.vi.type = VT_UINT8;
    variables.push_back(var);
    var.variableValue = QString::number(var2value);
    StringToName(var2Name,var.vi.name);
    var.vi.slotIndex = var2slot;
    var.vi.type = VT_UINT8;
    variables.push_back(var);

    sendASNPVariables(address,variables);
    sendASNPVariables(address,variables);
    sendASNPVariables(address,variables);
    sendASNPVariables(address,variables);
}

void cASNPClient::startGate(uint8_t address, bool up)
{
    ASNPSetVariableUint8(address, "Active", 0, 0);
    ASNPSetVariables2Uint8(address, "Active", 0, 1, "Direction", 0, up?0:1);
}

void cASNPClient::stopGate(uint8_t address)
{
    ASNPSetVariables2Uint8(address, "Direction", 0, 0, "Active", 0, 0);
}

void cASNPClient::setLight(uint8_t address, uint8_t channel, uint8_t value, uint8_t speedSqrt)
{
    ASNPSetVariables2Uint8(address, "ChannelStepSqrt", channel, speedSqrt, "ChannelTarget", channel, value);
}
