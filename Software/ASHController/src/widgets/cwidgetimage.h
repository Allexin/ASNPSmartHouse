#ifndef CWIDGETIMAGE_H
#define CWIDGETIMAGE_H


#include "../cbasewidget.h"

class cWidgetImage : public cBaseWidget
{
protected:
    QPixmap             m_Image;  //PROPERTY FILENAME
public:
    cWidgetImage();

    virtual void draw(QPainter& p) override;

    void setImage(QString fileName);

    virtual void setProperties(QMap<QString, QString> &properties)  override;
};

#endif // CWIDGETIMAGE_H
