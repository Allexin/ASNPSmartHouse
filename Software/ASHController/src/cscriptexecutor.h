#ifndef CSCRIPTEXECUTOR_H
#define CSCRIPTEXECUTOR_H

#include <QObject>
#include <QJSEngine>
#include "cudpclient.h"
#include <asnp_utils.h>

struct sVariable{
  sVariableInfo vi;
  QString variableValue;
};

class cScriptExecutor: public QObject
{
    Q_OBJECT
protected:
    QMap<QString, QString> m_Variables;
    QJSEngine           m_ScriptEngine;
    cUDPClient          m_UDPClient;

    void sendASNPVariables(uint8_t address, QVector<sVariable> variables);
public:
    cScriptExecutor();
    ~cScriptExecutor();

    void evalute(QString script);
public slots:
    QString getLocalVariable(QString variableName, QString defaultValue = "");
    void setLocalVariable(QString variableName, QString value);

    void ASNPSetVariables2Unit8(int address, QString var1Name, int var1slot, int var1value, QString var2Name, int var2slot, int var2value);

    void vibrateLong();
    void vibrateShort();
};

#endif // CSCRIPTEXECUTOR_H
