#ifndef CWIDGETBUTTON_H
#define CWIDGETBUTTON_H


#include "../cbasewidget.h"

class cWidgetButton : public cBaseWidget
{
protected:
    QPixmap              m_Normal;        //PROPERTY NORMALIMAGEFILENAME
    QPixmap              m_Pressed;       //PROPERTY PRESSEDIMAGEFILENAME
    QString             m_OnClickScript; //PROPERTY ONCLICK
    bool                m_IsPressed;
    bool                m_IsUnderCursor;
public:
    cWidgetButton();

    virtual void draw(QPainter& p) override;
    virtual bool touchStart(QPoint pos)  override;
    virtual void touchEnd()  override;
    virtual void touchMove(QPoint pos)  override;
    virtual void touchCancel()  override;

    virtual void setProperties(QMap<QString, QString> &properties) override;

    void setOnClickScript(QString value);
    void setNormalImage(QString fileName);
    void setPressedImage(QString fileName);
};

#endif // CWIDGETBUTTON_H
