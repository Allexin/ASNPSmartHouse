#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSerialPort>
#include "dialogvariableinput.h"

#ifdef __GNUC__
#define PACKED_ATTRIB __attribute__((packed))
#else
#define PACKED_ATTRIB
#endif

namespace Ui {
class MainWindow;
}

enum eVariableType : uint8_t{
  VT_UINT8 = 0,
  VT_UINT16,
  VT_UFIXFLOAT16,
  VT_FIXFLOAT16,
  VT_STRING16,
  VT_LINK,
  VT_UINT8VECTOR4
};

#include "pack_start.h"

struct PACKED_ATTRIB sUFixFloat{
  uint8_t integer;
  uint8_t frac;
};

struct PACKED_ATTRIB sFixFloat{
  int8_t integer;
  uint8_t frac;
};


struct PACKED_ATTRIB sVariableLink{
  uint8_t addressType;
  uint8_t address;
  char linkVariableName[16];
  uint8_t linkVariableSlotIndex;
  eVariableType linkVariableType;
};

struct PACKED_ATTRIB sVariableInfo{
  char name[16];
  eVariableType type;
  uint8_t slotsCount;//also slot index in some cases
};

#include "pack_end.h"

struct sDeviceVariable{
    sVariableInfo info;
    QByteArray value;
};

struct sDeviceInfo{
    uint8_t address;
    bool infoRequested;
    QString deviceType;
    QString deviceDescription;
    QVector<sDeviceVariable> variables;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    enum eBufferState
    {
        BS_WAIT_START,
        BS_WAIT_ADDRESS,
        BS_WAIT_LENGTH,
        BS_WAIT_CHECKSUM
    };


    QSerialPort         m_SerialPort;
    QTimer              m_UpdatePortsTimer;
    QTimer              m_ResetReceiverStateTimer;
    bool                m_ReceiverActive;
    QTimer              m_RequestInfoTimer;

    DialogVariableInput m_DialogVariableInput;

    QVector<sDeviceInfo> m_Devices;
    int gedDeviceIndex(uint8_t address);
    void addDevice(uint8_t address);
    void updateDeviceInfo(uint8_t deviceAddress,QString deviceType,QString deviceDescription);
    void updateDeviceVariable(uint8_t deviceAddress, sDeviceVariable variable);
    void updateDevicesList();
protected:
    void addLog(uint8_t data, bool newLine);
    void addLogMessageType(uint8_t cmdType, uint8_t cmd);
protected:
    static const uint8_t INFO_POS = 0;
    static const uint8_t ADDRESS_POS = 1;
    static const uint8_t LENGTH_POS = 2;
    static const uint8_t DATA_START_POS = 3;
    uint8_t             m_Checksum;
    QByteArray          m_Buffer;
    eBufferState        m_BufferState;
    void processChar(uint8_t data);
    void addToBuffer(uint8_t data, bool clear);
protected:
    static const uint8_t DEFAULT_ADDRESS = 0xff;
    QByteArray          m_SendBuffer;
    void writeStartToTempBuffer(bool highPriority, bool groupAddress, bool event, uint8_t address, uint8_t dataLength, uint8_t cmdType, uint8_t cmd, uint8_t dataVersion);
    void writeDataLengthToTempBuffer();
    void writeChecksumToTempBuffer();
    void writeToTempBuffer(const uint8_t* data, uint8_t lngt);
    void writeuint8ToTempBuffer(uint8_t value);
    void writeVariableInfoToTempBuffer(sVariableInfo* info);

    void sendPing();
    void sendPong();
    void sendRequestInfo(uint8_t targetAddress, bool groupAddress);
    void sendGetVariablesCount(uint8_t targetAddress, bool groupAddress);
    void sendGetVariables(uint8_t targetAddress, bool groupAddress, uint8_t startIndex);
    void sendGetVariable(uint8_t targetAddress, bool groupAddress, sVariableInfo* info);
    void sendSetVariable(uint8_t targetAddress, bool groupAddress, sVariableInfo* info, void* variableValue);

    void send(QByteArray data);
protected:
    QByteArray          m_DataBuffer;
    uint8_t takeuint8();
    QString takeString();
    sVariableInfo takeVariableInfo();
    void processPackage();
    void processMessage(uint8_t dataLength, char *data);

    void parsePong();
    void parseResponseInfo();
    void parseVariablesCount();
    void parseResponseVariables();
    void parseResponseVariable();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void tryUpdatePortsList();
    void rescan();
    void resetReceiverState();

    void requestDevicesInfo();

    void onData();
protected slots:
    void actionRequestVariables();
    void actionSetVariable();
private slots:
    void on_pushButton_Connect_clicked();

    void on_pushButton_ClearLogs_clicked();

    void on_pushButton_Disconnect_clicked();

    void on_treeWidgetDevices_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
