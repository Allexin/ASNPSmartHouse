#ifndef CFIXEDSLIDER_H
#define CFIXEDSLIDER_H

#include <QMouseEvent>
#include <QSlider>
#include <QWidget>

class cFixedSlider : public QSlider
{
protected:
  void mousePressEvent ( QMouseEvent * event )
  {
    if (event->button() == Qt::LeftButton)
    {
        if (orientation() == Qt::Vertical)
            setValue(minimum() + ((maximum()-minimum()) * (height()-event->y())) / height() ) ;
        else
            setValue(minimum() + ((maximum()-minimum()) * event->x()) / width() ) ;

        event->accept();
    }
    QSlider::mousePressEvent(event);
  }
public:
  cFixedSlider(QWidget* parent = nullptr):QSlider(parent){}
};

#endif // CFIXEDSLIDER_H
