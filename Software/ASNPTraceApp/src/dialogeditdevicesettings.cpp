#include "dialogeditdevicesettings.h"
#include "ui_dialogeditdevicesettings.h"
#include <QLayout>
#include <QListWidget>

const char* DialogEditDeviceSettings::PropertyID = "SETTINGS_ID";

void DialogEditDeviceSettings::clearList()
{
    for (int i = 0; i<m_SettingsWidgets.size(); ++i)
        delete m_SettingsWidgets[i];
    m_SettingsWidgets.clear();
    ui->listWidget->clear();
}

DialogEditDeviceSettings::DialogEditDeviceSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditDeviceSettings)
{
    ui->setupUi(this);
}

DialogEditDeviceSettings::~DialogEditDeviceSettings()
{
    delete ui;
}

void DialogEditDeviceSettings::exec(uint8_t deviceAddress)
{
    m_DeviceAddress = deviceAddress;
    ui->labelCurrentDeviceAddress->setText(QString::number(m_DeviceAddress));
    ui->spinBoxNewAddress->setValue(m_DeviceAddress);
    on_pushButtonReloadSettings_clicked();

    QDialog::exec();
}

void DialogEditDeviceSettings::processSettings(uint8_t deviceAddress, sDeviceSettings settings)
{
    if (deviceAddress!=m_DeviceAddress)
        return;
    m_DeviceSettings = settings;

    clearList();

    for (int i = 0; i<m_DeviceSettings.settings.size(); ++i){
        const sDeviceSettingsItem& s = m_DeviceSettings.settings[i];
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(QSize(item->sizeHint().width(), 40));

        QWidget* w = new QWidget();
        QHBoxLayout* l = new QHBoxLayout(w);
        l->setAlignment(Qt::AlignLeft);
        l->setAlignment(Qt::AlignTop);
        w->setObjectName(QString::number(i));
        w->setFixedHeight(200);
        w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        w->setLayout(l);

        QLabel* label = new QLabel(w);
        w->layout()->addWidget(label);
        label->setText(s.name);

        QLabel* label2 = new QLabel(w);
        w->layout()->addWidget(label2);
        label2->setText(s.loadedValue);

        switch (s.type){
            case sDeviceSettingsItem::eType::T_CONST_BYTE:{
                QLabel* label3 = new QLabel(w);
                w->layout()->addWidget(label3);
                label3->setText("");
            };break;
            case sDeviceSettingsItem::eType::T_UINT8:{
                QSpinBox* spinBox3 = new QSpinBox(w);
                w->layout()->addWidget(spinBox3);
                spinBox3->setMinimum(s.min.toInt());
                spinBox3->setMaximum(s.max.toInt());
                spinBox3->setValue(s.value.toInt());
                spinBox3->setProperty(PropertyID,i);
                connect(spinBox3,SIGNAL(editingFinished()),this,SLOT(onSpinBoxSettingEdit()));
            };break;
        }

        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item,w);
        m_SettingsWidgets.append(w);
    }
}

void DialogEditDeviceSettings::addressChanged(uint8_t deviceAddress, uint8_t newAddress)
{
    if (deviceAddress!=m_DeviceAddress)
        return;
    m_DeviceAddress = newAddress;
    ui->labelCurrentDeviceAddress->setText(QString::number(m_DeviceAddress));
    ui->spinBoxNewAddress->setValue(m_DeviceAddress);
}

void DialogEditDeviceSettings::on_pushButtonSetNewAddress_clicked()
{
    if (ui->spinBoxNewAddress->value()==m_DeviceAddress)
        return;
    emit setNewAddress(m_DeviceAddress,ui->spinBoxNewAddress->value());

    void requestSettings(uint8_t deviceAddress);

}

void DialogEditDeviceSettings::on_pushButtonReloadSettings_clicked()
{
    clearList();
    emit requestSettings(m_DeviceAddress);
}

void DialogEditDeviceSettings::on_pushButtonSetSettings_clicked()
{
    emit setSettings(m_DeviceAddress,m_DeviceSettings);
}

void DialogEditDeviceSettings::onSpinBoxSettingEdit()
{
    QSpinBox* spinBox = qobject_cast<QSpinBox*>(sender());
    if( spinBox != NULL ) {
        int id = spinBox->property(PropertyID).toInt();
        if (id>=0 && id<m_DeviceSettings.settings.size()){
            m_DeviceSettings.settings[id].value = QString::number(spinBox->value());
        }
    }
}
