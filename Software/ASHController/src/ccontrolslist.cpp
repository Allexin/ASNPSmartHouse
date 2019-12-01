#include "ccontrolslist.h"
#include "ui_ccontrolslist.h"
#include <QPainter>
#include <QFont>
#include <QDateTime>
#include <QMouseEvent>
#include <QPoint>

static const uint8_t FLAG_PARITY = 1;
static const uint8_t FLAG_HIGH_PRIORITY = 2;
static const uint8_t FLAG_GROUP_ADDRESS = 4;
static const uint8_t FLAG_EVENT = 8;

static const uint8_t CMD_SET_VARIABLE = 0x28;

static const long PACKAGE_HEADER_SIZE = 1+1+1; //PACKAGE_INFO + ADDRESS + DATA_SIZE
static const long PACKAGE_SERVICE_DATA_SIZE = PACKAGE_HEADER_SIZE+1; // + CHECKSUM
static const long MAX_PACKAGE_DATA_SIZE = 255;
static const long PACKAGE_BUFFER_SIZE = MAX_PACKAGE_DATA_SIZE + PACKAGE_SERVICE_DATA_SIZE;

enum eVariableType : uint8_t{
  VT_UINT8 = 0,
  VT_UINT16,
  VT_UFIXFLOAT16,
  VT_FIXFLOAT16,
  VT_STRING16,
  VT_LINK
};

struct __attribute__((packed)) sUFixFloat{
  uint8_t integer;
  uint8_t frac;
};

struct __attribute__((packed)) sFixFloat{
  int8_t integer;
  uint8_t frac;
};


struct __attribute__((packed)) sVariableLink{
  uint8_t addressType;
  uint8_t address;
  char linkVariableName[16];
  uint8_t linkVariableSlotIndex;
  eVariableType linkVariableType;
};

struct __attribute__((packed)) sVariableInfo{
  char name[16];
  eVariableType type;
  uint8_t slotsCount;//also slot index in some cases
};

uint8_t getVariableSize(eVariableType type)
{
  switch(type){
    case VT_UINT8: return sizeof(uint8_t);break;
    case VT_UINT16: return sizeof(uint16_t);break;
    case VT_UFIXFLOAT16: return sizeof(sUFixFloat);break;
    case VT_FIXFLOAT16: return sizeof(sFixFloat);break;
    case VT_STRING16: return 16;break;
    case VT_LINK: return sizeof(sVariableLink);break;
  }
}



class cASNPMessageBuilder{
protected:
    QByteArray          m_SendBuffer;
public:
    void writeStart(bool highPriority, bool groupAddress, bool event, uint8_t address, uint8_t dataLength, uint8_t cmdType, uint8_t cmd, uint8_t dataVersion)
    {
        uint8_t parity = (highPriority?1:0) +(groupAddress?1:0) +(event?1:0);
        bool parityBit = parity % 2 == 1;
        m_SendBuffer.clear();
        uint8_t flags = 0;
        if (parityBit)
          flags = flags | FLAG_PARITY;
        if (highPriority)
          flags = flags | FLAG_HIGH_PRIORITY;
        if (groupAddress)
          flags = flags | FLAG_GROUP_ADDRESS;
        if (event)
          flags = flags | FLAG_EVENT;
        m_SendBuffer.push_back(flags);
        m_SendBuffer.push_back(address);
        if (dataLength<3)
          dataLength = 3;
        m_SendBuffer.push_back(dataLength);
        m_SendBuffer.push_back(cmdType);
        m_SendBuffer.push_back(cmd);
        m_SendBuffer.push_back(dataVersion);
    }

    void writeEnd()
    {
        m_SendBuffer[2] = m_SendBuffer.size() - PACKAGE_HEADER_SIZE;
        uint8_t checksum = 0;
        for (int i = 0; i<m_SendBuffer.size(); ++i){
            checksum += m_SendBuffer[i];
        }
        writeuint8(checksum);
    }
    void write(const uint8_t* data, uint8_t lngt)
    {
      if (m_SendBuffer.size()+lngt>PACKAGE_BUFFER_SIZE)
        return;
      m_SendBuffer.append((const char*)data,lngt);
    }
    void writeuint8(uint8_t value)
    {
        write(&value, 1);
    }

    void writeVariableInf(sVariableInfo *info)
    {
        for (int i = 0; i<16; ++i){
            writeuint8(info->name[i]);
        }
        writeuint8(info->type);
        writeuint8(info->slotsCount);
    }

    QByteArray package() { return m_SendBuffer; }
};



const float MENU_SCALE = 4.f;

void cBaseItem::draw(QPainter *p, int y, int width)
{
    p->save();
    p->fillRect(0,y,width,getHeight(),Qt::lightGray);

    p->setPen(Qt::black);
    p->drawLine(0,y+getHeight()-1,width,y+getHeight()-1);
    p->restore();
}


void drawButton(QPainter *p, int x, int y, int width, int height, QString text){
    QRect r(x+1,y+1,width-2,height-2);
    p->setPen(Qt::black);
    p->setBrush(Qt::gray);
    p->drawRect(r);
    p->drawText(r, Qt::AlignCenter, text);
}

cHeaderItem::cHeaderItem(QString text, int level):cBaseItem(),
    m_Text(text),
    m_Level(level)
{
}
int cHeaderItem::getHeight()
{
    return 20*MENU_SCALE;
}
void cHeaderItem::draw(QPainter* p, int y, int width)
{
    cBaseItem::draw(p,y,width);
    p->save();

    QFont font = p->font();
    font.setPointSize(font.pointSize()-m_Level*2);
    font.setBold(m_Level==0);
    p->setFont(font);

    p->drawText(5*MENU_SCALE+m_Level*5*MENU_SCALE,y+1+16*MENU_SCALE, m_Text);

    p->restore();
}

uint8_t min(uint8_t a, uint8_t b)
{
    return a<b?a:b;
}

void StringToName(QString value, char* name)
{
    memset(name,0,16);
    QByteArray dta = value.toLatin1();
    memcpy(name, dta.data(), min(16,dta.size()));
}

void cDimmerItem::setChannelState(uint8_t state, uint8_t speed)
{
    sVariableInfo vi;
    vi.type = VT_UINT8;

    cASNPMessageBuilder speedBuilder;
    speedBuilder.writeStart(true,false,false,m_Address,0x00,0x00,CMD_SET_VARIABLE,0x01);
    StringToName("ChannelStepSqrt",vi.name);
    vi.slotsCount = m_Channel;
    speedBuilder.writeVariableInf(&vi);
    speedBuilder.writeuint8(speed);
    speedBuilder.writeEnd();

    cASNPMessageBuilder targetBuilder;
    targetBuilder.writeStart(true,false,false,m_Address,0x00,0x00,CMD_SET_VARIABLE,0x01);
    StringToName("ChannelTarget",vi.name);
    vi.slotsCount = m_Channel;
    targetBuilder.writeVariableInf(&vi);
    targetBuilder.writeuint8(state);
    targetBuilder.writeEnd();

    QByteArray package = speedBuilder.package()+targetBuilder.package();

    m_Client->sendCommand((const uint8_t*)package.constData(), package.size());
}

cDimmerItem::cDimmerItem(uint8_t Address, uint8_t Channel, QColor Color, cUDPClient *client):cBaseItem(),
    m_Address(Address),
    m_Channel(Channel),
    m_Color(Color),
    m_Client(client)
{
}

int cDimmerItem::getHeight()
{
    return 64*MENU_SCALE;
}
void cDimmerItem::draw(QPainter* p, int y, int width)
{
    m_Y = y;
    m_Width = width - getHeight();

    m_Slider = QRect(getHeight(),m_Y,m_Width-getHeight()*2,getHeight());

    cBaseItem::draw(p,y,m_Width);

    p->save();

    drawButton(p,0,y,getHeight(),getHeight(),"OFF");
    drawButton(p,m_Width-getHeight(),y,getHeight(),getHeight(),"ON");

    QLinearGradient gradient(m_Slider.topLeft(), m_Slider.topRight());
    gradient.setColorAt(0, Qt::black);
    gradient.setColorAt(1, m_Color);
    p->fillRect(m_Slider, gradient);

    p->restore();
}
bool cDimmerItem::mousePress(int x, int y)
{
    if (x<m_Width && y>m_Y && y<m_Y+getHeight()){
        if (x<m_Slider.left()){
            setChannelState(0,2);
        }
        else
        if (x>m_Slider.right()){
            setChannelState(255,5);
        }
        else{
            float value = x-m_Slider.left();
            value = value / m_Slider.width();
            setChannelState((int)(value*255),1);
        }
        return true;
    }
    return false;
}

void cDimmerItem::mouseMove(int x, int y)
{
    mousePress(x,y);
}




void cGateItem::setState(uint8_t active, uint8_t direction)
{
    sVariableInfo vi;
    vi.type = VT_UINT8;
    vi.slotsCount = 0;

    cASNPMessageBuilder directionBuilder;
    directionBuilder.writeStart(true,false,false,m_Address,0x00,0x00,CMD_SET_VARIABLE,0x01);
    StringToName("Direction",vi.name);
    directionBuilder.writeVariableInf(&vi);
    directionBuilder.writeuint8(direction);
    directionBuilder.writeEnd();

    cASNPMessageBuilder activeBuilder;
    activeBuilder.writeStart(true,false,false,m_Address,0x00,0x00,CMD_SET_VARIABLE,0x01);
    StringToName("Active",vi.name);
    activeBuilder.writeVariableInf(&vi);
    activeBuilder.writeuint8(active);
    activeBuilder.writeEnd();

    QByteArray package = directionBuilder.package()+activeBuilder.package();

    m_Client->sendCommand((const uint8_t*)package.constData(), package.size());
}

cGateItem::cGateItem(uint8_t Address, cUDPClient *client):cBaseItem(),
    m_Address(Address),
    m_Client(client)
{
}

int cGateItem::getHeight()
{
    return 64*MENU_SCALE;
}
void cGateItem::draw(QPainter* p, int y, int width)
{
    m_Y = y;
    m_Width = width - getHeight();

    int buttonWidth = m_Width / 3;

    cBaseItem::draw(p,y,m_Width);

    p->save();

    drawButton(p,0,y,buttonWidth,getHeight(),"CLOSE");
    drawButton(p,buttonWidth,y,buttonWidth,getHeight(),"STOP");
    drawButton(p,buttonWidth*2,y,buttonWidth,getHeight(),"OPEN");

    p->restore();
}
bool cGateItem::mousePress(int x, int y)
{
    if (x<m_Width && y>m_Y && y<m_Y+getHeight()){

        int buttonWidth = m_Width / 3;

        if (x<buttonWidth)
        {
            //close
            setState(1, 1);
        }
        else
        if (x>buttonWidth*2)
        {
            setState(1, 0);
        }
        else
        {
            setState(0, 0);
        }
        return true;
    }
    return false;
}

void cGateItem::mouseMove(int x, int y)
{
    mousePress(x,y);
}



int cControlsList::getListHeight()
{
    int h = 0;
    for (int i = 0; i<m_Items.size(); ++i){
        h += m_Items[i]->getHeight();
    }
    return h;
}

cControlsList::cControlsList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cControlsList),
    m_Scrolling(false),
    m_Scroll(0),
    m_CurrentItem(-1)
{
    ui->setupUi(this);

    addItem(new cHeaderItem("Желтая комната",0));
    addItem(new cDimmerItem(1,0,Qt::white,&m_UDPClient));
    addItem(new cDimmerItem(1,1,Qt::red, &m_UDPClient));
    addItem(new cDimmerItem(1,2,Qt::green, &m_UDPClient));
    addItem(new cDimmerItem(1,3,Qt::blue, &m_UDPClient));
    addItem(new cGateItem(12, &m_UDPClient));

    addItem(new cHeaderItem("Кухня",0));
    addItem(new cDimmerItem(10,0,Qt::white, &m_UDPClient));
    addItem(new cDimmerItem(10,1,Qt::red, &m_UDPClient));

}

cControlsList::~cControlsList()
{
    delete ui;
    clearItems();
}

void cControlsList::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QFont font = p.font();
    font.setPointSize(14);
    p.setFont(font);

    int y = m_Scroll;
    for (int i = 0; i<m_Items.size(); ++i){
        m_Items[i]->draw(&p,y,width());

        y += m_Items[i]->getHeight();
    }
}

void cControlsList::mousePressEvent(QMouseEvent *eventPress)
{
    m_PrevMousePos = eventPress->pos();
    m_MouseShift = 0;
    bool canScroll = true;
    for (int i = 0; i<m_Items.size(); ++i){
        if (m_Items[i]->mousePress(m_PrevMousePos.x(), m_PrevMousePos.y())){
            canScroll = false;
            m_CurrentItem = i;
            break;
        }
    }
    m_Scrolling = canScroll;
    update();
}

void cControlsList::mouseMoveEvent(QMouseEvent *eventPress)
{
    if (m_Scrolling){
        m_Scroll = m_Scroll + eventPress->pos().y() - m_PrevMousePos.y();
        m_MouseShift = m_MouseShift /* +(eventPress->pos().x() - m_PrevMousePos.x())*/ + abs(eventPress->pos().y() - m_PrevMousePos.y());
    }    
    m_PrevMousePos = eventPress->pos();
    if (m_CurrentItem>-1){
        m_Items[m_CurrentItem]->mouseMove(m_PrevMousePos.x(), m_PrevMousePos.y());
    }
    update();
}

void cControlsList::mouseReleaseEvent(QMouseEvent *eventPress)
{
    if (m_Scrolling){

    }
    fixScroll();


    m_Scrolling = false;
    update();
}

void cControlsList::fixScroll()
{
    int maxScroll = getListHeight()-height();
    if (m_Scroll<-maxScroll)
        m_Scroll = -maxScroll;
    if (m_Scroll>20)
        m_Scroll = 20;

}

void cControlsList::clearItems()
{
    for (int i = 0; i<m_Items.size(); ++i){
        delete m_Items[i];
    }
    m_Items.clear();
}

void cControlsList::addItem(cBaseItem *item)
{
    m_Items.append(item);
}



