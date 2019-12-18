#include "cscriptexecutor.h"
#include <QFile>
#include <QDebug>
#include <QQmlEngine>
#include "utils.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QandroidJniObject>
#include <QtAndroid>
#include <QtAndroidExtras>
#endif

void cScriptExecutor::sendASNPVariables(uint8_t address, QVector<sVariable> variables)
{
    QByteArray package;
    for (int i = 0; i<variables.size(); ++i)
    {
        cASNPMessageBuilder builder;
        builder.writeStart(true,false,false,address,0x00,0x00,CMD_SET_VARIABLE,0x01);
        builder.writeVariableInf(&variables[i].vi);
        switch(variables[i].vi.type)
        {
            case VT_UINT8: {builder.writeuint8(variables[i].variableValue.toInt());};break;
            case VT_UINT16: {qDebug() << "TODO";};break;
            case VT_UFIXFLOAT16: {qDebug() << "TODO";};break;
            case VT_FIXFLOAT16: {qDebug() << "TODO";};break;
            case VT_STRING16: {qDebug() << "TODO";};break;
            case VT_LINK: {qDebug() << "TODO";};break;
        }
        builder.writeEnd();
        package = package + builder.package();
    }

    m_UDPClient.sendCommand((const uint8_t*)package.constData(), package.size());
}

cScriptExecutor::cScriptExecutor():QObject()
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
    if ( activity.isValid() )
    {
        QAndroidJniObject::callStaticMethod<jboolean>
                ("link/basov/asnp/VPlay" // class name
                , "startVibrator" // method name
                , "(Landroid/app/Activity;)Z", // signature
                activity.object<jobject>());
    }
#endif

    m_ScriptEngine.globalObject().setProperty("helper",m_ScriptEngine.newQObject(this));
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    QString scripts = getAppFolder() + "scripts.mjs";
    QFile f(scripts);
    if (f.open(QIODevice::ReadOnly))
    {
        QJSValue result = m_ScriptEngine.evaluate(f.readAll());
        if (result.isError())
        {
            qDebug()
                << "Uncaught exception at line"
                << result.property("lineNumber").toInt()
                << ":" << result.toString();
        }
        f.close();
    }

}

cScriptExecutor::~cScriptExecutor()
{
}

QString cScriptExecutor::getLocalVariable(QString variableName, QString defaultValue)
{
    auto it = m_Variables.find(variableName);
    if (it==m_Variables.end())
        return defaultValue;
    return it.value();
}

void cScriptExecutor::setLocalVariable(QString variableName, QString value)
{
    m_Variables[variableName] = value;
}

void cScriptExecutor::ASNPSetVariables2Unit8(int address, QString var1Name, int var1slot, int var1value, QString var2Name, int var2slot, int var2value)
{
    QVector<sVariable> variables;
    sVariable var;
    var.variableValue = QString::number(var1value);
    StringToName(var1Name,var.vi.name);
    var.vi.slotIndex = var1slot;
    var.vi.type = VT_UINT8;
    variables.push_back(var);
    var.variableValue = QString::number(var2value);
    StringToName(var2Name,var.vi.name);
    var.vi.slotIndex = var2slot;
    var.vi.type = VT_UINT8;
    variables.push_back(var);

    sendASNPVariables(address,variables);
    sendASNPVariables(address,variables);
    sendASNPVariables(address,variables);
    sendASNPVariables(address,variables);
}

void cScriptExecutor::vibrateLong()
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject::callStaticMethod<jboolean>
            ("link/basov/progman/VPlay" // class name
            , "playLong" // method name
            , "()Z" // signature
            );
#endif
}

void cScriptExecutor::vibrateShort()
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject::callStaticMethod<jboolean>
            ("link/basov/progman/VPlay" // class name
            , "playShort" // method name
            , "()Z" // signature
            );
#endif
}

void cScriptExecutor::evalute(QString script)
{

    QJSValue result = m_ScriptEngine.evaluate(script);
    if (result.isError())
    {
        qDebug()
            << "Uncaught exception at line"
            << result.property("lineNumber").toInt()
            << ":" << result.toString();
    }
}
