#ifndef CASNPCLIENT_H
#define CASNPCLIENT_H

#include <QObject>
#include "cudpclient.h"
#include <asnp_utils.h>

struct sVariable{
  sVariableInfo vi;
  QString variableValue;
};

enum class eDeviceType{
    UNKONWN,
    DMX_DIMMER,
    GATE
};

struct sDeviceInfo{
    uint8_t address;
    eDeviceType type;
    uint8_t channel;

    QString category;
    QString name;
    sDeviceInfo(){}
    sDeviceInfo(uint8_t setAddr, eDeviceType setType, uint8_t setChannel = 0);
};

class cASNPClient : public QObject
{
    Q_OBJECT
protected:
    cUDPClient          m_UDPClient;

    QVector<sDeviceInfo> m_AvailableDevices;

    void sendASNPVariables(uint8_t address, QVector<sVariable> variables);
public:
    explicit cASNPClient(QObject *parent = nullptr);

    const QVector<sDeviceInfo>& devices(){return m_AvailableDevices;}

    sDeviceInfo* device(int index)
    {
        if (index<0) return nullptr;
        if (index>=m_AvailableDevices.size()) return nullptr;
        return &m_AvailableDevices[index];
    }


    void startGate(uint8_t address, bool up);
    void stopGate(uint8_t address);

    void setLight(uint8_t address, uint8_t channel, uint8_t value, uint8_t speedSqrt = 1);

    cUDPClient* client(){return &m_UDPClient;}
signals:

private slots:
    void ASNPSetVariableUint8(int address, QString varName, int varslot, int varvalue);
    void ASNPSetVariables2Uint8(int address, QString var1Name, int var1slot, int var1value, QString var2Name, int var2slot, int var2value);

};

#endif // CASNPCLIENT_H
