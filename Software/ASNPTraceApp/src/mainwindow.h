#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include "tracetree.h"
#include "dialogeditdevicesettings.h"


namespace Ui {
class MainWindow;
}

struct sDevicePort{
    sTraceDevice *device;
    int port;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    enum eBufferState{
        WAIT_START,
        WAIT_ADDRESS,
        WAIT_LENGTH,
        WAIT_CHECKSUM
    };

    static const uint8_t selfAddress = 0;

    static const uint8_t START_CHAR_POS = 0;
    static const uint8_t ADDRESS_POS = 1;
    static const uint8_t LENGTH_POS = 2;
    static  const uint8_t DATA_START_POS = 3;

    static const uint8_t SYS_MSG = 0;

    static const uint8_t SYSMSG_TRACE_START  = 0x10;
    static const uint8_t SYSMSG_TRACE_STEP   = 0x11;
    static const uint8_t SYSMSG_TRACE_STATE  = 0x12;
    static const uint8_t SYSMSG_TRACE_ERROR  = 0x13;
    static const uint8_t SYSMSG_INFO  = 0x21;
    static const uint8_t SYSMSG_RESPONSE_SETTINGS  = 0x32;

    static const uint8_t BROADCAST_ADDRESS = 0x00;

    static const uint8_t START_CHAR_LOW  = 0x04;
    static const uint8_t START_CHAR_HIGH = 0x05;

    static const long MAX_DATA_SIZE = 255;
    static const int PACKAGE_BUFFER_SIZE = MAX_DATA_SIZE+4;
    static const int SEND_DELAY = 1000;
    static const int IDENTIFICATION_DELAY = 500;

    int                 m_IdentificationTimeoutCounter;
    int                 m_PortTimeoutCounter;
    int                 m_DataTimeoutCounter;

    uint8_t             m_Buffer[PACKAGE_BUFFER_SIZE];
    int                 m_BufferSize;    
    eBufferState        m_BufferState;
    uint8_t             m_Checksum;

    sTraceTree          m_TraceResult;
    bool                m_Tracing;
    QSerialPort         m_SerialPort;
    QTimer              m_TimeoutTimer;

    DialogEditDeviceSettings m_DeviceSettingsDialog;

    sDeviceSettings     m_UnknownDeviceSettings;
    QVector<sDeviceSettings> m_DeviceSettingsTemplates;
    sDeviceSettings* getDeviceSettingsTemplate(const QString& deviceType);
    void registerDeviceSettingsTemplate(sDeviceSettings deviceSettingsTemplate);
    void registerDeviceSettingsTemplates();

    void clearDevices();
    void send(const uint8_t *data, int size);
    void send(QByteArray data);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    sTraceDevice* getDeviceByAddr(uint8_t addr, bool create = false);
    sTraceDevice* getCurrentNotIdentifiedDevice();
    sDevicePort getCurrentDevicePort(sTraceDevice *device);
    void processNextDevicePort(sTraceDevice *device);
    void stopWithError(int errorCode);
    void finishTrace(bool force);
    void startIdentification();
private:
    void processDataByte(uint8_t c);
    void receive(uint8_t data, bool clear);
    void processPackage();
signals:
    void processSettings(uint8_t deviceAddress, sDeviceSettings settings);
    void addressChanged(uint8_t deviceAddress, uint8_t newAddress);
private slots:
    void on_pushButtonScanPorts_clicked();

    void on_pushButtonStartStopTrace_clicked();

    void onData();

    void onTimerSendStart();

    void onTimerSendStep();

    void onTimerSendIdentification();

    void onTimeout();

    void on_pushButtonDisconnect_clicked();

    void on_pushButtonConnect_clicked();

    void on_pushButtonClearLog_clicked();

    void onDeviceMousePressed(sTraceDevice* device);

    void requestSettings(uint8_t deviceAddress);
    void setSettings(uint8_t deviceAddress,sDeviceSettings settings);
    void setNewAddress(uint8_t deviceAddress, uint8_t deviceNewAddress);

    void on_pushButton_AddCustomAddress_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
