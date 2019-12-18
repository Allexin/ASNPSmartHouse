#ifndef CBASEWIDGET_H
#define CBASEWIDGET_H

#include <QPainter>
#include <QMap>
#include "cscriptexecutor.h"

class cBaseWidget
{
protected:
    cBaseWidget*        m_Parent;
    bool                m_IsVisible;    //PROPERTY VISIBLE(TRUE/FALSE)
    bool                m_IsEnabled;    //PROPERTY ENABLED(TRUE/FALSE)
    QPoint              m_Position;     //PROPERTY LEFT,TOP
    QSize               m_Size;         //PROPERTY WIDTH,HEIGHT
    bool                m_FitToParent;  //PROPERTY FITTOPARENT(TRUE/FALSE)
    QVector<cBaseWidget*> m_Widgets;    //PROPERTY WIDGETS_LIST=<COUNT>

    cScriptExecutor*    m_ScriptExecutor;

    QPoint toLocal(QPoint pos);
    QString getProperty(QMap<QString, QString> &properties, const QString& name, const QString& defaultValue = "");
    bool getPropertyBool(QMap<QString, QString> &properties, const QString& name, bool defaultValue = true);
    int getPropertyInt(QMap<QString, QString> &properties, const QString& name, int defaultValue = 0);
    int getPropertyFlags(QMap<QString, QString> &properties, const QString& name, int defaultValue = 0);
public:
    cBaseWidget();
    virtual ~cBaseWidget();

    virtual void setGeometry(QPoint position, QSize size);
    virtual void draw(QPainter& p);
    virtual bool touchStart(QPoint pos);
    virtual void touchEnd();
    virtual void touchMove(QPoint pos);
    virtual void touchCancel();

    virtual void addWidget(cBaseWidget* widget);

    bool isVisible() { return m_IsVisible; }
    bool isEnabled() { return m_IsEnabled; }

    QPoint position() { return m_Position; }
    QSize size() { return m_Size; }

    void setFitToParent(bool fit);

    virtual void setProperties(QMap<QString, QString>& properties);

    void setScriptExecutor(cScriptExecutor* se);
    cScriptExecutor* getScriptExecutor();
};

#endif // CBASEWIDGET_H
