#ifndef TRACETREE_H
#define TRACETREE_H
\
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QRect>

class sTraceDevice;

struct sTracePort{
    enum eState{
        PENDING,
        MASTER,
        TIMEOUT,
        PROCESSED
    };

    eState state;
    sTraceDevice* connection;
};

struct sTraceDevice{
    enum eState{
        WAIT_IDENTIFICATION,
        IDENTIFIED,
        NO_RESPOND
    };
    eState state;
    QString type;
    QString description;

    uint8_t address;
    QVector<sTracePort> ports;
    int level;

    int xPos;

    QRect renderedRect;
};

struct sTraceTree{
    QVector<sTraceDevice*> devices;
};

struct sDeviceSettingsItem{
    enum eType{
        T_CONST_BYTE,

        T_UINT8,
        //T_STRING
    };

    eType type;

    QString name;

    QString loadedValue;

    QString value;
    QString min;
    QString max;
    sDeviceSettingsItem(){}

    sDeviceSettingsItem(QString setName, eType setType, QString setMin = "",QString setMax = "")
        : type(setType)
        , name(setName)
        , min(setMin)
        , max(setMax)
    {}
};

struct sDeviceSettings{
    QString deviceType;

    QVector<sDeviceSettingsItem> settings;

    QByteArray toByteArray(){
        QByteArray a;
        for (int i = 0; i<settings.size(); ++i){
            switch (settings[i].type) {
            case sDeviceSettingsItem::T_UINT8:
                a.append((char)settings[i].value.toUInt());
                break;
            case sDeviceSettingsItem::T_CONST_BYTE:
                a.append((char)settings[i].value.toInt());
                break;
            default:
                break;
            }
        }
        return a;
    }

    bool fromByteArray(const QByteArray& rawData){
        if (settings.size()!=rawData.size())
            return false;

        const uint8_t* data = (const uint8_t*)rawData.constData();
        int pos = 0;
        for (int i = 0; i<settings.size(); ++i){
            if (i<settings.size())
            switch (settings[i].type) {
            case sDeviceSettingsItem::T_UINT8:
                if (pos==rawData.size())
                    return false;
                settings[i].value = QString::number(data[pos]);
                pos++;
                break;
            case sDeviceSettingsItem::T_CONST_BYTE:
                if (pos==rawData.size())
                    return false;
                settings[i].value = QString::number(data[pos]);
                pos++;
                break;
            default:
                break;
            }

            settings[i].loadedValue = settings[i].value;
        }
        return true;
    }
};

inline uint8_t calcSimpleChecksum(const uint8_t* buf, int length){
    uint8_t checksum = 0;
    for (int i = 0; i<length; ++i)
        checksum += buf[i];
    return checksum;
}

#endif // TRACETREE_H
