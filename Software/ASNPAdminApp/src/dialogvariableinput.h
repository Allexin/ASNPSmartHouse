#ifndef DIALOGVARIABLEINPUT_H
#define DIALOGVARIABLEINPUT_H

#include <QDialog>

struct sDeviceVariable;

namespace Ui {
class DialogVariableInput;
}

class DialogVariableInput : public QDialog
{
    Q_OBJECT
protected:
    sDeviceVariable* m_Variable;
public:
    explicit DialogVariableInput(QWidget *parent = 0);
    ~DialogVariableInput();

    int exec(sDeviceVariable* dv);

private slots:
    void on_pushButton_Cancel_clicked();

    void on_pushButton_OK_clicked();

private:
    Ui::DialogVariableInput *ui;
};

#endif // DIALOGVARIABLEINPUT_H
