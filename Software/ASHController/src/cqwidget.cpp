#include "cqwidget.h"

#include "widgets/cwidgetpages.h"

cQWidget::cQWidget(QWidget *parent) :
    QWidget(parent)
{
    cBaseWidget* base = new cBaseWidget();
    m_LowLevelWidgets.push_back(base);
}

cQWidget::~cQWidget()
{
    for (int i = 0; i<m_LowLevelWidgets.size(); ++i)
    {
        delete m_LowLevelWidgets[i];
    }
}

void cQWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    if (size()!=m_CachedSize)
    {
        m_CachedSize = size();
        for (int i = 0; i<m_LowLevelWidgets.size(); ++i)
        {
            m_LowLevelWidgets[i]->setGeometry(QPoint(0,0), m_CachedSize);
        }
    }

    for (int i = 0; i<m_LowLevelWidgets.size(); ++i)
    {
        if (m_LowLevelWidgets[i]->isVisible())
        {
            m_LowLevelWidgets[i]->draw(p);
        }
    }
}

void cQWidget::mousePressEvent(QMouseEvent *eventPress)
{
    for (int i = m_LowLevelWidgets.size() - 1; i>=0; --i)
    {
        if (m_LowLevelWidgets[i]->isVisible() && m_LowLevelWidgets[i]->isEnabled())
        {
            if (m_LowLevelWidgets[i]->touchStart(eventPress->pos()))
            {
                update();
                return;
            }
        }
    }
    update();
}

void cQWidget::mouseMoveEvent(QMouseEvent *eventPress)
{
    for (int i = 0; i<m_LowLevelWidgets.size(); ++i)
    {
        if (m_LowLevelWidgets[i]->isVisible() && m_LowLevelWidgets[i]->isEnabled())
        {
            m_LowLevelWidgets[i]->touchMove(eventPress->pos());
        }
    }
    update();
}

void cQWidget::mouseReleaseEvent(QMouseEvent *eventPress)
{
    for (int i = 0; i<m_LowLevelWidgets.size(); ++i)
    {
        if (m_LowLevelWidgets[i]->isVisible() && m_LowLevelWidgets[i]->isEnabled())
        {
            m_LowLevelWidgets[i]->touchEnd();
        }
    }
    update();
}

void cQWidget::addWidget(cBaseWidget *widget, cScriptExecutor* se)
{
    m_LowLevelWidgets.push_back(widget);
    widget->setScriptExecutor(se);
}

cBaseWidget *cQWidget::getBaseLayout()
{
    return m_LowLevelWidgets[0];
}
