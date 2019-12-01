#include "dialogvariableinput.h"
#include "ui_dialogvariableinput.h"
#include "mainwindow.h"

DialogVariableInput::DialogVariableInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVariableInput)
{
    ui->setupUi(this);
}

DialogVariableInput::~DialogVariableInput()
{
    delete ui;
}

int DialogVariableInput::exec(sDeviceVariable* dv)
{
    m_Variable = dv;
    for (int i = 0; i<ui->tabWidget->count(); ++i)
    {
        ui->tabWidget->widget(i)->setVisible(false);
    }
    switch(m_Variable->info.type){
        case VT_UINT8: {
            ui->tab_uint8->setVisible(true);
            ui->spinBox_uint8->setValue(m_Variable->value[0]);
        };break;
        case VT_UINT16: {
            //TODO
        };break;
        case VT_UFIXFLOAT16: {
            //TODO
        };break;
        case VT_FIXFLOAT16: {
            //TODO
        };break;
        case VT_STRING16: {
            //TODO
        };break;
        case VT_LINK: {
            //TODO
        };break;
    }

    return QDialog::exec();
}

void DialogVariableInput::on_pushButton_Cancel_clicked()
{
    reject();
}

void DialogVariableInput::on_pushButton_OK_clicked()
{
    switch(m_Variable->info.type){
        case VT_UINT8: {
            m_Variable->value[0] = ui->spinBox_uint8->value();
        };break;
        case VT_UINT16: {
            //TODO
        };break;
        case VT_UFIXFLOAT16: {
            //TODO
        };break;
        case VT_FIXFLOAT16: {
            //TODO
        };break;
        case VT_STRING16: {
            //TODO
        };break;
        case VT_LINK: {
            //TODO
        };break;
    }
    accept();
}
