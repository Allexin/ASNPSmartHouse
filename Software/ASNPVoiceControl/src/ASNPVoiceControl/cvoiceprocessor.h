#ifndef CVOICEPROCESSOR_H
#define CVOICEPROCESSOR_H

#include <QProcess>
#include <QTimer>
#include "cparsetree.h"


class cVoiceProcessor: public QObject
{
    Q_OBJECT
protected:
    static constexpr float RESET_PARSER_TIME = 5.f;
    static constexpr float PAUSE_TIME = 2.f;
    cParseTree*         m_Parser;

    float               m_PrevWordTime;

    QProcess            m_VoskProcess;
    void processing(QByteArray& json);
public:
    cVoiceProcessor(cParseTree* parseTree);

    void start();
private slots:
    void readOutput();
};

#endif // CVOICEPROCESSOR_H
