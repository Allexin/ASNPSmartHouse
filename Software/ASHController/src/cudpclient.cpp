#include "cudpclient.h"

#include <QDataStream>

static const QString PHEADER_TRANSMIT_ASNP_COMMAND = "ASNP";  //DATA == ASNP_RAW_COMMAND
static const QString PHEADER_REQUEST_DEVICES = "RDVS"; //DATA are empty
static const QString PHEADER_DEVICES_LIST = "DVSL"; //DATA = byte count, [byte address, byte id_length, string id]

int getInt(QByteArray data, int pos){
    QDataStream ds(data.mid(pos));
    ds.setByteOrder(QDataStream::LittleEndian);
    int value;
    ds >> value;
    return value;
}

void writeInt(QByteArray& data, int value){
    data.append(4,0);
    QDataStream ds(&data,QIODevice::ReadWrite);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds.skipRawData(data.size()-4);
    ds << value;
}
void writeByte(QByteArray& data, uint8_t value){
    data.append((char*)&value,1);
}

void writeString(QByteArray& data, QString& value){
    QByteArray string = value.toUtf8();
    uint8_t size = string.length();
    if (string.length()>255){
        size = 255;
    }
    writeByte(data,size);
    data.append(string);
}

int calcChecksum(QByteArray& data, int length = -1){
    if (length==-1)
        length = data.size();
    int checksum = 0;
    for (int i = 0; i<length; ++i){
        checksum+=uint8_t(data[i]);
    }
    return checksum;
}

bool isChecksumValid(QByteArray data){
    return getInt(data,data.size()-4)==calcChecksum(data,data.size()-4);
}

cUDPClient::cUDPClient():QObject()
{
    m_ServerAddress = QHostAddress("192.168.1.95");
    m_ServerPort = 25855;

    m_Socket.bind(QHostAddress::Any);
    connect(&m_Socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
}

void cUDPClient::sendCommand(QByteArray data)
{
    QByteArray ba;
    ba.append(PHEADER_TRANSMIT_ASNP_COMMAND.toUtf8().mid(0,4));
    ba.append(data);
    writeInt(ba, calcChecksum(ba));
    m_Socket.writeDatagram(ba, m_ServerAddress, m_ServerPort);
}

void cUDPClient::sendCommand(const uint8_t* package, int length)
{
    QByteArray ba;
    ba.append((char*)package, length);
    sendCommand(ba);
}

void cUDPClient::readyRead()
{
    QByteArray buffer;
    buffer.resize(static_cast<int>(m_Socket.pendingDatagramSize()));

    QHostAddress sender;
    quint16 senderPort;

    m_Socket.readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
    if (buffer.size()>=8){
        if (isChecksumValid(buffer)){
            QString header = QString::fromUtf8(buffer.mid(0,4));
        }
    }
}
