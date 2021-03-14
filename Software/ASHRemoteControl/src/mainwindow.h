#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QMainWindow>
#include "casnpclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    QString             m_CFGPin;
    bool                m_CFGEnableSettings;
protected:
    cASNPClient m_ASNPClient;

    int m_DeviceIndex;

    void showCategories();
    void showDevices(QString category);
    void showDimmer();
    void showGate();

    void showDevice(int index);

    void fixListItemSize(QListWidget* lw);

    void loadConfig();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void categoryClicked(QListWidgetItem *item);
    void deviceClicked(QListWidgetItem *item);


    void on_pushButton_DMXDimmer_OFF_clicked();
    void on_pushButton_DMXDimmer_ON_clicked();
    void on_horizontalSlider_Dimmer_valueChanged(int value);

    void on_pushButton_Back_clicked();

    void on_pushButton_Gate_UP_clicked();

    void on_pushButton_Gate_STOP_clicked();

    void on_pushButton_Gate_DOWN_clicked();

    void on_pushButton_Settings_clicked();

    void on_pushButton_PinOK_clicked();

    void on_pushButton_SettingsBack_clicked();

    void on_pushButton_SaveSettings_clicked();

    void on_pushButton_BackToCategories_clicked();



private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
