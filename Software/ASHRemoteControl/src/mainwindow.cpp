#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMap>
#include <QTabBar>
#include <QScroller>
#include <QListWidget>
#include <QSettings>

void MainWindow::showCategories()
{
    ui->listWidget_Categories->clear();

    auto devices = m_ASNPClient.devices();
    QStringList m_Categories;
    for (int i = 0; i<devices.size(); ++i)
    {
        if (devices[i].category.isEmpty())
        {

        }
        else
        {
            if (m_Categories.contains(devices[i].category))
            {

            }
            else
            {
                m_Categories.append(devices[i].category);
            }
        }
    }

    for (int i = 0; i<m_Categories.size(); ++i)
    {
        QListWidgetItem* item = new QListWidgetItem(m_Categories[i]);
        item->setData(Qt::UserRole,m_Categories[i]);
        ui->listWidget_Categories->addItem(item);
    }
    ui->listWidget_Categories->addItem("Uncategorized");

    fixListItemSize(ui->listWidget_Categories);
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::showDevices(QString category)
{
    ui->listWidget_Devices->clear();

    auto devices = m_ASNPClient.devices();

    for (int i = 0; i<devices.size(); ++i)
    {
        if (devices[i].category==category)
        {
            QListWidgetItem* item = new QListWidgetItem(devices[i].name);
            item->setData(Qt::UserRole,i);
            ui->listWidget_Devices->addItem(item);
        }
    }

    fixListItemSize(ui->listWidget_Devices);
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::fixListItemSize(QListWidget *lw)
{
    static const int SIZE = 50;
    int count = lw->count();
    for(int i = 0; i < count; i++)
    {
      QListWidgetItem *item = lw->item(i);
      item->setSizeHint(QSize(item->sizeHint().width(), SIZE));
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    ui->tabWidget->tabBar()->hide();
    ui->tabWidget_2->tabBar()->hide();

    QScroller::grabGesture(ui->listWidget_Categories, QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->listWidget_Devices, QScroller::LeftMouseButtonGesture);

    QObject::connect(ui->listWidget_Categories,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(categoryClicked(QListWidgetItem*)));
    QObject::connect(ui->listWidget_Devices,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(deviceClicked(QListWidgetItem*)));

    loadConfig();

    showCategories();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::categoryClicked(QListWidgetItem *item)
{
    showDevices(item->data(Qt::UserRole).toString());
}

void MainWindow::deviceClicked(QListWidgetItem *item)
{
    bool ok;
    m_DeviceIndex = item->data(Qt::UserRole).toInt(&ok);
    if (!ok)
        return;
    auto devices = m_ASNPClient.devices();
    if (m_DeviceIndex<0)
        return;
    if (m_DeviceIndex>=devices.size())
        return;

    switch (devices[m_DeviceIndex].type) {
        case eDeviceType::UNKONWN:
        {
        };break;
        case eDeviceType::DMX_DIMMER:
        {
            showDimmer();
        };break;
        case eDeviceType::GATE:
        {
            showGate();
        };break;
    }
}

void MainWindow::showDimmer()
{
    showDevice(0);
}

void MainWindow::showGate()
{
    showDevice(1);
}

void MainWindow::showDevice(int index)
{
    ui->tabWidget->setCurrentIndex(2);
    ui->tabWidget_2->setCurrentIndex(index);
    if (auto device = m_ASNPClient.device(m_DeviceIndex))
    {
        ui->label_DeviceName->setText(device->name);
    }
}

void MainWindow::on_pushButton_DMXDimmer_OFF_clicked()
{
    if (auto device = m_ASNPClient.device(m_DeviceIndex))
    {
        m_ASNPClient.setLight(device->address,device->channel,0,2);
    }

}

void MainWindow::on_pushButton_DMXDimmer_ON_clicked()
{
    if (auto device = m_ASNPClient.device(m_DeviceIndex))
    {
        m_ASNPClient.setLight(device->address,device->channel,200,2);
    }
}

void MainWindow::on_horizontalSlider_Dimmer_valueChanged(int value)
{
    if (auto device = m_ASNPClient.device(m_DeviceIndex))
    {
        m_ASNPClient.setLight(device->address,device->channel,value,0);
    }
}


void MainWindow::on_pushButton_Back_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_BackToCategories_clicked()
{
    showCategories();
}


void MainWindow::on_pushButton_Gate_UP_clicked()
{
    if (auto device = m_ASNPClient.device(m_DeviceIndex))
    {
        m_ASNPClient.startGate(device->address,true);
    }
}

void MainWindow::on_pushButton_Gate_STOP_clicked()
{
    if (auto device = m_ASNPClient.device(m_DeviceIndex))
    {
        m_ASNPClient.stopGate(device->address);
    }
}

void MainWindow::on_pushButton_Gate_DOWN_clicked()
{
    if (auto device = m_ASNPClient.device(m_DeviceIndex))
    {
        m_ASNPClient.startGate(device->address,false);
    }
}

void MainWindow::on_pushButton_Settings_clicked()
{
    ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::loadConfig()
{
    QSettings settings;
    m_CFGPin = settings.value("SETTINGS_PIN","0000").toString();
    m_CFGEnableSettings = settings.value("SETTINGS_ENABLED",false).toBool();

    ui->pushButton_DeviceSettings->setVisible(m_CFGEnableSettings);
}

void MainWindow::on_pushButton_SaveSettings_clicked()
{
    QSettings settings;
    settings.setValue("SETTINGS_PIN",ui->lineEdit_SettingsPIN->text());
    settings.setValue("SETTINGS_ENABLED",ui->checkBox_EnableSettings->isChecked());

    settings.sync();
    loadConfig();
}

void MainWindow::on_pushButton_PinOK_clicked()
{
    if (ui->lineEdit_PIN->text()==m_CFGPin)
    {
        ui->checkBox_EnableSettings->setChecked(m_CFGEnableSettings);
        ui->lineEdit_SettingsPIN->setText(m_CFGPin);
        ui->lineEdit_PIN->setText("");
        ui->tabWidget->setCurrentIndex(4);
    }
}

void MainWindow::on_pushButton_SettingsBack_clicked()
{
    showCategories();
}




