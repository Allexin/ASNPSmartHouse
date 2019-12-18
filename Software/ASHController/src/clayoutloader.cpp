#include "clayoutloader.h"
#include <QMap>
#include <QDebug>

#include "widgets/cwidgetlabel.h"
#include "widgets/cwidgetpages.h"
#include "widgets/cwidgetimage.h"
#include "widgets/cwidgetbutton.h"

static const QString LAYOUT_HEADER = "ASNP";
static const int LAYOUT_VERSION = 1;

cLayoutLoader::sKeyValue::sKeyValue(QString s)
{
    int pos = s.indexOf("=");
    if (pos<=0)
    {
        key = s.trimmed().toUpper();
        value = "";
    }
    else{
        key = s.mid(0,pos).trimmed().toUpper();
        value = s.mid(pos+1).trimmed();
    }
}

QVector<cBaseWidget *> cLayoutLoader::loadWidgets(QIODevice *device)
{
    QVector<cBaseWidget *> widgets;
    while (cBaseWidget* widget = loadWidget(device))
    {
        widgets.push_back(widget);
    }
    return widgets;
}

QString readMultiLine(QIODevice* device)
{
    QString line = device->readLine().trimmed();
    while (line.mid(line.size()-1)=="\\")
    {
        line = line.mid(0,line.size()-2) + '\n' + device->readLine().trimmed();
    }
    return line;
}

cBaseWidget *cLayoutLoader::loadWidget(QIODevice *device)
{
    sKeyValue kv(readMultiLine(device));
    if (kv.key!="WIDGET")
    {
        return nullptr;
    }

    QVector<cBaseWidget *> childs;
    QMap<QString,QString> properties;
    while (kv.key!="WIDGET_END")
    {
        if (kv.key=="WIDGETS_LIST")
        {
            childs = loadWidgets(device);
        }
        else
        {
            properties[kv.key] = kv.value;
        }

        if (device->canReadLine())
        {
            kv = sKeyValue(readMultiLine(device));
        }
        else
        {
            qCritical() << "unexpected end of file";
            break;
        }
    }

    cBaseWidget* widget = createWidget(properties["WIDGET"]);
    widget->setProperties(properties);
    for (int i = 0; i<childs.size(); ++i)
    {
        widget->addWidget(childs[i]);
    }
    return widget;
}

cBaseWidget *cLayoutLoader::createWidget(QString type)
{
    if (type=="BASE")
        return new cBaseWidget();
    if (type=="LABEL")
        return new cWidgetLabel();
    if (type=="IMAGE")
        return new cWidgetImage();
    if (type=="BUTTON")
        return new cWidgetButton();
    if (type=="PAGES")
        return new cWidgetPages();
    qCritical() << "unknown widget type " << type;
    return nullptr;
}



cBaseWidget * cLayoutLoader::parse(QIODevice *device)
{
    sKeyValue header(readMultiLine(device));
    if (header.key==LAYOUT_HEADER)
    {
        if (header.value.toInt()==LAYOUT_VERSION)
        {
            return loadWidget(device);
        }
        else
        {
            qCritical() << "cLayoutLoader: incorrect version " << header.value;
        }
    }
    else
    {
        qCritical() << "cLayoutLoader: incorrect header " << header.key;
    }
    return nullptr;
}
