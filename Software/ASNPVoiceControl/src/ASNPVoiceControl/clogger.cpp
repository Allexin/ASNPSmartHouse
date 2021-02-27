#include "clogger.h"

#include <QDebug>

void logError(QString tag, QString info)
{
    qCritical() << tag <<info;
}

void logVerbose(QString tag, QString info)
{
    qInfo() << tag << info;
}
