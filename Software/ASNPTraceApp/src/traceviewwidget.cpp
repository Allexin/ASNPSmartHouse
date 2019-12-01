#include "traceviewwidget.h"

int TraceViewWidget::getMasterPort(sTraceDevice* device){
    for (int i = 0; i<device->ports.size(); ++i)
        if (device->ports[i].state==sTracePort::eState::MASTER)
            return i;
    return -1;
}

int TraceViewWidget::getPort(sTraceDevice *device, sTraceDevice *child)
{
    for (int i = 0; i<device->ports.size(); ++i)
        if (device->ports[i].connection==child)
            return i;
    return -1;
}

int TraceViewWidget::getPortX(sTraceDevice* device, int port){
    int w = qMax(device->ports.size()*PORT_WIDTH, MIN_DEVICE_WIDTH);
    int portStartX = (w - device->ports.size()*PORT_WIDTH) / 2;
    return portStartX+port*PORT_WIDTH;
}

int TraceViewWidget::drawDevice(QPainter &p, int x, int y, int deviceIndex)
{
    if (deviceIndex>=m_TraceResult->devices.size())
        return 0;

    sTraceDevice* device = m_TraceResult->devices[deviceIndex];

    int w = qMax(device->ports.size()*PORT_WIDTH, MIN_DEVICE_WIDTH);
    int h = DEVICE_HEIGHT;

    device->renderedRect = QRect(x,y,w,h);

    p.drawRect(device->renderedRect);
    QString name;
    if (getMasterPort(device)==-1 && device->address==0){
        name = tr("root");
    }
    else{
        if (device->address==0){
            name = tr("router");
        }
        else{
            if (device->state==sTraceDevice::eState::IDENTIFIED){
                name = device->type+"\n";
            }
            name += tr("address: %1").arg(QString::number(device->address));
        }

    }
    if (device->state==sTraceDevice::eState::NO_RESPOND){
        p.setPen(Qt::red);
    }
    else{
        p.setPen(Qt::black);
    }
    p.drawText(device->renderedRect, Qt::AlignCenter, name);
    p.setPen(Qt::black);

    for (int i = 0; i<device->ports.size(); ++i){
        if (device->ports[i].state==sTracePort::eState::TIMEOUT)
            p.setPen(Qt::red);
        QRect r(x+getPortX(device,i),device->ports[i].state==sTracePort::eState::MASTER?y:y+h-PORT_HEIGHT,PORT_WIDTH,PORT_HEIGHT);
        p.drawRect(r);
        p.drawText(r, Qt::AlignCenter, QString::number(i+1));

        p.setPen(Qt::black);

        if (device->ports[i].state==sTracePort::eState::MASTER && device->ports[i].connection!=NULL){
            int pX = getPortX(device->ports[i].connection,getPort(device->ports[i].connection,device));
            p.drawLine(device->ports[i].connection->renderedRect.x() + pX + PORT_WIDTH/2, device->ports[i].connection->renderedRect.y()+DEVICE_HEIGHT,r.left()+PORT_WIDTH/2,r.top());
        }
    }
    return w;
}

TraceViewWidget::TraceViewWidget(QWidget *parent) :
    QWidget(parent),
    m_TraceResult(NULL)

{
}

TraceViewWidget::~TraceViewWidget()
{
}



void TraceViewWidget::paintEvent(QPaintEvent *)
{
    if (!m_TraceResult)
        return;

    if (m_TraceResult->devices.size()==0)
        return;

    QPainter p(this);

    int centerX = width () / 2;
    int y = PADDING_VERTICAL;

    for (int l = 0; l<m_layers.size(); ++l ){
        sViewLayer& layer = m_layers[l];
        int x = centerX - layer.width / 2;
        for (int i = 0; i<layer.devices.size(); ++i){
            x +=drawDevice(p, x, y, layer.devices[i]) + PADDING_HORIZONTAL;
        }
        y+= DEVICE_HEIGHT + PADDING_VERTICAL;
    }


}

void TraceViewWidget::mouseReleaseEvent(QMouseEvent *releaseEvent)
{
     if (releaseEvent->button()==Qt::RightButton){
         if (m_TraceResult){
            for (int i = 0; i<m_TraceResult->devices.size(); ++i){
                sTraceDevice* device = m_TraceResult->devices[i];
                if (device->renderedRect.contains(releaseEvent->pos())){
                    emit onDeviceMousePressed(device);
                    break;
                }
            }
         }


     }
}


void TraceViewWidget::traceResultUpdated()
{
    if (!m_TraceResult)
        return;
    int newWidth = MIN_DEVICE_WIDTH + PADDING_HORIZONTAL * 2;
    m_layers.clear();
    for (int i = 0; i<m_TraceResult->devices.size(); ++i){
        sTraceDevice* device = m_TraceResult->devices[i];
        if (device->level>=m_layers.size()){
            m_layers.resize(device->level+1);
        }
        device->xPos = m_layers[device->level].width;
        m_layers[device->level].width = m_layers[device->level].width + PADDING_HORIZONTAL + qMax(device->ports.size()*PORT_WIDTH, MIN_DEVICE_WIDTH);
        if (newWidth < m_layers[device->level].width)
            newWidth = m_layers[device->level].width;
        m_layers[device->level].devices.append(i);
    }

    int newHeight = PADDING_VERTICAL + m_layers.size()* (DEVICE_HEIGHT + PADDING_VERTICAL);

    setMaximumHeight(newHeight);
    setMinimumHeight(newHeight);

    setMaximumWidth(newWidth);
    setMinimumWidth(newWidth);

    update();
}
