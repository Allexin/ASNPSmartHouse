#include "cvoiceprocessor.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qfile.h>
#include <iostream>
#include <windows.h>

/*
{
  "result" : [{
      "conf" : 1.000000,
      "end" : 6.990000,
      "start" : 6.540000,
      "word" : "привет"
    }, {
      "conf" : 1.000000,
      "end" : 7.410000,
      "start" : 6.990000,
      "word" : "пока"
    }],
  "text" : "привет пока"
}
*/

void cVoiceProcessor::processing(QByteArray &json)
{
    QJsonParseError error;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(json,&error);
    //qDebug() << error.errorString();
    QJsonObject jsonObject = jsonResponse.object();
    if (!jsonObject.contains("result"))
    {
        return;
    }

    QJsonArray jsonArray = jsonObject["result"].toArray();

    QString fullString;

    foreach (const QJsonValue & value, jsonArray) {
        QJsonObject obj = value.toObject();
        QString word = obj["word"].toString();
        if (m_Parser->isValidWord(word))
        {
            float start = obj["start"].toDouble();
            float end = obj["start"].toDouble();
            if (start-m_PrevWordTime>=RESET_PARSER_TIME)
            {
                m_Parser->resetParsing();
                fullString+="[RESET]";
            }
            bool longPause = start-m_PrevWordTime>=PAUSE_TIME;
            m_PrevWordTime = end;

            m_Parser->parseWithNextWord(word,longPause);

            fullString+=("["+word+"] ");
        }
        else
        {
            fullString+=(word+" ");
        }
    }    
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),
          fullString.utf16(), fullString.size(), NULL, NULL);
}

cVoiceProcessor::cVoiceProcessor(cParseTree *parseTree):QObject(nullptr),
    m_Parser(parseTree)
{
    connect(&m_VoskProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    m_PrevWordTime = 0;
}

void cVoiceProcessor::start()
{
    qDebug()<<"STARTED";
    m_VoskProcess.start("py",QStringList() << "voicecapture.py");
}

void cVoiceProcessor::readOutput()
{
    QByteArray outData = m_VoskProcess.readAllStandardOutput();
    processing(outData);
    //qDebug()<<QString::fromUtf8(outData);
}
