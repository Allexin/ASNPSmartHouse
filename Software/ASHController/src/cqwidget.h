#ifndef CQWIDGET_H
#define CQWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "cbasewidget.h"

class cQWidget : public QWidget
{
    Q_OBJECT
protected:
    QVector<cBaseWidget*> m_LowLevelWidgets;
    QSize m_CachedSize;
public:
    explicit cQWidget(QWidget *parent = 0);
    ~cQWidget();

    virtual void paintEvent(QPaintEvent*) override;
    virtual void mousePressEvent(QMouseEvent *eventPress) override;
    virtual void mouseMoveEvent(QMouseEvent *eventPress) override;
    virtual void mouseReleaseEvent(QMouseEvent *eventPress) override;

    void addWidget(cBaseWidget* widget, cScriptExecutor *se);

    cBaseWidget* getBaseLayout();
};

#endif // CQWIDGET_H
