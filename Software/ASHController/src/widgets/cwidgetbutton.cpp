#include "cwidgetbutton.h"
#include "../utils.h"

cWidgetButton::cWidgetButton():cBaseWidget(),
    m_IsPressed(false)
{

}

void cWidgetButton::draw(QPainter &p)
{
    if (m_IsPressed && m_IsUnderCursor)
    {
        p.drawPixmap(QRect(QPoint(0,0),m_Size),m_Pressed);
    }
    else
    {
        p.drawPixmap(QRect(QPoint(0,0),m_Size),m_Normal);
    }

    cBaseWidget::draw(p);
}

bool cWidgetButton::touchStart(QPoint pos)
{
    cBaseWidget::touchStart(pos);
    m_IsPressed = m_IsUnderCursor = true;
    return true;
}

void cWidgetButton::touchEnd()
{
    cBaseWidget::touchEnd();
    if (m_IsPressed && m_IsUnderCursor)
    {
        if (!m_OnClickScript.isEmpty())
        {
            if (auto se = getScriptExecutor())
            {
                se->evalute(m_OnClickScript);
            }
        }
    }
    m_IsPressed = false;
}

void cWidgetButton::touchMove(QPoint pos)
{
    cBaseWidget::touchMove(pos);
    m_IsUnderCursor = QRect(QPoint(0,0), m_Size).contains(pos);
}

void cWidgetButton::touchCancel()
{
    cBaseWidget::touchCancel();
    m_IsPressed = false;
}

void cWidgetButton::setProperties(QMap<QString, QString> &properties)
{
    cBaseWidget::setProperties(properties);

    QString fileName = getProperty(properties,"NORMALIMAGEFILENAME","");
    if (!fileName.isEmpty())
    {
        setNormalImage(getAppFolder()+fileName);
    }
    fileName = getProperty(properties,"PRESSEDIMAGEFILENAME","");
    if (!fileName.isEmpty())
    {
        setPressedImage(getAppFolder()+fileName);
    }

    m_OnClickScript = getProperty(properties,"ONCLICK",m_OnClickScript);
}

void cWidgetButton::setOnClickScript(QString value)
{
    m_OnClickScript = value;
}

void cWidgetButton::setNormalImage(QString fileName)
{
    m_Normal = QPixmap(fileName);
}

void cWidgetButton::setPressedImage(QString fileName)
{
    m_Pressed = QPixmap(fileName);
}
