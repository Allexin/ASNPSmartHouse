#include "cwidgetpages.h"
#include <QDebug>

void cWidgetPages::resetToCurrentPage()
{
    m_Scroll = 0;
}

cWidgetPages::cWidgetPages():cBaseWidget(),
    m_CurrentPage(0),
    m_Scroll(0),
    m_Direction(VERTICAL)
{

}

void cWidgetPages::setGeometry(QPoint position, QSize size)
{
    cBaseWidget::setGeometry(position,size);
    for (int i = 0; i<m_Widgets.size(); ++i)
    {
        m_Widgets[i]->setGeometry(m_Direction==VERTICAL?QPoint(0,i*m_Size.height()):QPoint(i*m_Size.width(),0),m_Widgets[i]->size());
    }
    resetToCurrentPage();
}

void cWidgetPages::draw(QPainter &p)
{
    if (m_CurrentPage>=m_Widgets.size())
    {
        m_CurrentPage = m_Widgets.size() - 1;
    }
    if (m_CurrentPage<0)
    {
        m_CurrentPage = 0;
    }

    QPoint shift;
    if (m_Direction==VERTICAL)
    {
        shift.setY(-m_CurrentPage*m_Size.height()+m_Scroll);
    }
    else
    {
        shift.setX(-m_CurrentPage*m_Size.width()+m_Scroll);
    }

    p.setClipping(true);
    for (int i = qMax(0,m_CurrentPage-1); i<=qMin(m_CurrentPage+1,m_Widgets.size()-1); ++i)
    {
        if (m_Widgets[i]->isVisible())
        {
            p.save();
            p.translate(m_Widgets[i]->position()+shift);
                p.setClipRect(QRect(QPoint(0,0),m_Widgets[i]->size()), Qt::ReplaceClip);
                m_Widgets[i]->draw(p);
            p.restore();
        }
    }
    p.setClipping(false);
}

bool cWidgetPages::touchStart(QPoint pos)
{
    QPoint shift;
    if (m_Direction==VERTICAL)
    {
        shift.setY(m_CurrentPage*m_Size.height()+m_Scroll);
    }
    else
    {
        shift.setX(m_CurrentPage*m_Size.width()+m_Scroll);
    }

    cBaseWidget::touchStart(pos+shift);
    m_ScrollDist = 0;
    m_Scrolling = true;
    m_TouchPos = pos;
    return true;
}

void cWidgetPages::touchEnd()
{
    cBaseWidget::touchEnd();
    if (m_Scrolling)
    {
        if (qAbs(m_Scroll)>(m_Direction==VERTICAL?m_Size.height():m_Size.width())/2)
        {
            if (m_Scroll<0)
            {
                m_CurrentPage++;
            }
            else{
                m_CurrentPage--;
            }
        }
        resetToCurrentPage();
    }
}

void cWidgetPages::touchMove(QPoint pos)
{
    QPoint shift;
    if (m_Direction==VERTICAL)
    {
        shift.setY(m_CurrentPage*m_Size.height()+m_Scroll);
    }
    else
    {
        shift.setX(m_CurrentPage*m_Size.width()+m_Scroll);
    }
    cBaseWidget::touchMove(pos+shift);
    if (m_Scrolling)
    {
        int diff;
        if (m_Direction==VERTICAL)
        {
            diff = pos.y() - m_TouchPos.y();
        }
        else
        {
            diff = pos.x() - m_TouchPos.x();
        }
        m_Scroll+=diff;
        m_ScrollDist+=abs(diff);

        if (m_ScrollDist>MAX_SCROLL_TO_CANCEL_TOUCH)
        {
            for (int i = 0; i<m_Widgets.size(); ++i)
            {
                m_Widgets[i]->touchCancel();
            }
        }
    }
    m_TouchPos = pos;
}

void cWidgetPages::touchCancel()
{
    cBaseWidget::touchCancel();
    m_Scrolling = false;
    resetToCurrentPage();
}

void cWidgetPages::addWidget(cBaseWidget *widget)
{
    cBaseWidget::addWidget(widget);
    setGeometry(m_Position,m_Size);
}

void cWidgetPages::setDirection(cWidgetPages::eDirection direction)
{
    m_Direction = direction;
}

void cWidgetPages::setProperties(QMap<QString, QString> &properties)
{
    cBaseWidget::setProperties(properties);
    QString value = getProperty(properties,"DIRECTION","");
    if (value.toUpper()=="VERTICAL")
    {
        m_Direction = VERTICAL;
    }
    if (value.toUpper()=="HORIZONTAL")
    {
        m_Direction = HORIZONTAL;
    }
    m_CurrentPage = getPropertyInt(properties,"DEFAULTPAGE",0);
}
