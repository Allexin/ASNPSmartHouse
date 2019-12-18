#ifndef CWIDGETPAGES_H
#define CWIDGETPAGES_H

#include "../cbasewidget.h"

class cWidgetPages : public cBaseWidget
{
public:
    enum eDirection{
        VERTICAL,
        HORIZONTAL
    };
protected:
    static const int MAX_SCROLL_TO_CANCEL_TOUCH = 100;

    int                 m_CurrentPage; //PROPERTY DEFAULTPAGE
    int                 m_Scroll;
    eDirection          m_Direction;   //PROPERTY DIRECTION(VERTICAL,HORIZONTAL)

    int                 m_ScrollDist;
    bool                m_Scrolling;
    QPoint              m_TouchPos;

    void resetToCurrentPage();
public:
    cWidgetPages();

    virtual void setGeometry(QPoint position, QSize size) override;
    virtual void draw(QPainter& p) override;
    virtual bool touchStart(QPoint pos) override;
    virtual void touchEnd() override;
    virtual void touchMove(QPoint pos) override;
    virtual void touchCancel() override;

    virtual void addWidget(cBaseWidget* widget) override;

    void setDirection(eDirection direction);
    virtual void setProperties(QMap<QString, QString> &properties) override;
};

#endif // CWIDGETPAGES_H
