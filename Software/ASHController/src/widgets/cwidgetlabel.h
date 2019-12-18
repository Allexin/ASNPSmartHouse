#ifndef CWIDGETLABEL_H
#define CWIDGETLABEL_H

#include "../cbasewidget.h"

class cWidgetLabel : public cBaseWidget
{
protected:
    QString             m_Text;  //PROPERTY TEXT
    int                 m_Flags; //PROPERTY FLAGS
public:
    cWidgetLabel();

    virtual void draw(QPainter& p) override;

    void setText(QString text);
    QString text();

    void setFlags(int flags);
    int flags();
    virtual void setProperties(QMap<QString, QString> &properties) override;
};

#endif // CWIDGETLABEL_H
