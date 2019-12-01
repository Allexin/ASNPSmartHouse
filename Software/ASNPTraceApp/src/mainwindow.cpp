#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
QString rawDataToHexString(const uint8_t* data, int size){
    QString result;
    for (int i = 0; i<size; i++){
        QString h = QString::number((unsigned char)(data[i]),16).toUpper();
        if (h.length()==1)
            h = "0"+h;
        result = result +h +" ";
    }
    return result;
}

QString rawDataToHexString(QByteArray data){
    QString result;
    for (int i = 0; i<data.size(); i++){
        QString h = QString::number((unsigned char)(data[i]),16).toUpper();
        if (h.length()==1)
            h = "0"+h;
        result = result +h +" ";
    }
    return result;
}

sDeviceSettings *MainWindow::getDeviceSettingsTemplate(const QString &deviceType)
{
    for (int i = 0; i<m_DeviceSettingsTemplates.size(); ++i)
        if (m_DeviceSettingsTemplates[i].deviceType==deviceType)
            return &m_DeviceSettingsTemplates[i];
    return NULL;
}

void MainWindow::registerDeviceSettingsTemplate(sDeviceSettings deviceSettingsTemplate)
{
    m_DeviceSettingsTemplates.append(deviceSettingsTemplate);
}

void MainWindow::registerDeviceSettingsTemplates()
{
    {
        sDeviceSettings light_switch_ex;
        light_switch_ex.deviceType = "LIGHT_SWITCH_EX";
        light_switch_ex.settings.append(sDeviceSettingsItem(tr("Dimmer address"),sDeviceSettingsItem::eType::T_UINT8,"1","255"));
        light_switch_ex.settings.append(sDeviceSettingsItem(tr("Turn on time"),sDeviceSettingsItem::eType::T_UINT8,"0","255"));
        light_switch_ex.settings.append(sDeviceSettingsItem(tr("Turn off time"),sDeviceSettingsItem::eType::T_UINT8,"0","255"));
        light_switch_ex.settings.append(sDeviceSettingsItem(tr("Main channel default value"),sDeviceSettingsItem::eType::T_UINT8,"0","255"));
        light_switch_ex.settings.append(sDeviceSettingsItem(tr("Additional channel default value"),sDeviceSettingsItem::eType::T_UINT8,"0","255"));
        registerDeviceSettingsTemplate(light_switch_ex);
    }

    {
        sDeviceSettings dimmer_pwm_4c_ac;
        dimmer_pwm_4c_ac.deviceType = "DIMMER_PWM_4C_AC";
        dimmer_pwm_4c_ac.settings.append(sDeviceSettingsItem(tr("Channel 1 minimum"),sDeviceSettingsItem::eType::T_UINT8,"0","255"));
        dimmer_pwm_4c_ac.settings.append(sDeviceSettingsItem(tr("Channel 2 minimum"),sDeviceSettingsItem::eType::T_UINT8,"0","255"));
        dimmer_pwm_4c_ac.settings.append(sDeviceSettingsItem(tr("Channel 3 minimum"),sDeviceSettingsItem::eType::T_UINT8,"0","255"));
        dimmer_pwm_4c_ac.settings.append(sDeviceSettingsItem(tr("Channel 4 minimum"),sDeviceSettingsItem::eType::T_UINT8,"0","255"));
        registerDeviceSettingsTemplate(dimmer_pwm_4c_ac);
    }
}

void MainWindow::clearDevices()
{
    for (int i = 0; i<m_TraceResult.devices.size(); ++i){
        delete m_TraceResult.devices[i];
    }
    m_TraceResult.devices.clear();
}

void MainWindow::send(const uint8_t* data, int size)
{
    m_SerialPort.write((char*)data,size);
    ui->listWidgetLog->addItem(">>" +rawDataToHexString(data,size));
}

void MainWindow::send(QByteArray data)
{
    m_SerialPort.write(data);
    ui->listWidgetLog->addItem(">>" +rawDataToHexString(data));
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_Tracing(false),
    m_DeviceSettingsDialog(this),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget->setTraceResult(&m_TraceResult);

    connect(ui->widget,SIGNAL(onDeviceMousePressed(sTraceDevice*)),this,SLOT(onDeviceMousePressed(sTraceDevice*)));

    connect(&m_SerialPort,SIGNAL(readyRead()),this,SLOT(onData()));
    connect(&m_TimeoutTimer,SIGNAL(timeout()),this,SLOT(onTimeout()));

    connect(&m_DeviceSettingsDialog,SIGNAL(requestSettings(uint8_t)),this,SLOT(requestSettings(uint8_t)));
    connect(&m_DeviceSettingsDialog,SIGNAL(setSettings(uint8_t,sDeviceSettings)),this,SLOT(setSettings(uint8_t,sDeviceSettings)));
    connect(&m_DeviceSettingsDialog,SIGNAL(setNewAddress(uint8_t, uint8_t)),this,SLOT(setNewAddress(uint8_t, uint8_t)));

    connect(this,SIGNAL(processSettings(uint8_t, sDeviceSettings)),&m_DeviceSettingsDialog,SLOT(processSettings(uint8_t, sDeviceSettings)));
    connect(this,SIGNAL(addressChanged(uint8_t, uint8_t)),&m_DeviceSettingsDialog,SLOT(addressChanged(uint8_t, uint8_t)));

    registerDeviceSettingsTemplates();

    m_TimeoutTimer.start(250);

    on_pushButtonScanPorts_clicked();
}

MainWindow::~MainWindow()
{
    clearDevices();
    delete ui;
}

sTraceDevice *MainWindow::getDeviceByAddr(uint8_t addr, bool create)
{
    if (addr==0)
        return NULL;
    for (int i = 0; i<m_TraceResult.devices.size(); ++i){
        if (m_TraceResult.devices[i]->address==addr)
            return m_TraceResult.devices[i];
    }
    if (!create)
        return NULL;
    sTraceDevice* dev = new sTraceDevice();
    dev->state = sTraceDevice::eState::IDENTIFIED;
    dev->address = addr;
    dev->level = 0;
    dev->ports.resize(1);
    dev->ports[0].connection = NULL;
    dev->ports[0].state = sTracePort::eState::PROCESSED;
    m_TraceResult.devices.append(dev);
    return dev;
}

sTraceDevice *MainWindow::getCurrentNotIdentifiedDevice()
{
    for (int i = 0; i<m_TraceResult.devices.size(); ++i){
        if (m_TraceResult.devices[i]->state==sTraceDevice::eState::WAIT_IDENTIFICATION)
            return m_TraceResult.devices[i];
    }
    return NULL;
}

sDevicePort MainWindow::getCurrentDevicePort(sTraceDevice* device)
{
    sDevicePort d;
    d.device = NULL;
    for (int i = 0; i<device->ports.size(); ++i){
        if (device->ports[i].state==sTracePort::eState::PENDING){
            if (device->ports[i].connection!=NULL){
                return getCurrentDevicePort(device->ports[i].connection);
            }
            else{
                d.device = device;
                d.port = i;
                return d;
            }
        };
    }
    return d;
}

void MainWindow::processNextDevicePort(sTraceDevice *device)
{
    int masterPort = -1;
    for (int i = 0; i<device->ports.size(); ++i){
        if (device->ports[i].state==sTracePort::eState::PENDING){
            if (device->ports[i].connection==NULL){
                m_PortTimeoutCounter = 10;
                QTimer::singleShot(SEND_DELAY,this,SLOT(onTimerSendStep()));
                return;
            }
            else{
                processNextDevicePort(device->ports[i].connection);
                return;
            }
        }

        if (device->ports[i].state==sTracePort::eState::MASTER){
            masterPort = i;
        }
    }

    if (masterPort==-1){
        startIdentification();
        return;
    }

    if (device->ports[masterPort].connection==NULL){
        stopWithError(0);
        return;
    }

    sTraceDevice* parent = device->ports[masterPort].connection;

    for (int i = 0; parent->ports.size(); ++i)
        if (parent->ports[i].connection==device){
            parent->ports[i].state = sTracePort::eState::PROCESSED;
            break;
        }
    processNextDevicePort(parent);
}

void MainWindow::stopWithError(int errorCode)
{
    finishTrace(false);
    ui->statusBar->showMessage(tr("Stoped by error %1").arg(QString::number(errorCode,16)));
}

void MainWindow::finishTrace(bool force)
{
    if (m_Tracing){
        m_Tracing = false;
        ui->pushButtonStartStopTrace->setText(tr("Start"));

        if (force){
            ui->statusBar->showMessage(tr("Canceled by user"));
        }
        else{
            ui->statusBar->showMessage(tr("Complete"));
        }
    }
}

void MainWindow::startIdentification()
{
    QTimer::singleShot(IDENTIFICATION_DELAY, this, SLOT(onTimerSendIdentification()));
    ui->statusBar->showMessage(tr("Identifying..."));
}

void MainWindow::on_pushButtonScanPorts_clicked()
{
    ui->comboBoxPort->clear();
    QList<QSerialPortInfo> ports =  QSerialPortInfo::availablePorts();
        for (int i = 0; i<ports.size(); i++)
            ui->comboBoxPort->addItem(ports[i].portName());
}

void MainWindow::on_pushButtonStartStopTrace_clicked()
{
    if (m_Tracing){
        finishTrace(true);
    }
    else{
        m_Tracing = true;
        m_BufferSize = 0;
        m_BufferState = eBufferState::WAIT_START;
        QTimer::singleShot(SEND_DELAY,this, SLOT(onTimerSendStart()));
        ui->pushButtonStartStopTrace->setText(tr("Stop"));
        ui->statusBar->showMessage(tr("Tracing..."));
    }
}

void MainWindow::processPackage()
{
    uint8_t dataLength = m_Buffer[LENGTH_POS];
    if (m_Buffer[ADDRESS_POS]==BROADCAST_ADDRESS){
        if (dataLength>=2){
            if (m_Buffer[DATA_START_POS+0]==SYS_MSG){
                switch (m_Buffer[DATA_START_POS+1]) {
                case SYSMSG_TRACE_STATE:{
                    if (dataLength==5){
                        uint8_t addr = m_Buffer[DATA_START_POS+2];
                        uint8_t portsCount = m_Buffer[DATA_START_POS+3];
                        uint8_t masterPort = m_Buffer[DATA_START_POS+4];

                        sDevicePort devport = MainWindow::getCurrentDevicePort(m_TraceResult.devices[0]);
                        if (devport.device && masterPort<portsCount){
                            sTraceDevice* dev = new sTraceDevice();
                            dev->address = addr;
                            if (addr==0){
                                dev->state = sTraceDevice::eState::IDENTIFIED;
                            }
                            else{
                                dev->state = sTraceDevice::eState::WAIT_IDENTIFICATION;
                            }
                            dev->level = devport.device->level+1;
                            dev->ports.resize(portsCount);
                            for (int i = 0; i<portsCount; ++i){
                                dev->ports[i].connection = NULL;
                                dev->ports[i].state = sTracePort::eState::PENDING;
                            }

                            dev->ports[masterPort].connection = devport.device;
                            dev->ports[masterPort].state = sTracePort::eState::MASTER;
                            devport.device->ports[devport.port].connection = dev;
                            m_TraceResult.devices.append(dev);
                            ui->widget->traceResultUpdated();
                            processNextDevicePort(m_TraceResult.devices[0]);
                        }
                        else{
                            //WTF????
                            stopWithError(1);
                        }
                        ui->widget->traceResultUpdated();
                    }
                    else{
                        //error
                        stopWithError(2);
                    }
                };break;
                case SYSMSG_INFO:{
                    if (dataLength>=4){
                        uint8_t addr = m_Buffer[DATA_START_POS+2];
                        uint8_t typeLength = m_Buffer[DATA_START_POS+3];
                        uint8_t typeStart = 4;
                        if (dataLength>=typeStart+typeLength+1){
                            uint8_t descLength = m_Buffer[DATA_START_POS+typeStart+typeLength];
                            uint8_t descStart = typeStart+typeLength+1;
                            if (dataLength==descStart+descLength){
                                sTraceDevice* device = getDeviceByAddr(addr,true);
                                if (device){
                                    device->type = QString::fromUtf8((const char*)&m_Buffer[DATA_START_POS+typeStart],typeLength);
                                    device->description = QString::fromUtf8((const char*)&m_Buffer[DATA_START_POS+descStart],descLength);
                                    device->state = sTraceDevice::eState::IDENTIFIED;
                                    ui->widget->traceResultUpdated();
                                    QTimer::singleShot(IDENTIFICATION_DELAY, this, SLOT(onTimerSendIdentification()));
                                }
                            }
                        }
                    }
                };break;
                case SYSMSG_RESPONSE_SETTINGS:{
                    if (dataLength>=4){
                        uint8_t addr = m_Buffer[DATA_START_POS+2];
                        sTraceDevice* device = getDeviceByAddr(addr);
                        if (device){
                            sDeviceSettings* settings = getDeviceSettingsTemplate(device->type);
                            if (!settings){
                                settings = &m_UnknownDeviceSettings;
                                settings->settings.resize(dataLength-3);
                                for (int i = 0; i<settings->settings.size(); ++i){
                                    settings->settings[i].type = sDeviceSettingsItem::eType::T_UINT8;
                                    settings->settings[i].name = tr("unknown");
                                    settings->settings[i].max = "255";
                                    settings->settings[i].min = "0";
                                }
                            }

                            if (settings){
                                if (!settings->fromByteArray(QByteArray((const char*)&m_Buffer[DATA_START_POS+3],dataLength-3))){
                                    settings = &m_UnknownDeviceSettings;
                                    settings->settings.resize(dataLength-3);
                                    for (int i = 0; i<settings->settings.size(); ++i){
                                        settings->settings[i].type = sDeviceSettingsItem::eType::T_UINT8;
                                        settings->settings[i].name = tr("unknown");
                                        settings->settings[i].max = "255";
                                        settings->settings[i].min = "0";
                                    }
                                    settings->fromByteArray(QByteArray((const char*)&m_Buffer[DATA_START_POS+3],dataLength-3));
                                    ui->statusBar->showMessage(tr("Received settings not correct for device type %1").arg(device->type),5000);
                                }
                                emit processSettings(device->address, *settings);
                            }
                            else{
                                ui->statusBar->showMessage(tr("Settings template for device type %1 not found").arg(device->type),5000);
                            }
                        }
                    }
                };break;
                case SYSMSG_TRACE_ERROR:{
                    if (dataLength>=3){
                        uint8_t errorCode = m_Buffer[DATA_START_POS+2];
                        stopWithError(errorCode);
                    }
                };break;
                default:
                    break;
                }
            }
        }
    }
}

void MainWindow::receive(uint8_t data, bool clear)
{
    if (clear){
        m_BufferSize = 0;
        m_Checksum = 0;
      }
      m_Buffer[m_BufferSize] = data;
      m_Checksum += data;
      if (m_BufferSize<PACKAGE_BUFFER_SIZE-1)
          m_BufferSize++;
}

void MainWindow::processDataByte(uint8_t c)
{
    switch(m_BufferState){
          case WAIT_START:{
            if (c==START_CHAR_LOW || c==START_CHAR_HIGH){
              receive(c,true);
              m_BufferState = WAIT_ADDRESS;
            }
          };break;
          case WAIT_ADDRESS:{
            receive(c,false);
            m_BufferState = WAIT_LENGTH;
          };break;
          case WAIT_LENGTH:{
            if (c<=MAX_DATA_SIZE){
              receive(c,false);
              m_BufferState = WAIT_CHECKSUM;
            }
            else{
              //incorrect data, wait next package
                ui->listWidgetLog->addItem("too big data");
              m_BufferState = WAIT_START;
            }
          };break;
          case WAIT_CHECKSUM:{
            uint8_t currentChecksum = m_Checksum;
            receive(c,false);
            if (m_Buffer[LENGTH_POS]==m_BufferSize-LENGTH_POS-2){
              m_BufferState = WAIT_START;
              if (c==currentChecksum){
                  //data ready
                  processPackage();
              }
              else{
                //error - incorrect checksum
                  ui->listWidgetLog->addItem("incorrect checksum" + QString::number(c) + " " + QString::number(currentChecksum));
              }
            }
          };break;
    }
}

void MainWindow::onData()
{
    m_DataTimeoutCounter = 3;
    QByteArray data = m_SerialPort.readAll();
    for (int i = 0; i<data.size(); ++i)
        processDataByte(data[i]);
    ui->listWidgetLog->addItem("<<" +rawDataToHexString(data));
}

void MainWindow::onTimerSendStart()
{
    if (!m_Tracing)
        return;
    const uint8_t data[6] = {0x05, 0x00, 0x02, 0x00, 0x10, 0x17};
    send(data,6);
    QTimer::singleShot(SEND_DELAY,this,SLOT(onTimerSendStep()));
    m_Tracing = true;
    ui->pushButtonStartStopTrace->setEnabled(true);
    m_PortTimeoutCounter = 10;
    m_IdentificationTimeoutCounter = 0;

    clearDevices();
    sTraceDevice* dev = new sTraceDevice();
    dev->state = sTraceDevice::eState::IDENTIFIED;
    dev->address = 0;
    dev->level = 0;
    dev->ports.resize(1);;
    dev->ports[0].connection = NULL;
    dev->ports[0].state = sTracePort::eState::PENDING;
    m_TraceResult.devices.append(dev);
    ui->widget->traceResultUpdated();
}

void MainWindow::onTimerSendStep()
{
    if (!m_Tracing)
        return;
    m_PortTimeoutCounter = 10;
    const uint8_t data[6] = {0x05, 0x00, 0x02, 0x00, 0x11, 0x18};
    send(data,6);
}

void MainWindow::onTimerSendIdentification()
{
    if (!m_Tracing)
        return;
    m_IdentificationTimeoutCounter = 10;
    sTraceDevice* device = getCurrentNotIdentifiedDevice();
    if (device){
        uint8_t data[7] = {0x05, device->address, 0x03, 0x00, 0x20, selfAddress, 0};
        data[6] = calcSimpleChecksum(data,6);
        send(data,7);
    }
    else{
        finishTrace(false);
    }
}

void MainWindow::onTimeout()
{
    if (!m_Tracing)
        return;

    if (m_DataTimeoutCounter>0){
        m_DataTimeoutCounter--;
        if (m_DataTimeoutCounter==0){
            m_DataTimeoutCounter = 0;
            m_BufferSize = 0;
            m_BufferState = eBufferState::WAIT_START;
        }
    }

    if (m_PortTimeoutCounter>0){
        m_PortTimeoutCounter--;
        if (m_PortTimeoutCounter==0){
            sDevicePort devport = MainWindow::getCurrentDevicePort(m_TraceResult.devices[0]);
            if (devport.device)
                devport.device->ports[devport.port].state = sTracePort::eState::TIMEOUT;
            ui->widget->traceResultUpdated();
            MainWindow::processNextDevicePort(m_TraceResult.devices[0]);
        }
    }
    if (m_IdentificationTimeoutCounter>0){
        m_IdentificationTimeoutCounter--;
        if (m_IdentificationTimeoutCounter==0){
            sTraceDevice* device = getCurrentNotIdentifiedDevice();
            if (device){
                device->state = sTraceDevice::eState::NO_RESPOND;
                ui->widget->traceResultUpdated();
                onTimerSendIdentification();
            }
            else{

            }
        }
    }
}

void MainWindow::on_pushButtonDisconnect_clicked()
{
    m_SerialPort.close();

    ui->pushButtonScanPorts->setEnabled(true);
    ui->pushButtonStartStopTrace->setEnabled(false);

    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
}

void MainWindow::on_pushButtonConnect_clicked()
{
    if (ui->comboBoxPort->count()>0 && ui->comboBoxPort->currentIndex()>-1){
        m_SerialPort.close();
        m_SerialPort.setPortName(ui->comboBoxPort->currentText());
        m_SerialPort.setBaudRate(QSerialPort::Baud19200);
        m_SerialPort.setParity(QSerialPort::Parity::NoParity);
        m_SerialPort.setStopBits(QSerialPort::StopBits::OneStop);
        m_SerialPort.setDataBits(QSerialPort::DataBits::Data8);
        if (m_SerialPort.open(QIODevice::ReadWrite)){
            ui->pushButtonScanPorts->setEnabled(false);
            ui->pushButtonStartStopTrace->setEnabled(true);

            ui->pushButtonConnect->setEnabled(false);
            ui->pushButtonDisconnect->setEnabled(true);

            ui->pushButtonStartStopTrace->setText(tr("Start"));

            ui->statusBar->showMessage(tr("Connected"));
        }
        else{
            QMessageBox::critical(0,"ERROR","Can't open selected port");
        }
    }
    else{
        QMessageBox::warning(0,"ERROR","Select port connected to ASN to start tracing");
    }
}

void MainWindow::on_pushButtonClearLog_clicked()
{
    ui->listWidgetLog->clear();
}

void MainWindow::onDeviceMousePressed(sTraceDevice *device)
{
    if (m_Tracing){
        ui->statusBar->showMessage(tr("Can't process operation while tracing..."),5000);
        return;
    }
    if (!m_SerialPort.isOpen()){
        ui->statusBar->showMessage(tr("Port must be opened for this operation"),5000);
        return;
    }

    if (device->address==0 || device->address==selfAddress){
        ui->statusBar->showMessage(tr("Can't edit root or routers"),5000);
        return;
    }

    m_DeviceSettingsDialog.exec(device->address);
}

void MainWindow::requestSettings(uint8_t deviceAddress)
{
    uint8_t data[7] = {0x05, deviceAddress, 0x03, 0x00, 0x31, selfAddress, 0};
    data[6] = calcSimpleChecksum(data,6);
    send(data,7);
}

void MainWindow::setSettings(uint8_t deviceAddress, sDeviceSettings settings)
{
    QByteArray raw = settings.toByteArray();
    if (raw.size()>MAX_DATA_SIZE-3){
        QMessageBox::critical(0,tr("Incorrect settings"),tr("Data size is to big, please decrease settings size"));
        return;
    }

    uint8_t header[6] = {0x05, deviceAddress, (uint8_t)(raw.size()+3), 0x00, 0x33, selfAddress};
    uint8_t checksum = calcSimpleChecksum(header,6) + calcSimpleChecksum((const uint8_t*)raw.constData(),raw.size());
    QByteArray package;
    package.append((char*)header,6);
    package.append(raw.constData(),raw.size());
    package.append((const char*)&checksum,1);
    send(package);
}

void MainWindow::setNewAddress(uint8_t deviceAddress, uint8_t deviceNewAddress)
{
    if (deviceNewAddress==0)
        return;
    sTraceDevice* device = getDeviceByAddr(deviceAddress);
    sTraceDevice* collision = getDeviceByAddr(deviceNewAddress);
    if (collision){
        QMessageBox::critical(0,tr("Collision detected"),tr("Can't set new address. Device with this address already exists in the network."));
        return;
    }
    if (device){
        uint8_t data[7] = {0x05, deviceAddress, 0x03, 0x00, 0x30, deviceNewAddress, 0};
        data[6] = calcSimpleChecksum(data,6);
        send(data,7);
        device->address = deviceNewAddress;
        ui->widget->traceResultUpdated();
        emit addressChanged(deviceAddress,deviceNewAddress);
    }
}

void MainWindow::on_pushButton_AddCustomAddress_clicked()
{
    uint8_t data[7] = {0x05, ui->spinBoxAddress->value(), 0x03, 0x00, 0x20, selfAddress, 0};
    data[6] = calcSimpleChecksum(data,6);
    send(data,7);
}
