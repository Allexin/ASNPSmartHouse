#include "cbasewidget.h"
#include <QDebug>

QPoint cBaseWidget::toLocal(QPoint pos)
{
    return QPoint(pos.x()-m_Position.x(),pos.y()-m_Position.y());
}

cBaseWidget::cBaseWidget():
    m_Parent(nullptr),
    m_IsVisible(true),
    m_IsEnabled(true),
    m_FitToParent(false),
    m_ScriptExecutor(nullptr)
{

}

cBaseWidget::~cBaseWidget()
{
    for (int i = 0; i<m_Widgets.size(); ++i)
    {
        delete m_Widgets[i];
    }
}

void cBaseWidget::setGeometry(QPoint position, QSize size)
{
    m_Position = position;
    m_Size = size;
    for (int i = 0; i<m_Widgets.size(); ++i)
    {
        if (m_Widgets[i]->m_FitToParent)
        {
            m_Widgets[i]->setGeometry(QPoint(0,0),m_Size);
        }
    }
}
void cBaseWidget::draw(QPainter& p)
{    
    p.setClipping(true);
    for (int i = 0; i<m_Widgets.size(); ++i)
    {
        if (m_Widgets[i]->isVisible())
        {
            p.save();
            p.translate(m_Widgets[i]->m_Position);
                p.setClipRect(QRect(QPoint(0,0),m_Widgets[i]->m_Size), Qt::ReplaceClip);
                m_Widgets[i]->draw(p);
            p.restore();
        }
    }    
    p.setClipping(false);
}
bool cBaseWidget::touchStart(QPoint pos)
{    
    for (int i = m_Widgets.size() - 1; i>=0; --i)
    {
        if (m_Widgets[i]->isVisible() && m_Widgets[i]->isEnabled())
        {
            if (QRect(m_Widgets[i]->position(),m_Widgets[i]->size()).contains(pos))
            {
                if (m_Widgets[i]->touchStart(m_Widgets[i]->toLocal(pos)))
                {
                    return true;
                }
            }
        }
    }
    return false;
}
void cBaseWidget::touchEnd()
{
    for (int i = 0; i<m_Widgets.size(); ++i)
    {
        if (m_Widgets[i]->isVisible() && m_Widgets[i]->isEnabled())
        {
            m_Widgets[i]->touchEnd();
        }
    }
}
void cBaseWidget::touchMove(QPoint pos)
{
    for (int i = 0; i<m_Widgets.size(); ++i)
    {
        if (m_Widgets[i]->isVisible() && m_Widgets[i]->isEnabled())
        {
            m_Widgets[i]->touchMove(m_Widgets[i]->toLocal(pos));
        }
    }
}
void cBaseWidget::touchCancel()
{
    for (int i = 0; i<m_Widgets.size(); ++i)
    {
        if (m_Widgets[i]->isVisible() && m_Widgets[i]->isEnabled())
        {
            m_Widgets[i]->touchCancel();
        }
    }
}

void cBaseWidget::addWidget(cBaseWidget* widget)
{
    m_Widgets.push_back(widget);
    widget->m_Parent = this;
    if (widget->m_FitToParent)
    {
        widget->setGeometry(QPoint(0,0), m_Size);
    }
}

void cBaseWidget::setFitToParent(bool fit)
{
    m_FitToParent = fit;
}

QString cBaseWidget::getProperty(QMap<QString, QString> &properties, const QString& name, const QString& defaultValue)
{
    auto it = properties.find(name);
    if (it==properties.end())
    {
        return defaultValue;
    }
    return it.value();
}

bool cBaseWidget::getPropertyBool(QMap<QString, QString> &properties, const QString &name, bool defaultValue)
{
    QString defVal = defaultValue?"true":"false";
    QString value = getProperty(properties,name,defVal);
    return value.toUpper()=="TRUE";
}

int cBaseWidget::getPropertyInt(QMap<QString, QString> &properties, const QString &name, int defaultValue)
{
    QString value = getProperty(properties,name,QString::number(defaultValue));
    return value.toInt();
}

int cBaseWidget::getPropertyFlags(QMap<QString, QString> &properties, const QString &name, int defaultValue)
{
    if (!properties.contains(name))
        return defaultValue;

    QString value = getProperty(properties,name,"");
    QStringList flagsList = value.split("|");
    int flags = 0;
    for (int i = 0; i<flagsList.size(); ++i)
    {
        QString flag = flagsList[i].trimmed().toUpper();
        if (flag=="ALIGNLEFT")
            flags |= Qt::AlignLeft;
        else
        if (flag=="ALIGNRIGHT")
            flags |= Qt::AlignRight;
        else
        if (flag=="ALIGNHCENTER")
            flags |= Qt::AlignHCenter;
        else
        if (flag=="ALIGNJUSTIFY")
            flags |= Qt::AlignJustify;
        else
        if (flag=="ALIGNTOP")
            flags |= Qt::AlignTop;
        else
        if (flag=="ALIGNBOTTOM")
            flags |= Qt::AlignBottom;
        else
        if (flag=="ALIGNVCENTER")
            flags |= Qt::AlignVCenter;
        else
        if (flag=="ALIGNBASELINE")
            flags |= Qt::AlignBaseline;
        else
        if (flag=="ALIGNCENTER")
            flags |= Qt::AlignCenter;
        else
        if (flag=="TEXTSINGLELINE")
            flags |= Qt::TextSingleLine;
        else
        if (flag=="TEXTDONTCLIP")
            flags |= Qt::TextDontClip;
        else
        if (flag=="TEXTEXPANDTABS")
            flags |= Qt::TextExpandTabs;
        else
        if (flag=="TEXTSHOWMNEMONIC")
            flags |= Qt::TextShowMnemonic;
        else
        if (flag=="TEXTWORDWRAP")
            flags |= Qt::TextWordWrap;
        else
        if (flag=="TEXTWRAPANYWHERE")
            flags |= Qt::TextWrapAnywhere;
        else
        if (flag=="TEXTHIDEMNEMONIC")
            flags |= Qt::TextHideMnemonic;
        else
        if (flag=="TEXTDONTPRINT")
            flags |= Qt::TextDontPrint;
        else
        if (flag=="TEXTINCLUDETRAILINGSPACES")
            flags |= Qt::TextIncludeTrailingSpaces;
        else
        if (flag=="TEXTJUSTIFICATIONFORCED")
            flags |= Qt::TextJustificationForced;
        else
            qWarning() <<("unknown flag \""+flagsList[i].trimmed()+"\"");
    }
    return flags;
}

void cBaseWidget::setProperties(QMap<QString, QString> &properties)
{
    m_IsVisible = getPropertyBool(properties,"VISIBLE",m_IsVisible);
    m_IsEnabled = getPropertyBool(properties,"ENABLED",m_IsEnabled);
    m_FitToParent = getPropertyBool(properties,"FITTOPARENT",m_FitToParent);
    m_Position.setX(getPropertyInt(properties,"LEFT",m_Position.x()));
    m_Position.setY(getPropertyInt(properties,"TOP",m_Position.y()));
    m_Size.setWidth(getPropertyInt(properties,"WIDTH",m_Size.width()));
    m_Size.setHeight(getPropertyInt(properties,"HEIGHT",m_Size.height()));
}

void cBaseWidget::setScriptExecutor(cScriptExecutor *se)
{
    m_ScriptExecutor = se;
}

cScriptExecutor *cBaseWidget::getScriptExecutor()
{
    if (m_ScriptExecutor || !m_Parent)
        return m_ScriptExecutor;

    m_ScriptExecutor = m_Parent->getScriptExecutor();
    return m_ScriptExecutor;
}
