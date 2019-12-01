#ifndef CCONTROLSLIST_H
#define CCONTROLSLIST_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QDateTime>
#include <cudpclient.h>

namespace Ui {
class cControlsList;
}

class cBaseItem: public QObject{
    Q_OBJECT
public:
    cBaseItem():QObject() {}
    virtual int getHeight() = 0;
    virtual void draw(QPainter* p, int y, int width);
    virtual bool mousePress(int x, int y) = 0;
    virtual void mouseMove(int x, int y) = 0;
};

class cHeaderItem: public cBaseItem{
    Q_OBJECT
protected:
    QString             m_Text;
    int                 m_Level;
public:
    cHeaderItem(QString text, int level);
    virtual int getHeight() override;
    virtual void draw(QPainter* p, int y, int width) override;
    virtual bool mousePress(int x, int y) override {return false;}
    virtual void mouseMove(int x, int y) override {}
};

class cDimmerItem: public cBaseItem{
    Q_OBJECT
protected:
    uint8_t m_Address;
    uint8_t m_Channel;
    QColor m_Color;
    int m_Width;
    int m_Y;
    QRect m_Slider;

    cUDPClient* m_Client;

    void setChannelState(uint8_t state, uint8_t speed);
public:
    cDimmerItem(uint8_t Address, uint8_t Channel, QColor Color, cUDPClient* client);
    virtual int getHeight() override;
    virtual void draw(QPainter* p, int y, int width) override;
    virtual bool mousePress(int x, int y) override;
    virtual void mouseMove(int x, int y) override;
};

class cGateItem: public cBaseItem{
    Q_OBJECT
protected:
    uint8_t m_Address;
    int m_Width;
    int m_Y;

    cUDPClient* m_Client;

    void setState(uint8_t active, uint8_t direction);
public:
    cGateItem(uint8_t Address, cUDPClient* client);
    virtual int getHeight() override;
    virtual void draw(QPainter* p, int y, int width) override;
    virtual bool mousePress(int x, int y) override;
    virtual void mouseMove(int x, int y) override;
};

class cControlsList : public QWidget
{
    Q_OBJECT
protected:
    int                 m_Scroll;
    bool                m_Scrolling;
    QPoint              m_PrevMousePos;
    int                 m_MouseShift;
    int                 m_CurrentItem;
    QVector<cBaseItem*> m_Items;

    cUDPClient          m_UDPClient;

    int getListHeight();
    void fixScroll();
    void clearItems();
    void addItem(cBaseItem* item);
public:
    explicit cControlsList(QWidget *parent = 0);
    ~cControlsList();

    virtual void paintEvent(QPaintEvent*) override;
    virtual void mousePressEvent(QMouseEvent *eventPress) override;
    virtual void mouseMoveEvent(QMouseEvent *eventPress) override;
    virtual void mouseReleaseEvent(QMouseEvent *eventPress) override;
private:
    Ui::cControlsList *ui;
};

#endif // CCONTROLSLIST_H
