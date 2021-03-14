#include "dialogvariableinput.h"
#include "ui_dialogvariableinput.h"
#include "mainwindow.h"
#include <QTabBar>

DialogVariableInput::DialogVariableInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVariableInput)
{
    ui->setupUi(this);
    ui->tabWidget->tabBar()->hide();
}

DialogVariableInput::~DialogVariableInput()
{
    delete ui;
}

int DialogVariableInput::exec(sDeviceVariable* dv)
{
    m_Variable = dv;
    switch(m_Variable->info.type){
        case VT_UINT8: {            
            ui->tabWidget->setCurrentWidget(ui->tab_uint8);

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
            ui->tabWidget->setCurrentWidget(ui->tab_strin16);
            ui->lineEdit_string16->setText(QString::fromLatin1(m_Variable->value));
        };break;
        case VT_LINK: {
            //TODO
        };break;
        case VT_UINT8VECTOR4: {
            ui->tabWidget->setCurrentWidget(ui->tab_uint8vector4);

            ui->spinBox_uint8vector4_0->setValue(m_Variable->value[0]);
            ui->spinBox_uint8vector4_1->setValue(m_Variable->value[1]);
            ui->spinBox_uint8vector4_2->setValue(m_Variable->value[2]);
            ui->spinBox_uint8vector4_3->setValue(m_Variable->value[3]);
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
            QByteArray ba = ui->lineEdit_string16->text().toLatin1();
            if (ba.size()>16)
            {
                ba.resize(16);
            }
            while (ba.size()<16)
            {
                ba.push_back('\0');
            }
            for (int i = 0; i<16; i++)
            {
                m_Variable->value[i] = ba[i];
            }
        };break;
        case VT_LINK: {
            //TODO
        };break;
        case VT_UINT8VECTOR4: {
            m_Variable->value[0] = ui->spinBox_uint8vector4_0->value();
            m_Variable->value[1] = ui->spinBox_uint8vector4_1->value();
            m_Variable->value[2] = ui->spinBox_uint8vector4_2->value();
            m_Variable->value[3] = ui->spinBox_uint8vector4_3->value();
        };break;
    }
    accept();
}

void DialogVariableInput::on_lineEdit_string16_textChanged(const QString &)
{
    QByteArray ba = ui->lineEdit_string16->text().toLatin1();
    if (ba.size()!=16)
    {
        ui->label_String16validationInfo->setText(tr("invalid value, raw length(include unicode multibyte chars) must be 16 chars, but now its %1. String will be cutted to 16 bytes and empty chars will be filled with \\0").arg(ba.size()));
    }
    else
    {
        ui->label_String16validationInfo->setText("");
    }
}
