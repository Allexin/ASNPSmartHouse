#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDateTime>
#include <QMenu>
#include <QAction>

int min(int a, int b)
{
    return a<b?a:b;
}

static char m_TempBuffer[16];
//result is valid only before next call of this method
char* StringToChar16(const QString& str){
    memset(m_TempBuffer,0,16);
    QByteArray dta = str.toLatin1();
    memcpy(m_TempBuffer,dta.constData(),min(16,dta.size()));
    return m_TempBuffer;
}

uint8_t getVariableSize(eVariableType type)
{
  switch(type){
    case VT_UINT8: return sizeof(uint8_t);break;
    case VT_UINT16: return sizeof(uint16_t);break;
    case VT_UFIXFLOAT16: return sizeof(sUFixFloat);break;
    case VT_FIXFLOAT16: return sizeof(sFixFloat);break;
    case VT_STRING16: return 16;break;
    case VT_LINK: return sizeof(sVariableLink);break;
    case VT_UINT8VECTOR4:return sizeof(uint8_t)*4;break;
  }
  return 1;
}

QString typeToString(eVariableType type)
{
    switch(type){
      case VT_UINT8: return "uint8";break;
      case VT_UINT16: return "uint16";break;
      case VT_UFIXFLOAT16: return "ufixed16";break;
      case VT_FIXFLOAT16: return "fixed16";break;
      case VT_STRING16: return "string16";break;
      case VT_LINK: return "link";break;
      case VT_UINT8VECTOR4: return "uint8vector4";break;
    }
    return "unknown variable type";
}

QString valueToString(eVariableType type, QByteArray data)
{
    switch(type){
      case VT_UINT8: return QString::number(uint8_t(data[0]));break;
      case VT_UINT16: return "not implemented";break;
      case VT_UFIXFLOAT16: return "not implemented";break;
      case VT_FIXFLOAT16: return "not implemented";break;
      case VT_STRING16: return QString::fromLatin1(data);break;
      case VT_LINK: return "not implemented";break;
      case VT_UINT8VECTOR4: return QString("%1 %2 %3 %5").arg(uint8_t(data[0])).arg(uint8_t(data[1])).arg(uint8_t(data[2])).arg(uint8_t(data[3]));break;
    }
    return "unknown variable type";
}

static const uint8_t FLAG_PARITY = 1;
static const uint8_t FLAG_HIGH_PRIORITY = 2;
static const uint8_t FLAG_GROUP_ADDRESS = 4;
static const uint8_t FLAG_EVENT = 8;

static const uint8_t CMD_PING = 0x00;
static const uint8_t CMD_PONG = 0x01;
static const uint8_t CMD_REQUEST_INFO = 0x20;
static const uint8_t CMD_RESPONSE_INFO = 0x21;
static const uint8_t CMD_GET_VARIABLES_COUNT = 0x22;
static const uint8_t CMD_RESPONSE_VARIABLES_COUNT = 0x23;
static const uint8_t CMD_GET_VARIABLES = 0x24;
static const uint8_t CMD_RESPONSE_VARIABLES = 0x25;
static const uint8_t CMD_GET_VARIABLE = 0x26;
static const uint8_t CMD_RESPONSE_VARIABLE = 0x27;
static const uint8_t CMD_SET_VARIABLE = 0x28;
static const uint8_t CMD_SUBSCRIBE_TO_VARIABLE = 0x29;
static const uint8_t CMD_EVENT_VARIABLE_CHANGED = 0x2A;

static const long PACKAGE_HEADER_SIZE = 1+1+1; //PACKAGE_INFO + ADDRESS + DATA_SIZE
static const long PACKAGE_SERVICE_DATA_SIZE = PACKAGE_HEADER_SIZE+1; // + CHECKSUM
static const long MAX_PACKAGE_DATA_SIZE = 255;
static const long PACKAGE_BUFFER_SIZE = MAX_PACKAGE_DATA_SIZE + PACKAGE_SERVICE_DATA_SIZE;

void MainWindow::writeStartToTempBuffer(bool highPriority, bool groupAddress, bool event, uint8_t address, uint8_t dataLength, uint8_t cmdType, uint8_t cmd, uint8_t dataVersion)
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

void MainWindow::writeDataLengthToTempBuffer()
{
  m_SendBuffer[2] = m_SendBuffer.size() - PACKAGE_HEADER_SIZE;
}

void MainWindow::writeChecksumToTempBuffer()
{
  uint8_t checksum = 0;
  for (int i = 0; i<m_SendBuffer.size(); ++i){
    checksum += m_SendBuffer[i];
  }
  writeuint8ToTempBuffer(checksum);
}
void MainWindow::writeToTempBuffer(const uint8_t* data, uint8_t lngt)
{
  if (m_SendBuffer.size()+lngt>PACKAGE_BUFFER_SIZE)
    return;
  m_SendBuffer.append((const char*)data,lngt);
}
void MainWindow::writeuint8ToTempBuffer(uint8_t value)
{
    writeToTempBuffer(&value, 1);
}

void MainWindow::writeVariableInfoToTempBuffer(sVariableInfo *info)
{
    for (int i = 0; i<16; ++i){
        writeuint8ToTempBuffer(info->name[i]);
    }
    writeuint8ToTempBuffer(info->type);
    writeuint8ToTempBuffer(info->slotsCount);
}



void MainWindow::sendPing()
{
    //highPriority, groupAddress, event, address, dataLength, cmdType, cmd,           dataVersion
    writeStartToTempBuffer(true,        false,       false,  0x00,    0x00,       0x00,    CMD_PING,      0x01);
    //fill data
    //NO DATA FOR THIS CMD
    //~fill data
    writeDataLengthToTempBuffer();
    writeChecksumToTempBuffer();
    send(m_SendBuffer);
}

void MainWindow::sendPong()
{    
    for (int s = 0; s<4; ++s)
    {
                        //highPriority, groupAddress, event, address, dataLength, cmdType, cmd,           dataVersion
    writeStartToTempBuffer(true,        false,       false,  0x00,    0x00,       0x00,    CMD_PONG,      0x01);
    //fill data
    writeuint8ToTempBuffer(64);
    writeuint8ToTempBuffer(DEFAULT_ADDRESS);
    for (int i = 0; i<63; ++i){
        writeuint8ToTempBuffer(i+1+s*64);
    }
    writeuint8ToTempBuffer(0);
    writeuint8ToTempBuffer(0);
    //~fill data
    writeDataLengthToTempBuffer();
    writeChecksumToTempBuffer();
    send(m_SendBuffer);
    }
}

void MainWindow::sendRequestInfo(uint8_t targetAddress, bool groupAddress)
{
    //highPriority, groupAddress, event,  address,          dataLength, cmdType, cmd,                               dataVersion
    writeStartToTempBuffer(true,        groupAddress, false,  targetAddress,    0x00,       0x00,    CMD_REQUEST_INFO,                  0x01);
    //fill data
    writeuint8ToTempBuffer(DEFAULT_ADDRESS);
    //~fill data
    writeDataLengthToTempBuffer();
    writeChecksumToTempBuffer();
    send(m_SendBuffer);
}

void MainWindow::sendGetVariablesCount(uint8_t targetAddress, bool groupAddress)
{
                        //highPriority, groupAddress, event,  address,          dataLength, cmdType, cmd,                               dataVersion
    writeStartToTempBuffer(true,        groupAddress, false,  targetAddress,    0x00,       0x00,    CMD_GET_VARIABLES_COUNT,                  0x01);
    //fill data
    writeuint8ToTempBuffer(DEFAULT_ADDRESS);
    //~fill data
    writeDataLengthToTempBuffer();
    writeChecksumToTempBuffer();
    send(m_SendBuffer);
}

void MainWindow::sendGetVariables(uint8_t targetAddress, bool groupAddress, uint8_t startIndex)
{
                        //highPriority, groupAddress, event,  address,          dataLength, cmdType, cmd,                               dataVersion
    writeStartToTempBuffer(true,        groupAddress, false,  targetAddress,    0x00,       0x00,    CMD_GET_VARIABLES,                  0x01);
    //fill data
    writeuint8ToTempBuffer(DEFAULT_ADDRESS);
    writeuint8ToTempBuffer(startIndex);
    //~fill data
    writeDataLengthToTempBuffer();
    writeChecksumToTempBuffer();
    send(m_SendBuffer);
}

void MainWindow::sendGetVariable(uint8_t targetAddress, bool groupAddress, sVariableInfo *info)
{
                        //highPriority, groupAddress, event,  address,          dataLength, cmdType, cmd,                               dataVersion
    writeStartToTempBuffer(true,        groupAddress, false,  targetAddress,    0x00,       0x00,    CMD_GET_VARIABLE,                  0x01);
    //fill data
    writeuint8ToTempBuffer(DEFAULT_ADDRESS);
    writeVariableInfoToTempBuffer(info);
    //~fill data
    writeDataLengthToTempBuffer();
    writeChecksumToTempBuffer();
    send(m_SendBuffer);
}

void MainWindow::sendSetVariable(uint8_t targetAddress, bool groupAddress, sVariableInfo *info, void *variableValue)
{
                            //highPriority, groupAddress, event,  address,          dataLength, cmdType, cmd,                               dataVersion
    writeStartToTempBuffer(true,        groupAddress, false,  targetAddress,    0x00,       0x00,    CMD_SET_VARIABLE,                  0x01);
    //fill data
    writeVariableInfoToTempBuffer(info);
    writeToTempBuffer((uint8_t*)variableValue,getVariableSize(info->type));
    //~fill data
    writeDataLengthToTempBuffer();
    writeChecksumToTempBuffer();
    send(m_SendBuffer);
}

void MainWindow::send(QByteArray data)
{
    QString s = QDateTime::currentDateTime().toString("hh:mm:ss :");
    for (int i = 0; i<data.size(); ++i)
    {
        QString h = QString::number((unsigned char)(data[i]),16).toUpper();
        if (h.length()==1)
            h = "0"+h;
        s+=h+" ";
    }
    ui->listWidget_SendLogs->addItem(s);
    m_SerialPort.write(data);
}








void MainWindow::on_pushButton_ClearLogs_clicked()
{
    ui->listWidget_LogHex->clear();
    ui->listWidget_LogText->clear();
    ui->listWidget_SendLogs->clear();
}


void MainWindow::addLog(uint8_t data, bool newLine)
{
    QString hexData;
    QString textData;
    if (newLine)
    {
        ui->label_Buffer->setText("");
        hexData = textData = QDateTime::currentDateTime().toString("hh:mm:ss :");
    }

    QString h = QString::number((unsigned char)(data),16).toUpper();
    if (h.length()==1)
        h = "0"+h;
    hexData+=h+" ";

    ui->label_Buffer->setText(ui->label_Buffer->text()+h+" ");
    if (data>=0x20)
    {
        textData += QChar::fromLatin1(data);
    }

    if (newLine || ui->listWidget_LogHex->count()==0)
    {
        ui->listWidget_LogHex->addItem(hexData);
    }
    else
    {
        QListWidgetItem* item = ui->listWidget_LogHex->item(ui->listWidget_LogHex->count() - 1);
        item->setText(item->text() + hexData);
    }

    if (newLine || ui->listWidget_LogText->count()==0)
    {
        ui->listWidget_LogText->addItem(textData);
    }
    else
    {
        QListWidgetItem* item = ui->listWidget_LogText->item(ui->listWidget_LogText->count() - 1);
        item->setText(item->text() + textData);
    }
}

void MainWindow::addLogMessageType(uint8_t cmdType, uint8_t cmd)
{
    QString name;
    if (cmdType==0x00){
      switch (cmd){
        case CMD_PING:{
          name = "PING";
        };break;
        case CMD_PONG:{
          name = "PONG";
        };break;
        case CMD_REQUEST_INFO:{
          name = "REQUEST_INFO";
        };break;
        case CMD_RESPONSE_INFO:{
          name = "RESPONSE_INFO";
        };break;
        case CMD_GET_VARIABLES_COUNT:{
          name = "GET_VARIABLES_COUNT";
        };break;
        case CMD_RESPONSE_VARIABLES_COUNT:{
          name = "RESPONSE_VARIABLES_COUNT";
        };break;
        case CMD_GET_VARIABLES:{
          name = "GET_VARIABLES";
        };break;
        case CMD_RESPONSE_VARIABLES:{
          name = "RESPONSE_VARIABLES";
        };break;
        case CMD_GET_VARIABLE:{
          name = "GET_VARIABLE";
        };break;
        case CMD_RESPONSE_VARIABLE:{
          name = "RESPONSE_VARIABLE";
        };break;
        case CMD_SET_VARIABLE:{
          name = "SET_VARIABLE";
        };break;
        case CMD_SUBSCRIBE_TO_VARIABLE:{
          name = "SUBSCRIBE_TO_VARIABLE";
        };break;
        case CMD_EVENT_VARIABLE_CHANGED:{
          name = "EVENT_VARIABLE_CHANGE";
        };break;
      }
    };
    if (!name.isEmpty())
    {
        QListWidgetItem* item = ui->listWidget_LogHex->item(ui->listWidget_LogHex->count() - 1);
        item->setText(item->text() + " ["+name+"]");
    }
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&m_SerialPort,SIGNAL(readyRead()),this,SLOT(onData()));

    connect(&m_UpdatePortsTimer,SIGNAL(timeout()),this, SLOT(tryUpdatePortsList()));
    m_UpdatePortsTimer.start(1000);

    connect(&m_ResetReceiverStateTimer,SIGNAL(timeout()),this, SLOT(resetReceiverState()));
    m_ResetReceiverStateTimer.start(250);

    connect(&m_RequestInfoTimer,SIGNAL(timeout()),this, SLOT(requestDevicesInfo()));


    tryUpdatePortsList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tryUpdatePortsList()
{
    if (m_SerialPort.isOpen()){
        return;
    }

    QList<QSerialPortInfo> ports =  QSerialPortInfo::availablePorts();
    bool needUpdate = ports.size() != ui->comboBoxPort->count();
    if (!needUpdate)
    {
        for (int i = 0; i<ports.size(); ++i)
        {
            if (ui->comboBoxPort->itemText(i)!=ports[i].portName())
            {
                needUpdate = true;
                break;
            }
        }
    }

    if (!needUpdate)
    {
        return;
    }

    QString current = ui->comboBoxPort->currentText();
    ui->comboBoxPort->clear();

    for (int i = 0; i<ports.size(); i++)
    {
        ui->comboBoxPort->addItem(ports[i].portName());
    }

    for (int i = 0; i<ui->comboBoxPort->count(); ++i)
    {
        if (ui->comboBoxPort->itemText(i)==current)
        {
            ui->comboBoxPort->setCurrentIndex(i);
            break;
        }
    }
}

void MainWindow::rescan()
{
    ui->treeWidgetDevices->clear();
    m_Devices.clear();
    sendPong();
    sendPing();
}

void MainWindow::resetReceiverState()
{
    if (m_ReceiverActive)
    {
        m_ReceiverActive = false;
        return;
    }

    m_BufferState = BS_WAIT_START;
}

void MainWindow::requestDevicesInfo()
{
    if (!m_SerialPort.isOpen())
        return;
    for (int i = 0; i<m_Devices.size(); ++i){
        if (!m_Devices[i].infoRequested){
            m_Devices[i].infoRequested = true;
            sendRequestInfo(m_Devices[i].address,false);
        }
    }
}

void MainWindow::on_pushButton_Connect_clicked()
{
    if (ui->comboBoxPort->currentText().isEmpty())
    {
        return;
    }

    ui->comboBoxPort->setEnabled(false);
    ui->pushButton_Connect->setEnabled(false);
    ui->pushButton_Disconnect->setEnabled(true);

    m_SerialPort.setPortName(ui->comboBoxPort->currentText());
    m_SerialPort.setBaudRate(QSerialPort::Baud19200);
    m_SerialPort.setParity(QSerialPort::NoParity);
    m_SerialPort.setStopBits(QSerialPort::OneStop);
    m_SerialPort.setDataBits(QSerialPort::Data8);
    m_SerialPort.open(QIODevice::ReadWrite);

    rescan();

    m_RequestInfoTimer.start(100);
}

void MainWindow::on_pushButton_Disconnect_clicked()
{
    m_RequestInfoTimer.stop();

    ui->comboBoxPort->setEnabled(true);
    ui->pushButton_Connect->setEnabled(true);
    ui->pushButton_Disconnect->setEnabled(false);

    m_SerialPort.close();
}




void MainWindow::onData()
{
    m_ReceiverActive = true;
    QByteArray data = m_SerialPort.readAll();
    for (int i = 0; i<data.size(); ++i)
    {
        processChar(data[i]);
    }
}



void MainWindow::processChar(uint8_t data)
{
    static const uint8_t MAX_PACKAGE_DATA_SIZE = 255;
    addLog(data, m_BufferState==BS_WAIT_START);

    switch(m_BufferState){
    case BS_WAIT_START:
    {
        addToBuffer(data,true);
        m_BufferState = BS_WAIT_ADDRESS;
    };break;
    case BS_WAIT_ADDRESS:
    {
        addToBuffer(data,false);
        m_BufferState = BS_WAIT_LENGTH;
    };break;
    case BS_WAIT_LENGTH:
    {
        int dataLength = data; //to avoid warning
        if (dataLength<=MAX_PACKAGE_DATA_SIZE)
        {
            addToBuffer(data,false);
            m_BufferState = BS_WAIT_CHECKSUM;
        }
        else{
          //incorrect data, wait next package
          m_BufferState = BS_WAIT_START;
        }
    };break;
    case BS_WAIT_CHECKSUM:
    {
        uint8_t currentChecksum = m_Checksum;
        addToBuffer(data,false);
        if (m_Buffer.at(LENGTH_POS)==m_Buffer.size()-LENGTH_POS-2)
        {
            m_BufferState = BS_WAIT_START;
            if (data==currentChecksum)
            {
                if (m_Buffer.at(LENGTH_POS)>=3)
                {
                    addLogMessageType(m_Buffer[DATA_START_POS],m_Buffer[DATA_START_POS+1]);
                    processPackage();
                }
            }
            else
            {
                //error - incorrect checksum
            }
        }
    };break;
    }
}

void MainWindow::addToBuffer(uint8_t data, bool clear)
{
    if (clear)
    {
        m_Buffer.clear();
        m_Checksum = 0;
    }
    m_Buffer.push_back(data);
    m_Checksum += data;
}

void MainWindow::processPackage()
{
    uint8_t info = m_Buffer[INFO_POS];
    uint8_t address = m_Buffer[ADDRESS_POS];
    uint8_t dataLength = m_Buffer[LENGTH_POS];
    char* data = &m_Buffer.data()[DATA_START_POS];
    bool highPriority = (info & FLAG_HIGH_PRIORITY) == FLAG_HIGH_PRIORITY;
    bool isGroupAddress = (info & FLAG_GROUP_ADDRESS) == FLAG_GROUP_ADDRESS;
    bool isEvent = (info & FLAG_EVENT) == FLAG_EVENT;
    bool isBroadcast = address == 0x00;
    if (isEvent)
    {

    }
    else
    {
        processMessage(dataLength, data);
    }
}

uint8_t MainWindow::takeuint8()
{
    uint8_t value = m_DataBuffer[0];
    m_DataBuffer.remove(0,1);
    return value;
}

QString MainWindow::takeString()
{
    QByteArray data;
    data.resize(takeuint8());
    for (int i = 0; i<data.size(); ++i)
    {
        data[i] = takeuint8();
    }
    return QString::fromLatin1(data);
}

sVariableInfo MainWindow::takeVariableInfo()
{
    sVariableInfo info;
    for (int i = 0; i<16; ++i){
        info.name[i] = takeuint8();
    }
    info.type = (eVariableType)takeuint8();
    info.slotsCount = takeuint8();
    return info;
}

void MainWindow::processMessage(uint8_t dataLength, char *data)
{
    m_DataBuffer.clear();
    m_DataBuffer.append(data,dataLength);


    uint8_t cmdType =takeuint8();
    bool isSysCommand = cmdType==0x00;
    uint8_t command = takeuint8();
    uint8_t version = takeuint8();

    if (isSysCommand){
      switch (command){
        case CMD_PING:{
          sendPong();
        };break;
        case CMD_PONG:{
          //set routes
          parsePong();
        };break;
        case CMD_REQUEST_INFO:{
          //do nothing
        };break;
        case CMD_RESPONSE_INFO:{
          parseResponseInfo();
        };break;
        case CMD_GET_VARIABLES_COUNT:{
          //do nothing
        };break;
        case CMD_RESPONSE_VARIABLES_COUNT:{
          parseVariablesCount();
        };break;
        case CMD_GET_VARIABLES:{
          //do nothing
        };break;
        case CMD_RESPONSE_VARIABLES:{
          parseResponseVariables();
        };break;
        case CMD_GET_VARIABLE:{
          //do nothing
        };break;
        case CMD_RESPONSE_VARIABLE:{
          parseResponseVariable();
        };break;
        case CMD_SET_VARIABLE:{
          //do nothing
        };break;
        case CMD_SUBSCRIBE_TO_VARIABLE:{
          //do nothing
        };break;
        case CMD_EVENT_VARIABLE_CHANGED:{
          //do nothing
        };break;
      }
    };
}

void MainWindow::parsePong()
{
    if (takeuint8()==0x00)
        return;
    addDevice(takeuint8());
}

void MainWindow::parseResponseInfo()
{
    uint8_t deviceAddress = takeuint8();
    QString deviceType = takeString();
    QString deviceDescription = takeString();
    updateDeviceInfo(deviceAddress,deviceType,deviceDescription);
}

void MainWindow::parseVariablesCount()
{
    uint8_t deviceAddress = takeuint8();
    uint8_t variablesCount = takeuint8();
    uint8_t requestedCount = 0;
    while (requestedCount<variablesCount)
    {
        sendGetVariables(deviceAddress,false,requestedCount);
        requestedCount+=8;
    }
}

void MainWindow::parseResponseVariables()
{
    uint8_t deviceAddress = takeuint8();
    uint8_t startIndex = takeuint8();
    uint8_t lastIndex = takeuint8();
    for (int i = startIndex; i<=lastIndex; ++i)
    {
        sVariableInfo vi = takeVariableInfo();
        int slotsCount = vi.slotsCount;
        for (int j = 0; j<slotsCount; ++j)
        {
            vi.slotsCount = j;
            sendGetVariable(deviceAddress, false,&vi);
        }
    }
}

void MainWindow::parseResponseVariable()
{
    uint8_t deviceAddress = takeuint8();

    sDeviceVariable dv;
    dv.info = takeVariableInfo();
    dv.value.resize(getVariableSize(dv.info.type));
    for (int i = 0; i<dv.value.size();++i)
    {
        dv.value[i] = takeuint8();
    }
    updateDeviceVariable(deviceAddress,dv);
}

int MainWindow::gedDeviceIndex(uint8_t address)
{
    for (int i = 0; i<m_Devices.size(); ++i)
    {
        if (m_Devices[i].address==address)
        {
            return i;
        }
    }
    return -1;
}

void MainWindow::addDevice(uint8_t address)
{
    if(address==0)
        return;

    if (gedDeviceIndex(address)>-1)
        return;

    sDeviceInfo info;
    info.address = address;
    info.deviceType = "RECEIVING";
    info.deviceDescription = "RECEIVING";
    info.infoRequested = false;
    m_Devices.push_back(info);
    updateDevicesList();
}

void MainWindow::updateDeviceInfo(uint8_t deviceAddress, QString deviceType, QString deviceDescription)
{
    int index = gedDeviceIndex(deviceAddress);
    if (index==-1)
        return;
    m_Devices[index].deviceType = deviceType;
    m_Devices[index].deviceDescription = deviceDescription;
    updateDevicesList();
}

void MainWindow::updateDeviceVariable(uint8_t deviceAddress, sDeviceVariable variable)
{
    int index = gedDeviceIndex(deviceAddress);
    if (index==-1)
        return;
    int varIndex = -1;
    for (int i = 0; i<m_Devices[index].variables.size(); ++i){
        if (memcmp(m_Devices[index].variables[i].info.name,variable.info.name,16)==0 && m_Devices[index].variables[i].info.type==variable.info.type && m_Devices[index].variables[i].info.slotsCount == variable.info.slotsCount)
        {
            varIndex = i;
            break;
        }
    }

    if (varIndex>-1)
    {
        m_Devices[index].variables[varIndex] = variable;
    }
    else
    {
        m_Devices[index].variables.push_back(variable);
    }
    updateDevicesList();
}

void MainWindow::updateDevicesList()
{
    ui->treeWidgetDevices->clear();
    for(int i = 0; i<m_Devices.size(); ++i)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetDevices);
        item->setText(0,QString::number(m_Devices[i].address));
        item->setText(1,m_Devices[i].deviceType);
        item->setText(2,m_Devices[i].deviceDescription);
        item->setData(0,Qt::UserRole,i);

        for (int j = 0; j<m_Devices[i].variables.size(); ++j)
        {
            sDeviceVariable& v = m_Devices[i].variables[j];
            QTreeWidgetItem* var = new QTreeWidgetItem(item);
            var->setText(3,QString(v.info.name)+"["+QString::number(v.info.slotsCount)+"]");
            var->setText(4,typeToString(v.info.type));
            var->setText(5,valueToString(v.info.type,v.value));
            item->addChild(var);

            var->setData(0,Qt::UserRole,i);
            var->setData(3,Qt::UserRole,j);
        }

        ui->treeWidgetDevices->addTopLevelItem(item);

    }
}



void MainWindow::on_treeWidgetDevices_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* item = ui->treeWidgetDevices->itemAt(pos);
    if (!item)
        return;
    QMenu * menu = new QMenu(this);
    if (item->data(0,Qt::UserRole).canConvert(QMetaType::Int))
    {
        {
            QAction* a = new QAction("Request variables", this);
            a->setData(item->data(0,Qt::UserRole));
            connect(a,SIGNAL(triggered()),this,SLOT(actionRequestVariables()));
            menu->addAction(a);
        }
        if (item->data(3,Qt::UserRole).canConvert(QMetaType::Int))
        {
            QAction* a = new QAction("Change variable value", this);
            a->setData(QPoint(item->data(0,Qt::UserRole).toInt(),item->data(3,Qt::UserRole).toInt()));
            connect(a,SIGNAL(triggered()),this,SLOT(actionSetVariable()));
            menu->addAction(a);
        }
    }
    if (menu->actions().size()>0)
        menu->popup(ui->treeWidgetDevices->viewport()->mapToGlobal(pos));
}

void MainWindow::actionRequestVariables()
{
    if (QAction* action = qobject_cast<QAction*>(sender()))
    {
        sendGetVariablesCount(m_Devices[action->data().toInt()].address,false);
    }
}

void MainWindow::actionSetVariable()
{
    if (QAction* action = qobject_cast<QAction*>(sender()))
    {
        QPoint p = action->data().toPoint();
        uint8_t address = m_Devices[p.x()].address;
        sDeviceVariable* var = &m_Devices[p.x()].variables[p.y()];

        if (m_DialogVariableInput.exec(var)==QDialog::Accepted)
        {
            sendSetVariable(address,false,&var->info,var->value.data());
            if (memcmp(var->info.name,StringToChar16("ADDRESS"),16)==0)
            {
                rescan();
            }
            else{
                sendGetVariable(address,false,&var->info);
            }
        }
    }
}
