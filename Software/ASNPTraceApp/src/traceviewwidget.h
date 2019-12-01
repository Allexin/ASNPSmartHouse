#ifndef TRACEVIEWWIDGET_H
#define TRACEVIEWWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "tracetree.h"
class TraceViewWidget : public QWidget
{
    Q_OBJECT
private:
    sTraceTree*         m_TraceResult;

    const int PORT_WIDTH = 25;
    const int PORT_HEIGHT = PORT_WIDTH;
    const int DEVICE_HEIGHT = 40 + PORT_HEIGHT * 2;
    const int MIN_DEVICE_WIDTH = 140;



    const int PADDING_VERTICAL = 40;
    const int PADDING_HORIZONTAL = 10;

    struct sViewLayer{
        QVector<int> devices;
        int width;
    };
    int getMasterPort(sTraceDevice* device);
    int getPort(sTraceDevice* device, sTraceDevice* child);
    int getPortX(sTraceDevice* device, int port);
    QVector<sViewLayer> m_layers;
    int drawDevice(QPainter &p, int x, int y, int deviceIndex);
public:
    explicit TraceViewWidget(QWidget *parent = 0);
    ~TraceViewWidget();

    virtual void paintEvent(QPaintEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent *releaseEvent) override;

    void setTraceResult(sTraceTree *traceResult){
        m_TraceResult = traceResult;
        traceResultUpdated();
    }

    void traceResultUpdated();
signals:
    void onDeviceMousePressed(sTraceDevice* device);
};

#endif // TRACEVIEWWIDGET_H
