#ifndef DIALOGEDITDEVICESETTINGS_H
#define DIALOGEDITDEVICESETTINGS_H

#include <QDialog>
#include "tracetree.h"

namespace Ui {
class DialogEditDeviceSettings;
}

class DialogEditDeviceSettings : public QDialog
{
    Q_OBJECT
private:
    static const char* PropertyID;

    uint8_t             m_DeviceAddress;
    sDeviceSettings     m_DeviceSettings;
    QVector<QWidget*>   m_SettingsWidgets;
    void clearList();
public:
    explicit DialogEditDeviceSettings(QWidget *parent = 0);
    ~DialogEditDeviceSettings();

    void exec(uint8_t deviceAddress);
private:
    Ui::DialogEditDeviceSettings *ui;
public slots:
    void processSettings(uint8_t deviceAddress, sDeviceSettings settings);
    void addressChanged(uint8_t deviceAddress, uint8_t newAddress);
signals:
    void requestSettings(uint8_t deviceAddress);
    void setSettings(uint8_t deviceAddress,sDeviceSettings settings);
    void setNewAddress(uint8_t deviceAddress, uint8_t deviceNewAddress);
private slots:
    void on_pushButtonSetNewAddress_clicked();
    void on_pushButtonReloadSettings_clicked();
    void on_pushButtonSetSettings_clicked();
private slots:
    void onSpinBoxSettingEdit();
};

#endif // DIALOGEDITDEVICESETTINGS_H
