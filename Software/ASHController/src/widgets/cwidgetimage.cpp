#include "cwidgetimage.h"
#include "../utils.h"

cWidgetImage::cWidgetImage():cBaseWidget(),
    m_Image()
{

}

void cWidgetImage::draw(QPainter &p)
{
    p.drawPixmap(QRect(QPoint(0,0),m_Size),m_Image);
    cBaseWidget::draw(p);
}

void cWidgetImage::setImage(QString fileName)
{
    m_Image = QPixmap(fileName);
}

void cWidgetImage::setProperties(QMap<QString, QString> &properties)
{
    cBaseWidget::setProperties(properties);
    QString fileName = getProperty(properties,"FILENAME","");
    if (!fileName.isEmpty())
    {
        setImage(getAppFolder()+fileName);
    }
}
