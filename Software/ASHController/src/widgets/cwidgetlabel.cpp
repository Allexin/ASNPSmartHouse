#include "cwidgetlabel.h"

cWidgetLabel::cWidgetLabel():cBaseWidget(),
    m_Flags(Qt::AlignCenter),
    m_Text("")
{

}

void cWidgetLabel::draw(QPainter &p)
{
    p.drawText(QRect(QPoint(0,0),m_Size),m_Flags,m_Text);
    cBaseWidget::draw(p);
}

void cWidgetLabel::setText(QString text)
{
    m_Text = text;
}

QString cWidgetLabel::text()
{
    return m_Text;
}

void cWidgetLabel::setFlags(int flags)
{
    m_Flags = flags;
}

int cWidgetLabel::flags()
{
    return m_Flags;
}

void cWidgetLabel::setProperties(QMap<QString, QString> &properties)
{
    cBaseWidget::setProperties(properties);
    m_Text = getProperty(properties,"TEXT",m_Text);
    m_Flags = getPropertyFlags(properties,"FLAGS",m_Flags);
}
