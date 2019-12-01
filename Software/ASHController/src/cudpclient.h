#ifndef CUDPCLIENT_H
#define CUDPCLIENT_H
#include <QUdpSocket>
#include <QObject>

class cUDPClient: public QObject
{
    Q_OBJECT
protected:
    QHostAddress        m_ServerAddress;
    quint16             m_ServerPort;

    QUdpSocket          m_Socket;
public:
    cUDPClient();
public slots:
    void sendCommand(QByteArray data);
    void sendCommand(const uint8_t* package, int length);
private slots:
    void readyRead();
};

#endif // CUDPCLIENT_H
